#ifndef _GRAPHICS_CLIENT_H_
#define _GRAPHICS_CLIENT_H_

#include <simpleprotorpc/rpc.h>

#include "graphics.pb.h"

using namespace std;

// This is all stuff that in a non-crappy RPC system would be auto-generated
class GraphicsClient {
 public:
  GraphicsClient(string host, string port);
  ~GraphicsClient();
  void SetColor(double r, double g, double b);
  void DrawPoint( double x, double y );
  void DrawLine( double x0, double y0, double x1, double y1 );
  void SetPointSize( double sz ); 
  void DrawCircle(double cx, double cy, double radius);
  void SetScale(double scale);
  void DrawMap(double xcenter, double ycenter, double scale, double xres,
               double yres, unsigned char* data);
  void DrawFrame();
  GraphicsTransaction* current_transaction();
 private:
  bool enabled;
  RPC* conn;
  GraphicsTransaction cur_trans;
};

#endif /* GRAPHICS_CLIENT_H_ */
