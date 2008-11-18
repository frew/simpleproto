import sys

def CheckGL(context):
  # Hack to avoid Result error
  context.did_show_result = True
  found_header = False
  if context.sconf.CheckHeader('GL/gl.h', language='C++'):
    context.env.AppendUnique(CXXFLAGS='-DHAVE_GL_GL_H')
    found_header = True
  if context.sconf.CheckHeader('OpenGL/gl.h', language='C++'):
    context.env.AppendUnique(CXXFLAGS='-DHAVE_OPENGL_GL_H')
    found_header = True
  if not found_header:
    return False

  if sys.platform.find('darwin') != -1:
    context.env.AppendUnique(FRAMEWORKS = ['Cocoa', 'OpenGL'])
  else:
    if context.sconf.CheckLib('GL'):
      context.env.AppendUnique(LIBS = ['GL']) 
    elif context.sconf.CheckLib('opengl32'):
      context.env.AppendUnique(LIBS = ['opengl32'])
    else:
      return False
  return True

def CheckGLU(context):
  context.did_show_result = True
  found_header = False
  if context.sconf.CheckHeader('GL/glu.h', language='C++'):
    context.env.AppendUnique(CXXFLAGS='-DHAVE_GL_GLU_H')
    found_header = True
  if context.sconf.CheckHeader('OpenGL/glu.h', language='C++'):
    context.env.AppendUnique(CXXFLAGS='-DHAVE_OPENGL_GLU_H')
    found_header = True
  if not found_header:
    return False

  if sys.platform.find('darwin') == -1:
    if context.sconf.CheckLib('GLU'):
      context.env.AppendUnique(LIBS = ['GLU']) 
    elif context.sconf.CheckLib('glu32'):
      context.env.AppendUnique(LIBS = ['glu32'])
    else:
      return False
  return True

def CheckGLUT(context):
  context.did_show_result = True
  found_header = False
  if context.sconf.CheckHeader('GL/glut.h', language='C++'):
    context.env.AppendUnique(CXXFLAGS='-DHAVE_GL_GLUT_H')
    found_header = True
  if context.sconf.CheckHeader('GLUT/glut.h', language='C++'):
    context.env.AppendUnique(CXXFLAGS='-DHAVE_GLUT_GLUT_H')
    found_header = True
  if not found_header:
    return False

  if sys.platform.find('darwin') != -1:
    context.env.AppendUnique(FRAMEWORKS = ['GLUT'])
  else:
    if context.sconf.CheckLib('glut'):
      context.env.AppendUnique(LIBS = ['glut']) 
    elif context.sconf.CheckLib('glut32'):
      context.env.AppendUnique(LIBS = ['glut32'])
    else:
      return False
  return True
