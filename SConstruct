import os
import shutil
import subprocess
import sys

linux_boost_version = '1.54.0'

libs = ['libboost_date_time',
             'libboost_filesystem',
             'libboost_iostreams',
             'libboost_regex',
             'libboost_system']

linkflags = []
if sys.platform == 'linux2':
    libs += ['libxerces-c-3.1']
    linkflags = ['-Wl,--gc-sections', '-Wl,-rpath', '.']
if sys.platform == 'darwin':
    libs = [ File('/usr/local/lib/'+lib+'.a') for lib in libs ] + ['libxerces-c'] + ['z', 'curl']
    linkflags = ['-Wl,-dead_strip', '-v', '-install_name', '@loader_path/libprotean.dylib', '-dynamiclib']

env = Environment(CPPPATH=['#'],
                  CPPFLAGS=['-Wno-multichar', '-std=c++11',
                    '-O3', '-fdata-sections', '-ffunction-sections'],
                  LINKFLAGS=linkflags,
                  LIBPATH=['#'])


env['CXX'] = os.getenv('CXX') or env['CXX']
env['PREFIX'] = os.getenv('PREFIX') or '/usr'

protean = env.SharedLibrary(target = 'protean',
                            source = Glob('src/*.cpp'),
                            LIBS=libs,
                            FRAMEWORKS=['CoreServices'])

protean_test = \
    env.Program(target     = 'protean_test',
                source     = Glob('test/core/*.cpp'),
                CPPDEFINES = ['BOOST_TEST_DYN_LINK'],
                LIBS       = libs+[protean, 'boost_unit_test_framework'],
                RPATH      = '.')

def copy_libs(target, source, env):
    if sys.platform != 'linux2':
        return

    import shutil
    for lib in libs:
        srclib = '/usr/local/lib/'+lib+'.so' + ('.'+linux_boost_version if 'boost' in lib else '')
        shutil.copy2(srclib, '.')


copy_libs_cmd = Command(target="copy-libs",
                     source=[],
                     action=copy_libs)


Depends( copy_libs_cmd, protean_test )
