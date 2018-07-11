import os
import platform
import shutil
import subprocess
import sys

libs = ['libboost_date_time',
        'libboost_filesystem',
        'libboost_iostreams',
        'libboost_regex',
        'libboost_system']

link_flags = []
if platform.system() == 'Linux':
    libs += ['libxerces-c-3.1']
    link_flags = ['-Wl,--gc-sections', '-Wl,-rpath', '.']
elif platform.system() == 'Darwin':
    boost_static_link_path = os.getenv('BOOST_STATIC_LINK_PATH', '')
    if boost_static_link_path:
        libs = [File('{}/{}.a'.format(boost_static_link_path, lib)) for lib in libs]
    libs += ['libxerces-c', 'z', 'curl']
    link_flags = ['-Wl,-dead_strip', '-v', '-install_name', '@loader_path/libprotean.dylib', '-dynamiclib']

cpp_path = os.getenv('CPPPATH', '').split(os.pathsep) + ["#"]
lib_path = os.getenv('LIBPATH', '').split(os.pathsep) + ["#"]

env = Environment(ENV       = {'PATH': os.getenv('PATH')},
                  CPPPATH   = cpp_path,
                  CPPFLAGS  = ['-Wno-deprecated-declarations', '-Wno-multichar', '-std=c++11',
                               '-O3', '-fdata-sections', '-ffunction-sections'],
                  LINKFLAGS = link_flags,
                  LIBPATH   = lib_path)

env['CXX'] = os.getenv('CXX', env['CXX'])
env['PREFIX'] = os.getenv('PREFIX', '/usr')

protean = env.SharedLibrary(target     = 'protean',
                            source     = Glob('src/*.cpp'),
                            LIBS       = libs,
                            FRAMEWORKS = ['CoreServices'])

if int(os.getenv('BUILD_TESTS', 1)):
    protean_test = \
        env.Program(target     = 'protean_test',
                    source     = Glob('test/core/*.cpp'),
                    CPPDEFINES = ['BOOST_TEST_DYN_LINK'],
                    LIBS       = libs + [protean, 'boost_unit_test_framework'],
                    LINKFLAGS  = ['-Wl,-rpath', '.', '-Wl,-rpath', '/usr/local/lib'])

    def copy_libs(target, source, env):
        if platform.system() == 'Linux':
            boost_version = os.getenv('BOOST_VERSION', '1.54.0')
            for lib in libs:
                src_lib = '/usr/local/lib/{}.so{}'.format(lib, '.' + boost_version if 'boost' in lib else '')
                shutil.copy2(src_lib, '.')

    copy_libs_cmd = Command(target="copy-libs", source=[], action=copy_libs)
    Depends(copy_libs_cmd, protean_test)
