#ifndef CS225B_GRAPHICS_H
#define CS225B_GRAPHICS_H

#include <string>
#include <boost/thread/mutex.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>
#include <map>

#include "graphics.pb.h"

using namespace std;

// This provides almost exactly the same functionality
// as the default wander-graphics package, but set up
// in such a way that it's running as an RPC server.

class Map
{
public:
    Map(const MapMessage &m);
    void Draw();
    static void DrawMap(const MapMessage &m);
private:
    double xOrig, yOrig;
    double scale;

    int xres, yres;
    unsigned char* data;

    double texScaleX, texScaleY;

    void LoadTexture();

    unsigned int texture;
    static Map* map;
};

// The main graphics class.
class Graphics
{
 public:
  static Graphics *Instance(); // get an instace of the global graphics object

  // Initialize the graphics system and start the graphics thread
  void Init(int &argc, char **arvg);
  
  void ClearMap() { map.reset(); }

  void SetBackgroundColor(double r, double g, double b) {
      bgR = r;
      bgB = b;
      bgG = g;
  }

  // Sets the center of the display window, in world coordinates
  void SetCenter(double xcenter, double ycenter) {
      xCenter = xcenter;
      yCenter = ycenter;
  }
  
  void ProcessTransaction(GraphicsTransaction* trans);

  void MainLoop(int &argc, char **argv);
private:
  Graphics();


  double xCenter, yCenter;
  double scale;

  boost::shared_ptr<Map> map;
 
  void Display();
  void Reshape(int width, int height);
  void Idle();

  void DrawTrans(GraphicsTransaction* t);
  
  static void display_func();
  static void reshape_func(int width, int height);
  static void idle_func();

  static Graphics *g;

  boost::mutex waitQueueMutex;

  double bgR, bgG, bgB;

  bool isDirty;
  GraphicsTransaction* cmd_process;
  GraphicsTransaction* cmd_wait;
  deque<GraphicsTransaction*> persistent;
};

#endif
