from os import listdir
from os.path import exists, isdir, join

import re

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

  if context.env.GetOption('boost_prefix') is not None:
    prefixes = [context.env.GetOption('boost_prefix')]

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
  if context.env.GetOption('boost_libdir') is not None:
    libdir = context.env.GetOption('boost_libdir')
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
  context.env.AppendUnique(LIBS = 'boost_thread' + context.env['BOOST_SUFFIX'])

  if not context.sconf.CheckLib('boost_thread' + best_libsuffix):
     return False
  return True
