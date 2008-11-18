#include "graphics.h"

#include <limits.h>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

namespace simpleprotographics {

using namespace simpleprotorpc;

Graphics::Graphics(string host, string port)
  : enabled(host != "disabled"), mouse_callback(NULL)
{
  if (!enabled) return;
  conn = RPC::CreateClient(host, port);
  conn->SetSendPolicy(RPC::SEND_LAST);
  conn->StartAsyncRecv(boost::bind(&Graphics::InputCallback, this, _1));
}

Graphics::~Graphics() {
  if (!enabled) return;
  delete conn;
}

void Graphics::SetColor(double r, double g, double b)
{
  if (!enabled) return;
  boost::mutex::scoped_lock l(graphics_mutex);
  ColorMessage* m = cur_trans.add_message()->mutable_color();
  m->set_r((int) (r * INT_MAX));
  m->set_g((int) (g * INT_MAX));
  m->set_b((int) (b * INT_MAX));
}

void Graphics::DrawPoint( double x, double y )
{
  if (!enabled) return;
  boost::mutex::scoped_lock l(graphics_mutex);
  PointMessage* m = cur_trans.add_message()->mutable_point();
  m->set_x(x);
  m->set_y(y);
}

void Graphics::DrawLine( double x0, double y0, double x1, double y1 )
{
  if (!enabled) return;
  boost::mutex::scoped_lock l(graphics_mutex);
  LineMessage* m = cur_trans.add_message()->mutable_line();
  m->set_x0(x0);
  m->set_y0(y0);
  m->set_x1(x1);
  m->set_y1(y1);
}

void Graphics::SetPointSize( double sz ) 
{
  if (!enabled) return;
  boost::mutex::scoped_lock l(graphics_mutex);
  PointSizeMessage* m = cur_trans.add_message()->mutable_point_size();
  m->set_sz(sz);
}

void Graphics::DrawCircle(double cx, double cy, double radius)
{
  if (!enabled) return;
  boost::mutex::scoped_lock l(graphics_mutex);
  CircleMessage* m = cur_trans.add_message()->mutable_circle();
  m->set_cx(cx);
  m->set_cy(cy);
  m->set_radius(radius);
}

void Graphics::SetScale(double scale) {
  if (!enabled) return;
  boost::mutex::scoped_lock l(graphics_mutex);
  ScaleMessage* m = cur_trans.add_message()->mutable_scale();
  m->set_scale(scale);
}

void Graphics::DrawMap(
    double xcenter, 
    double ycenter, 
    double scale,
    double xres,
    double yres,
    unsigned char* data) {
  MapMessage* m = cur_trans.add_message()->mutable_map();
  m->set_xcenter(xcenter);
  m->set_ycenter(ycenter);
  m->set_scale(scale);
  m->set_xres(xres);
  m->set_yres(yres);
  static unsigned char* old_data = NULL;
  if (old_data != data) {
    m->set_data(data, xres * yres);
    old_data = data;
    cout << "Map with new data" << endl;
  }
}

void Graphics::DrawFrame(bool persistent)
{
  if (!enabled) return;
  boost::mutex::scoped_lock l(graphics_mutex);
  if (persistent) cur_trans.set_persistent(persistent);
  string msg;
  cur_trans.SerializeToString(&msg);
  conn->SendMessage(msg, false, !cur_trans.persistent());
  cur_trans.Clear();
}

void Graphics::RegisterMouseCallback(
    boost::function<void (MouseButton button, bool down, double x, double y, 
                     bool shift_down, bool ctrl_down, bool alt_down)> callback) {
  mouse_callback = callback;
}

void Graphics::InputCallback(string* s) {
  // TODO(frew): Add support for events other than mouse
  if (mouse_callback != NULL) {
    MouseEvent e;
    e.ParseFromString(*s);
    MouseButton button;
    switch (e.button()) {
      case MouseEvent::LEFT_BUTTON:
        button = LEFT_BUTTON;
        break;
      case MouseEvent::MIDDLE_BUTTON:
        button = MIDDLE_BUTTON;
        break;
      case MouseEvent::RIGHT_BUTTON:
        button = RIGHT_BUTTON;
        break;
    }
    mouse_callback(button, e.state() == MouseEvent::DOWN, e.x(), e.y(),
                   e.shift_down(), e.ctrl_down(), e.alt_down());
  }
}

GraphicsTransaction* Graphics::current_transaction() {
  return &cur_trans;
}
}
