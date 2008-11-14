AddOption('--prefix',
          dest='prefix',
          type='string',
          nargs=1,
          action='store',
          metavar='DIR',
          default='#export',
          help='Installation prefix')

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

include = GetOption('prefix') + '/include'
lib = GetOption('prefix') + '/lib'
bin = GetOption('prefix') + '/bin'

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
  env.Alias('server', bin + '/graphics_server')
  env.Alias('libs', [lib, include])
  env.Alias('tests', 'build/test')
  env.Default(GetOption('prefix'))

  if ARGUMENTS.get('PROD', 0):
    env.AppendUnique(CXXFLAGS=['-O2',])
  else:
    env.AppendUnique(CXXFLAGS=['-g','-Wall'])

  SConscript(sconscript_file, exports='env', variant_dir=build_dir, duplicate=0)
