#include <simpleprotographics/graphics.pb.h>
#include <simpleprotorpc/rpc.h>

int main(int argc, char** argv) {
  RPC* rpc = RPC::CreateClient("localhost", "1234");
  GraphicsTransaction t;
  LineMessage* m = t.add_message()->mutable_line();
  m->set_x0(0.0);
  m->set_x1(1.0);
  m->set_y0(0.0);
  m->set_y1(1.0);
  string foo;
  t.SerializeToString(&foo);
  rpc->SendMessage(foo, false);
  return 0;
}
