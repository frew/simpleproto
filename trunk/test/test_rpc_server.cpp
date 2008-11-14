#include <simpleprotorpc/rpc.h>

using namespace simpleprotorpc;

int main(int argc, char** argv) {
  RPC* rpc = RPC::CreateServer("1234");
  while (true) {
    string* msg = rpc->PollMessage(true);
    if (msg) {
      cout << *msg << endl;
    }
  }
  return 0;
}
