#include "graphics.h"

#include "config.h"

#ifdef HAVE_GL_GLUT_H
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_array.hpp>

#include <iostream>
#include <cmath>

#include "graphics.pb.h"
#include "rpc.h"

namespace {
  const int DEFAULT_WINDOW_WIDTH=800;
  const int DEFAULT_WINDOW_HEIGHT=600;
  const string DefaultPort("1234");
}

using namespace boost;
using namespace std;

Map* Map::map = NULL;
Graphics *Graphics::g = NULL;

void Draw(const ColorMessage &m) {
  glColor3i(m.r(), m.g(), m.b());
}

void Draw(const PointSizeMessage &m) {
  glPointSize(m.sz());
}

void Draw(const CircleMessage &m) {
	glBegin(GL_LINE_LOOP);
	for(double ang = 0.0; ang < 2*M_PI; ang += .1 )
	    glVertex2d(m.cx() + m.radius()*cos(ang), m.cy()+m.radius()*sin(ang));
	glEnd();
}

void Draw(const PointMessage &m) {
	glBegin(GL_POINTS);
	glVertex2d(m.x(),m.y());
	glEnd();
}

void Draw(const LineMessage &m) {
	glBegin(GL_LINES);
	glVertex2d(m.x0(),m.y0());
	glVertex2d(m.x1(),m.y1());
	glEnd();
}

void Map::DrawMap(const MapMessage& m) {
  if (m.has_data()) {
    if (map) delete map;
    map = new Map(m);
  } else if (map) {
    map->xOrig = m.xcenter();
    map->yOrig = m.ycenter();
    map->scale = m.scale();
    map->xres = m.xres();
    map->yres = m.yres();
  }
  if (map) {
    map->Draw();
  } else {
    // cout << "DrawMap with no map :(" << endl;
  }
}

Graphics::Graphics() : xCenter(0.0), yCenter(0.0), scale(10), bgR(0.0), bgG(0.0), bgB(0.0), isDirty(false), cmd_process(NULL), cmd_wait(NULL)
{
}

Graphics *Graphics::Instance()
{
  if( !g )
    g = new Graphics;
  return g;
}

void Graphics::Init(int &argc, char **argv)
{
  thread(bind(&Graphics::MainLoop, this, argc, argv));
}

void Graphics::MainLoop(int &argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitWindowSize( DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT );
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
  glutCreateWindow( "CS225B Graphics Window"  );

  glutDisplayFunc(display_func);
  glutReshapeFunc(reshape_func);
  glutIdleFunc(idle_func);

  glutMainLoop();
}
void Graphics::DrawTrans(
    GraphicsTransaction* t) {
  for(int i = 0; i < t->message_size(); i++) {
    const GraphicsMessage& m = t->message(i);
    if (m.has_color()) Draw(m.color());
    if (m.has_point_size()) Draw(m.point_size());
    if (m.has_circle()) Draw(m.circle());
    if (m.has_point()) Draw(m.point());
    if (m.has_line()) Draw(m.line());
    if (m.has_scale()) scale = m.scale().scale();
    if (m.has_map()) Map::DrawMap(m.map());
  }
}

void Graphics::Display()
{
  glClearColor(bgR, bgG, bgB, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();

  //glEnable(GL_POINT_SMOOTH);

  glScaled(1.0/scale, 1.0/scale, 1.0/scale);
  glTranslated(-xCenter, -yCenter, 0.0);

  for (int i = 0; i < persistent.size(); i++) {
    GraphicsTransaction* pers = persistent[i];
    DrawTrans(pers);
  }
  if (cmd_process) {
    DrawTrans(cmd_process);
  }

  glutSwapBuffers();

  //cout << "Display" << endl;
  //usleep(100000);
}

void Graphics::Reshape(int width, int height)
{
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-1.0, 1.0, -double(height)/width, double(height)/width);

  glMatrixMode(GL_MODELVIEW);

}

void Graphics::Idle()
{
  if( isDirty ) {
    //lock and copy data
    mutex::scoped_lock lock(waitQueueMutex);
    if (cmd_wait) {
      if (cmd_process) delete cmd_process;
      cmd_process = cmd_wait;
      cmd_wait = NULL;
      isDirty=false;
    }
  }
  glutPostRedisplay();
}

void Graphics::display_func()
{
  Graphics::Instance()->Display();
}
void Graphics::reshape_func(int width, int height)
{
  Graphics::Instance()->Reshape(width, height);
}
void Graphics::idle_func()
{
  Graphics::Instance()->Idle();
}

void Graphics::ProcessTransaction(GraphicsTransaction* t)
{
  mutex::scoped_lock lock(waitQueueMutex);
  if (t->persistent()) {
    persistent.push_back(t);
    return;
  }
  if (cmd_wait) {
    delete cmd_wait;
  }
  cmd_wait = t;
  isDirty=true;
}


  Map::Map(const MapMessage &m)
: xOrig(m.xcenter()), yOrig(m.ycenter()), scale(m.scale()),
  xres(m.xres()), yres(m.yres()), texture(0)
{
  data = new unsigned char[xres * yres];
  string d = m.data();
  for (int i = 0; i < xres * yres; i++) {
    data[i] = d[i];
  }
}

void Map::Draw()
{
  glColor3d(1.0,1.0,1.0);

  if( texture == 0 )
    LoadTexture();   

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);

  glBegin(GL_QUADS);
  glTexCoord2d(0,0);
  glVertex2d(xOrig, yOrig);

  glTexCoord2d(texScaleX, 0);
  glVertex2d(xOrig + xres*scale, yOrig);

  glTexCoord2d(texScaleX, texScaleY);
  glVertex2d(xOrig + xres*scale, yOrig + yres*scale);

  glTexCoord2d(0, texScaleY);
  glVertex2d(xOrig, yOrig + yres*scale);
  glEnd();

  glDisable(GL_TEXTURE_2D);
}

void Map::LoadTexture()
{
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
      GL_LINEAR_MIPMAP_NEAREST );

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

  int pixSz = 1;
  while( pixSz < xres || pixSz < yres )
    pixSz *= 2;

  texScaleX = double(xres)/pixSz;
  texScaleY = double(yres)/pixSz;

  shared_array<unsigned char> pow2data( new unsigned char[pixSz*pixSz] );
  for(int j = 0; j < yres; ++j )
    for(int i = 0; i < xres; ++i )
      pow2data[i+j*pixSz] = data[i+j*xres];


  gluBuild2DMipmaps( GL_TEXTURE_2D, 1, pixSz, pixSz,
      GL_LUMINANCE, GL_UNSIGNED_BYTE, pow2data.get());
}

void NetworkThreadRun(string port) {
  Graphics* g = Graphics::Instance();
  try {
    RPC* rpc = RPC::CreateServer(port);
    while (true) {
      string* msg = rpc->PollMessage(true);
      if (msg) {
        GraphicsTransaction* trans = new GraphicsTransaction;
        trans->ParseFromString(*msg);
        g->ProcessTransaction(trans);
        delete msg;
      }
    }
  } catch (RPCException* ex) {
    cout << "RPCException: " << ex->description() << endl;
  }
}

int main(int argc, char** argv) {
  string port = DefaultPort;
  if (argc == 2) {
    port = argv[1];
  }
  Graphics* g = Graphics::Instance();
  thread(bind(NetworkThreadRun, port));
  g->MainLoop(argc, argv);
  return 0;
}