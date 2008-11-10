AddOption('--boost_inc_dir',
          dest='boost_inc_dir',
          type='string',
          nargs=1,
          action='store',
          metavar='DIR',
          default='/usr/include',
          help='boost include directory')

AddOption('--boost_lib_dir',
          dest='boost_lib_dir',
          type='string',
          nargs=1,
          action='store',
          metavar='DIR',
          default='/usr/lib',
          help='boost library directory')

include = '#export/include'
lib = '#export/lib'
bin = '#export/bin'

dir_list = ['simpleprotorpc',
            'simpleprotographics',
            'test']

for dir in dir_list: 
  sconscript_file = dir + '/SConscript'
  export_include = include + '/' + dir
  build_dir = 'build/' + dir
  env = Environment(BINDIR = bin,
                    INCDIR = export_include,
                    LIBDIR = lib,
                    CPPPATH = [include, GetOption('boost_inc_dir')],
                    LIBPATH = [lib, GetOption('boost_lib_dir')],
                    LIBS=['boost_thread','protobuf'])

  if ARGUMENTS.get('PROD', 0):
    env.AppendUnique(CXXFLAGS=['-O2',])
  else:
    env.AppendUnique(CXXFLAGS=['-g','-Wall'])

# env.AppendUnique(CXXFLAGS=['-Ibuild/include'])

# Execute(Delete('build'))
# if Execute(Mkdir(['build', 'build/include', 'build/include/simpleprotorpc', 'build/include/simpleprotographics', 'build/lib'])):
#  Exit(1)

#if Execute(Copy('build/include/simpleprotorpc', 'simpleprotorpc/rpc.h')):
#  Exit(2)

#if Execute(Copy('build/include/simpleprotographics', 'simpleprotographics/graphics.h')):
#  Exit(3)

  SConscript(sconscript_file, exports='env', variant_dir=build_dir, duplicate=0)
