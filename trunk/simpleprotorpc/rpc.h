#ifndef _RPC_H_
#define _RPC_H_

#include <iostream>
#include <deque>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <utility>

#include <boost/thread/mutex.hpp>

#include "util.h"

using namespace std;

namespace simpleprotorpc {
class RPCException : public StreamableException {
 public:
  RPCException() {}
  explicit RPCException(string description) : StreamableException(description) {}
  RPCException(const RPCException& rhs) : StreamableException(rhs) {}
};

/** 
 * RPC wraps a connection to another host, used to pass
 * strings encapsulating Google protocol buffers
 * (http://code.google.com/p/protobuf/) from one endpoint
 * to the other. Communication goes both ways, and the RPC
 * name is perhaps a misnomer, although the main flow of information
 * in the Graphics client is similar to the method calls in the
 * provided graphics library.
 */
class RPC {
 public:

  enum AsyncSendPolicy {
    // Send all packets
    SEND_ALL,
    // Send only the last packet in the queue
    SEND_LAST
  };
  
  /// Connect to the specified host/port.
  static RPC* CreateClient(string host, string port);
  
  /// Listen for a RPC connection on the provided port.
  static RPC* CreateServer(string port);

  /** 
   * Sends string msg, either synchronously or asynchronously.
   * The order of delivery is guaranteed among messages sent by
   * either method, but not when the methods are mixed.
   */
  void SendMessage(string& msg, bool async, bool ensure_sent = false);

  /** 
   * Returns either a pointer to a dynamically allocated string
   * to a received message or NULL if blocking is false and there
   * is no message waiting.
   */
  string* PollMessage(bool blocking);

  /**
   * Sets the current asynchronous send policy.
   * We always make the following two guarantees:
   * * Messages will be sent over the wire in the order they were sent to
   *   SendMessage.
   * * Given sufficient bandwidth, all messages will be sent).
   *
   * However, there is the obvious problem that in the case where
   * there is insufficient bandwidth, it is impossible to both
   * send all messages and to provide reasonably quick turnaround on
   * messages. So, we provide two policies:
   *   RPC::SEND_ALL ensures that all messages are sent.
   *   RPC::SEND_LAST sends only the most recent packet in the queue
   *     at the time that the previous message finishes sending.
   */
  void SetSendPolicy(AsyncSendPolicy new_send_policy); 
  
  // Not currently implemented.
  // void StartAsyncRecv(void (*callback)(string*));
  // void StopAsyncRecv();
  
 protected:
  /** 
   * Send messages that were given to RPC for asynchronous delivery
   * and have been waiting in the queue.
   */
  void AsyncSend();

 private:
  RPC();
  RPC(string port);
  RPC(string host, string port);
  RPC(RPC& r);

  deque<pair<string*, bool> > messages;
  boost::mutex messages_mutex;
  boost::mutex sending_mutex;

  deque<int> sock_list;
  string host;
  string port;
  bool async;
  bool server;
  int server_sock;
  int max_fd;

  AsyncSendPolicy send_policy;
  boost::mutex send_policy_mutex;
};
}
#endif
