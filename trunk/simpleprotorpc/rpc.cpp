#include "rpc.h"

#include <exception>
#include <cstring>
#include <stdio.h>
#include <errno.h>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

namespace simpleprotorpc {
namespace {
  // Throws an exception if result is non-zero.
  void CheckAddrInfo(int result) {
    if (result != 0) {
      RPCException ex;
      ex.stream() << "AddrInfo failure: "
                  << gai_strerror(result);
      throw ex;
    }
  }

  // Throws an exception with error message error_msg if result
  // is non-zero.
  void CheckErrno(int result, const char* error_msg) {
    if (result == -1) {
      RPCException ex;
      ex.stream() << error_msg << " (result: " << result 
                  << "errno: " << errno << ")";
      throw ex;
    }
  }

  void CleanupSocket(int sock_to_delete, deque<int>* sock_list) {
    for (deque<int>::iterator it = sock_list->begin();
        it != sock_list->end();
        ++it) {
      if (*it == sock_to_delete) {
        sock_list->erase(it);
        break;
      }
    }
  }
}

RPC* RPC::CreateClient(string host, string port) {
  RPC* rpc = new RPC(host, port);  
  return rpc;
}

RPC* RPC::CreateServer(string port) {
  RPC* rpc = new RPC(port);
  return rpc;
}

RPC::RPC(string port) : port(port), async_send_thread_started(false),
                        server(true), send_policy(SEND_ALL), 
                        async_recv_thread_started(false) {
  addrinfo hints, *res;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC; // IPvWhatever
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // Whatever IP
  CheckAddrInfo(getaddrinfo(NULL, port.c_str(), &hints, &res));
 
  server_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  CheckErrno(server_sock, "Error creating server sock.");
  max_fd = server_sock;
  CheckErrno(bind(server_sock, res->ai_addr, res->ai_addrlen), "Error binding server sock.");
  // 10 = how many connections to let queue
  CheckErrno(listen(server_sock, 10), "Error listening on server sock.");
}

RPC::RPC(string host, string port) : host(host), port(port), 
                                     async_send_thread_started(false), 
                                     server(false), send_policy(SEND_ALL),
                                     async_recv_thread_started(false) {
  addrinfo hints, *res;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  CheckAddrInfo(getaddrinfo(host.c_str(), port.c_str(), &hints, &res));
  
  int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  CheckErrno(sockfd, "Error creating client sock.");
  max_fd = sockfd;
  CheckErrno(connect(sockfd, res->ai_addr, res->ai_addrlen), "Error connecting to server.");
  sock_list.push_back(sockfd);
}

void RPC::SetSendPolicy(RPC::AsyncSendPolicy new_send_policy) {
  boost::mutex::scoped_lock l(send_policy_mutex);
  send_policy = new_send_policy;
}

void RPC::AsyncSend() {
  bool found = true;
  while (true) {
    if (!found) usleep(10 * 1000);
    found = true;
    string* cur_string;
    {
      boost::mutex::scoped_lock l(messages_mutex);
      if (messages.size() == 0) {
        found = false;
        continue;
      }
      {
        boost::mutex::scoped_lock ll(send_policy_mutex);
        switch (send_policy) {
          case SEND_LAST:
            if (messages.size() > 2) {
              cerr << "Queue backup: " << messages.size() << endl;
              while (messages.size() > 1) {
                // Check for ensured delivery
                if (messages.front().second) break;
                messages.pop_front(); 
              }
            }
            break;
          case SEND_ALL:
            break;
        }
      }
      cur_string = messages.front().first;
      messages.pop_front();
    }
    SendMessage(*cur_string, false);
    delete cur_string;
  }
}

void RPC::SendMessage(string& msg, bool send_async, bool ensure_sent) {
  if (send_async) {
    if (!async_send_thread_started) {
      // Start the async thread.
      boost::thread(boost::bind(&RPC::AsyncSend, this));
      async_send_thread_started = true;
    }
    boost::mutex::scoped_lock l(messages_mutex);
    messages.push_back(pair<string*, bool>(new string(msg), ensure_sent)); 
  } else {
    boost::mutex::scoped_lock l(sending_mutex);
    for (deque<int>::iterator it = sock_list.begin();
         it != sock_list.end();
         ++it) {
      int sock = *it;
      int msglen = msg.size();
      msglen = htonl(msglen);
      int num_sent = send(sock, &msglen, 4, 0);
      CheckErrno(num_sent, "Error sending length.");
      if (num_sent != 4) {
        RPCException ex;
        ex.stream() << "Send length failed: num_sent " << num_sent << " != 4!";
        throw ex;
      }
      unsigned int sent_so_far = 0;
      while (sent_so_far != msg.size()) {
        num_sent = send(sock, msg.c_str() + sent_so_far,
                        msg.size() - sent_so_far, 0);
        CheckErrno(num_sent, "Error sending message.");
        sent_so_far += num_sent;
      }
    }
  }
}

string* RPC::PollMessage(bool blocking) {
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  fd_set socks;
  FD_ZERO(&socks);
  for (deque<int>::iterator it = sock_list.begin();
       it != sock_list.end();
       ++it) {
    FD_SET(*it, &socks);
  }
  if (server) {
    FD_SET(server_sock, &socks);
  }
  CheckErrno(select(max_fd + 1, &socks, NULL, NULL, blocking ? NULL : &tv),
             "Select failed."); 
  if (server && FD_ISSET(server_sock, &socks)) {
    socklen_t addr_size = sizeof(sockaddr_storage);
    sockaddr_storage their_addr;
    int new_fd = accept(server_sock, (struct sockaddr *)&their_addr, &addr_size); 
    CheckErrno(new_fd, "Error accepting server socket.");
    if (new_fd > max_fd) max_fd = new_fd;
    cerr << "Adding " << new_fd << " to sock_list" << endl;
    sock_list.push_back(new_fd);
    return PollMessage(blocking);
  }
  int ss = -1;
  for (deque<int>::iterator it = sock_list.begin();
       it != sock_list.end();
       ++it) {
    if (FD_ISSET(*it, &socks)) {
      ss = *it;
      break;
    }
  }
  if (ss == -1) return NULL;

  int msglen;
  int num_read = recv(ss, &msglen, 4, 0); 
  if (num_read != 4) {
    // TODO(frew): error handling for this
    cerr << "Uh oh...msg length read " << num_read << " != 4." 
         << " Killing socket." << endl;
    CleanupSocket(ss, &sock_list);
    return PollMessage(blocking);
  }
  msglen = htonl(msglen);
  // TODO(frew): scoped_ptr
  char * buf = new char[msglen];
  int read_so_far = 0;
  while (read_so_far < msglen) {
    num_read = recv(ss, buf + read_so_far, msglen - read_so_far, 0);
    if (num_read <= 0) {
      cerr << "Got " << num_read << " from num_read. Killing socket. :(" << endl;
      CleanupSocket(ss, &sock_list);
      delete[] buf;
      return PollMessage(blocking);
    }
    read_so_far += num_read;
  }
  string* ret_string = new string(buf, msglen);
  delete[] buf;
  return ret_string;
}

void RPC::AsyncRecv() {
  while (async_recv_thread_started) {
    string* msg = PollMessage(true);
    async_recv_callback(msg);
    delete msg;
  }
}

void RPC::StartAsyncRecv(boost::function<void (string* s)> callback) {
  async_recv_callback = callback;
  if (!async_recv_thread_started) {
    async_recv_thread_started = true;
    boost::thread(boost::bind(&RPC::AsyncRecv, this));
  }
}

void RPC::StopAsyncRecv() {
  async_recv_thread_started = false;
}

}
