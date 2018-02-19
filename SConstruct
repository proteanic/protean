import os
import shutil
import subprocess
import sys

linux_boost_version = os.getenv('BOOST_VERSION', '1.54.0')

libs = ['libboost_date_time',
        'libboost_filesystem',
        'libboost_iostreams',
        'libboost_regex',
        'libboost_system']

link_flags = []
if sys.platform == 'linux2':
    libs += ['libxerces-c-3.1']
    link_flags = ['-Wl,--gc-sections', '-Wl,-rpath', '.']
if sys.platform == 'darwin':
    libs = [ File('/usr/local/lib/'+lib+'.a') for lib in libs ] + ['libxerces-c'] + ['z', 'curl']
    link_flags = ['-Wl,-dead_strip', '-v', '-install_name', '@loader_path/libprotean.dylib', '-dynamiclib']

cpp_path = os.getenv('CPPPATH', '').split(os.pathsep) + ["#"]
lib_path = os.getenv('LIBPATH', '').split(os.pathsep) + ["#"]

env = Environment(ENV       = {'PATH': os.getenv('PATH')},
                  CPPPATH   = cpp_path,
                  CPPFLAGS  = ['-Wno-multichar', '-std=c++11',
                               '-O3', '-fdata-sections', '-ffunction-sections'],
                  LINKFLAGS = link_flags,
                  LIBPATH   = lib_path)

env['CXX'] = os.getenv('CXX') or env['CXX']
env['PREFIX'] = os.getenv('PREFIX') or '/usr'

protean = env.SharedLibrary(target     = 'protean',
                            source     = Glob('src/*.cpp'),
                            LIBS       = libs,
                            FRAMEWORKS = ['CoreServices'])

if int(os.getenv('BUILD_TESTS', 1)):
    protean_test = \
        env.Program(target     = 'protean_test',
                    source     = Glob('test/core/*.cpp'),
                    CPPDEFINES = ['BOOST_TEST_DYN_LINK'],
                    LIBS       = libs+[protean, 'boost_unit_test_framework'],
                    LINKFLAGS  = ['-Wl,-rpath', '.', '-Wl,-rpath', '/usr/local/lib'])

    def copy_libs(target, source, env):
        if sys.platform != 'linux2':
            return
        for lib in libs:
            srclib = '/usr/local/lib/'+lib+'.so' + ('.'+linux_boost_version if 'boost' in lib else '')
            shutil.copy2(srclib, '.')


    copy_libs_cmd = Command(target = "copy-libs",
                            source = [],
                            action = copy_libs)

    Depends( copy_libs_cmd, protean_test )

