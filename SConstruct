from os import getcwd
from os.path import join

import Boost
import GL

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

AddOption('--protobuf-prefix',
          dest='protobuf_prefix',
          type='string',
          nargs=1,
          action='store',
          metavar='DIR',
          help='protobuf installation prefix')

include = GetOption('prefix') + '/include'
lib = GetOption('prefix') + '/lib'
bin = GetOption('prefix') + '/bin'

env = Environment(BINDIR = bin,
                  LIBDIR = lib,
                  CPPPATH = [include],
                  LIBPATH = [lib],
                  LIBS=[])
  
install_dir = GetOption('prefix')
if install_dir[0] == '#':
  # TODO(frew): Probably wrong to rely on this
  install_dir = join(getcwd(), install_dir[1:])
env.AppendUnique(RPATH=join(install_dir, 'lib'))
env.Alias('server', bin + '/graphics_server')
env.Alias('libs', [lib, include])
env.Alias('tests', 'build/test')
env.Default(GetOption('prefix'))

if ARGUMENTS.get('PROD', 0):
  env.AppendUnique(CXXFLAGS=['-O2',])
else:
  env.AppendUnique(CXXFLAGS=['-g','-Wall'])

conf = env.Configure(custom_tests = {'CheckBoost' : Boost.CheckBoost,},
                     conf_dir="/tmp/sconftmp")

if GetOption('protobuf_prefix') is not None:
  env.AppendUnique(CPPPATH=join(GetOption('protobuf_prefix'), 'include'))
  env.AppendUnique(LIBPATH=join(GetOption('protobuf_prefix'), 'lib'))
  

if not conf.CheckLibWithHeader('protobuf', 'google/protobuf/text_format.h', 
                               'C++'):
  print "Couldn't find protobuf"
  Exit(1)

env.AppendUnique(LIB='protobuf')

if not conf.CheckBoost():
  print "Couldn't find Boost!"
  Exit(1)

conf.Finish()

graphics_env = env.Clone()
conf = graphics_env.Configure(custom_tests = {
                                     'CheckGL' : GL.CheckGL,
                                     'CheckGLU' : GL.CheckGLU,
                                     'CheckGLUT' : GL.CheckGLUT},
                              conf_dir = '/tmp/sconftmp')
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
