#ifndef _RPC_H_
#define _RPC_H_

#include <iostream>
#include <sstream>
#include <deque>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <boost/thread/mutex.hpp>

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

using namespace std;

// A StreamableException allows the programmer to specify
// the exception's description via a stream interface.
class StreamableException : public exception {
 public:
  StreamableException();
  explicit StreamableException(string description);
  virtual ~StreamableException() throw ();
  virtual ostringstream& stream();
  virtual string description();
 protected:
  ostringstream descstr;
 private:
  DISALLOW_COPY_AND_ASSIGN(StreamableException);
};

class RPCException : public StreamableException {
 public:
  RPCException() {}
  explicit RPCException(string description) : StreamableException(description) {}
 private:
  DISALLOW_COPY_AND_ASSIGN(RPCException);
};

// RPC wraps a connection to another host, used to pass
// strings encapsulating Google protocol buffers
// (http://code.google.com/p/protobuf/) from one endpoint
// to the other. Communication goes both ways, and the RPC
// name is perhaps a misnomer, although the main flow of information
// in the Graphics client is similar to the method calls in the
// provided graphics library.
class RPC {
 public:
  // Connect to the specified host/port.
  static RPC* CreateClient(string host, string port);
  
  // Listen for a RPC connection on the provided port.
  static RPC* CreateServer(string port);
  
  // Sends string msg, either synchronously or asynchronously.
  // The order of delivery is guaranteed among messages sent by
  // either method, but not when the methods are mixed.
  void SendMessage(string& msg, bool async);

  // Returns either a pointer to a dynamically allocated string
  // to a received message or NULL if blocking is false and there
  // is no message waiting.
  string* PollMessage(bool blocking);
  
  // Not appearing in this assignment.
  void StartAsyncRecv(void (*callback)(string*));
  void StopAsyncRecv();
 protected:
  // Send messages that were given to RPC for asynchronous delivery
  // and have been waiting in the queue.
  void AsyncSend();
 private:
  RPC();
  RPC(string port);
  RPC(string host, string port);
  RPC(RPC& r);

  deque<string*> messages;
  boost::mutex messages_mutex;
  boost::mutex sending_mutex;

  deque<int> sock_list;
  string host;
  string port;
  void (*cbfn)(string*);
  bool async;
  bool server;
  int server_sock;
  int max_fd;
};

#endif
