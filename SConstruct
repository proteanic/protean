import os
import shutil
import subprocess
import sys

static_libs = [File('/usr/local/lib/libboost_date_time.a'),
        File('/usr/local/lib/libboost_filesystem.a'),
        File('/usr/local/lib/libboost_regex.a'),
        File('/usr/local/lib/libboost_system.a'),
        File('/usr/local/lib/libxerces-c.a'),

        File('/usr/local/lib/libboost_iostreams.a')]

include_paths, lib_paths, linkflags = [], [], []
if sys.platform == 'linux2':
    linkflags = ['-Wl,--gc-sections']
if sys.platform == 'darwin':
    # Correct if boost and xerces were installed with brew
    include_paths = ['/usr/local/include']
    lib_paths = ['/usr/local/lib']
    linkflags = ['-Wl,-dead_strip', '-v', '-install_name', '@loader_path/libprotean.dylib', '-dynamiclib']

env = Environment(CPPPATH=['#'] + include_paths,
                  CPPFLAGS=['-Wno-multichar', '-std=c++11',
                    '-O3', '-fdata-sections', '-ffunction-sections'],
                  LINKFLAGS=linkflags,
                  LIBPATH=['#'] + lib_paths)

env['CXX'] = os.getenv('CXX') or env['CXX']
env['PREFIX'] = os.getenv('PREFIX') or '/usr'

protean = env.SharedLibrary(target = 'protean',
                            source = Glob('src/*.cpp'),
                            LIBS=static_libs + ['z', 'curl'],
                            FRAMEWORKS=['CoreServices'])

protean_test = \
    env.Program(target     = 'protean_test',
                source     = Glob('test/core/*.cpp') + static_libs,
                CPPDEFINES = ['BOOST_TEST_DYN_LINK'],
                LIBS       = [protean, 'boost_unit_test_framework'],
                RPATH      = '.')

