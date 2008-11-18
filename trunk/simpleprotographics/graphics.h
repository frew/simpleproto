#ifndef _SIMPLEPROTOGRAPHICS_GRAPHICS_H_
#define _SIMPLEPROTOGRAPHICS_GRAPHICS_H_

#include <simpleprotorpc/rpc.h>
#include <boost/function.hpp>
#include <boost/thread.hpp>

#include "graphics.pb.h"

namespace simpleprotographics {
class Graphics {
 public:
  /**
   * Connect to a graphics server running at host:port.
   */
  Graphics(std::string host, std::string port);
  ~Graphics();
  
  /// Sets the current color based on a RGB values from 0.0 to 1.1.
  void SetColor(double r, double g, double b);

  /// Draw a point at (x, y).
  void DrawPoint(double x, double y);

  /// Draw a line from (x0, y0) to (x1, y1). 
  void DrawLine(double x0, double y0, double x1, double y1);

  /// Set the size of any subsequent points.
  void SetPointSize(double sz); 

  /// Draw a circle centered at (cx, cy) with the given radius.
  void DrawCircle(double cx, double cy, double radius);

  /// Set the scale for this and later frames.
  void SetScale(double scale);

  /// Draw a map with the given data.
  /// TODO(frew): Figure out why this isn't working.
  void DrawMap(double xcenter, double ycenter, double scale, double xres,
               double yres, unsigned char* data);

  /// Send the current frame off to the graphics server.
  /// @arg: persistent - if true, then the items in the current frame will be drawn
  ///   when any subsequent frame is rendered. This is useful for static content
  //    to cut down on RPC traffic. The items in the current frame are drawn before
  //    the items in the subsequent frames. If multiple persistent frames are sent,
  //    they are each drawn in the order that they were received.
  void DrawFrame(bool persistent=false);

  enum MouseButton {
    LEFT_BUTTON,
    RIGHT_BUTTON,
    MIDDLE_BUTTON
  };

  void RegisterMouseCallback(
      boost::function<void (MouseButton button, bool down, int x, int y, 
                            bool shift_down, bool alt_down, bool ctrl_down)>);

  /// Get at the current transaction. Not recommended unless you have a good idea
  /// of the behind the scenes functionality of the library.
  GraphicsTransaction* current_transaction();

 protected:
  void InputCallback(string* s);

 private:
  bool enabled;
  simpleprotorpc::RPC* conn;
  GraphicsTransaction cur_trans;
  boost::mutex graphics_mutex;
  // TODO(frew): Unlikely race
  boost::function<void (MouseButton button, bool down, int x, int y, 
                  bool shift_down, bool ctrl_down, bool alt_down)> 
      mouse_callback;
};
}

#endif /* GRAPHICS_CLIENT_H_ */
