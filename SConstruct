from os import listdir
from os.path import exists, isdir, join
import re
import sys

AddOption('--prefix',
          dest='prefix',
          type='string',
          nargs=1,
          action='store',
          metavar='DIR',
          default='#export',
          help='Installation prefix')

AddOption('--boost-prefix',
          dest='boost_prefix',
          type='string',
          nargs=1,
          action='store',
          metavar='DIR',
          help='boost installation prefix')

AddOption('--boost-libdir',
          dest='boost_libdir',
          type='string',
          nargs=1,
          action='store',
          metavar='DIR',
          help='boost library dir')

include = GetOption('prefix') + '/include'
lib = GetOption('prefix') + '/lib'
bin = GetOption('prefix') + '/bin'

env = Environment(BINDIR = bin,
                  LIBDIR = lib,
                  CPPPATH = [include],
                  LIBPATH = [lib],
                  LIBS=['protobuf'])
env.Alias('server', bin + '/graphics_server')
env.Alias('libs', [lib, include])
env.Alias('tests', 'build/test')
env.Default(GetOption('prefix'))

if ARGUMENTS.get('PROD', 0):
  env.AppendUnique(CXXFLAGS=['-O2',])
else:
  env.AppendUnique(CXXFLAGS=['-g','-Wall'])

def boost_header_version(version_string):
  vl = [int(x) for x in version_string.split('.')]
  return vl[0] * 100000 + vl[1] * 100 + (vl[2] if len(vl) > 2 else 0)

def boost_version_string(header_version):
  header_version = int(header_version)
  return '%d.%d.%d' % (header_version / 100000, header_version / 100 % 1000, header_version % 100) 

def boost_lib_version_string(header_version):
  header_version = int(header_version)
  return '%d_%d' % (header_version / 100000, header_version / 100 % 1000)

def CheckBoost(context):
  prefixes = ['/usr', '/usr/local', '/opt', '/opt/local']

  if GetOption('boost_prefix') is not None:
    prefixes = [GetOption('boost_prefix')]

  best_prefix = None
  best_incdir = None
  best_libsuffix = None
  best_version = 0

  context.Message('Checking for Boost include files...')
  for prefix in prefixes:
    incdir = join(prefix, 'include')

    if isdir(incdir):
      boost_incdirs = [dir for dir in listdir(incdir) if dir.startswith('boost')]
      for dir in boost_incdirs:
        if dir == 'boost':
          current_incdir = incdir
        else:
          current_incdir = join(incdir, dir)
        version_header = join(incdir, 'boost', 'version.hpp')
        if exists(version_header):
          version_file = open(version_header)
          for line in version_file:
            m = re.search(r'#define BOOST_VERSION (\d+)', line) 
            if m:
              current_version = int(m.group(1))
              if current_version > best_version:
                best_incdir = current_incdir
                best_version = current_version
                best_prefix = prefix
          version_file.close()

  context.Result(best_incdir is not None)
  if best_incdir is None:
    return False

  print 'Found boost version %s' % boost_version_string(best_version)
  
  context.env.AppendUnique(CPPPATH = best_incdir) 
  if not context.sconf.CheckHeader('boost/version.hpp', language='C++'):
    return False

  context.Message('Checking for Boost libs...')
  if GetOption('boost_libdir') is not None:
    libdir = GetOption('boost_libdir')
  else:
    libdir = join(best_prefix, 'lib')

  if isdir(libdir):
    boost_libs = [lib for lib in listdir(libdir) if lib.startswith('libboost_thread')]
    for lib in boost_libs:
      m = re.search(r'libboost_thread([^.]*)\.', lib)
      if m is not None:
        if lib.find(boost_lib_version_string(best_version)) != -1 or best_libsuffix is None:
          best_libsuffix = lib[len('libboost_thread'):lib.rfind('.')]
 
  context.Result(best_libsuffix is not None)
  if best_libsuffix is None:
    return False

  print 'Found boost lib %s' % join(libdir, 'libboost*%s' % best_libsuffix)
  context.env.AppendUnique(LIBPATH = libdir)
  context.env.Replace(BOOST_SUFFIX = best_libsuffix)
  context.env.AppendUnique(LIBS = 'boost_thread' + env['BOOST_SUFFIX'])

  if not context.sconf.CheckLib('boost_thread' + best_libsuffix):
     return False
  return True

def CheckGL(context):
  # Hack to avoid Result error
  context.did_show_result = True
  found_header = False
  if context.sconf.CheckHeader('GL/gl.h', language='C++'):
    context.env.AppendUnique(CXXFLAGS='-DHAS_GL_GL_H')
    found_header = True
  if context.sconf.CheckHeader('OpenGL/gl.h', language='C++'):
    context.env.AppendUnique(CXXFLAGS='-DHAS_OPENGL_GL_H')
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

conf = env.Configure(custom_tests = {'CheckBoost' : CheckBoost,})

if not conf.CheckBoost():
  print "Couldn't find Boost!"
  Exit(1)

conf.Finish()

graphics_env = env.Clone()
conf = graphics_env.Configure(custom_tests = {
                                     'CheckGL' : CheckGL,
                                     'CheckGLU' : CheckGLU,
                                     'CheckGLUT' : CheckGLUT})
if not conf.CheckGL():
  print "Couldn't find GL!"
  Exit(1)

if not conf.CheckGLU():
  print "Couldn't find GLU!"
  Exit(1)

if not conf.CheckGLUT():
  print "Couldn't find GLUT!"
  Exit(1)

conf.Finish()


def build_dir(start_env, start_graphics_env, dir):
  export_include = include + '/' + dir
  env = start_env.Clone(INCDIR = export_include)
  graphics_env = start_graphics_env.Clone(INCDIR = export_include)
  sconscript_file = dir + '/SConscript'
  build_dir = 'build/' + dir
  SConscript(sconscript_file, exports=['env', 'graphics_env'], variant_dir=build_dir, duplicate=0)
  
dir_list = ['simpleprotorpc',
            'simpleprotographics',
            'test']

for dir in dir_list: 
  build_dir(env, graphics_env, dir)
