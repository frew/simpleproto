AddOption('--prefix',
          dest='prefix',
          type='string',
          nargs=1,
          action='store',
          metavar='DIR',
          default='#export',
          help='Installation prefix')

import SConsAddons.Options.Boost as Boost

boost_options = Boost.Boost(name='boost', requiredVersion='1.35.0',
                            preferDynamic=False)

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
                    CPPPATH = [include],
                    LIBPATH = [lib],
                    LIBS=['boost_thread','protobuf'])
  env.Alias('server', bin + '/graphics_server')
  env.Alias('libs', [lib, include])
  env.Alias('tests', 'build/test')
  env.Default(GetOption('prefix'))
  if not boost_options.available:
    boost_options.find(env)
    boost_options.validate(env)
    if not boost_options.available:
      print "Couldn't locate Boost, exiting!"
      Exit(1)
  boost_options.apply(env)
  if ARGUMENTS.get('PROD', 0):
    env.AppendUnique(CXXFLAGS=['-O2',])
  else:
    env.AppendUnique(CXXFLAGS=['-g','-Wall'])

  SConscript(sconscript_file, exports='env', variant_dir=build_dir, duplicate=0)
