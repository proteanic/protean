import errno, os, shutil

user_libs = ['libboost_date_time.so.1.54.0',
             'libboost_filesystem.so.1.54.0',
             'libboost_iostreams.so.1.54.0',
             'libboost_regex.so.1.54.0',
             'libboost_system.so.1.54.0',
             'libxerces-c-3.1.so']

env = Environment(CPPPATH=['#'],
                  CPPFLAGS=['-Wno-multichar', '-std=c++11', '-s', '-O3', '-fdata-sections', '-ffunction-sections'],
                  LINKFLAGS=['-Wl,--gc-sections'],
                  LIBPATH=['#'])

env['CXX'] = os.getenv('CXX') or env['CXX']
env['PREFIX'] = os.getenv('PREFIX') or '/usr'

protean_libs = [lib.split('.so')[0] for lib in user_libs]

protean = env.SharedLibrary(target = 'protean',
                            source = Glob('src/*.cpp'),
                            LIBS   = protean_libs)

env.Program(target     = 'protean_test',
            source     = Glob('test/core/*.cpp'),
            CPPDEFINES = ['BOOST_TEST_DYN_LINK'],
            LIBS       = [protean, 'boost_unit_test_framework'] + protean_libs,
            RPATH      = '.')

env.Alias('install', env.Install(os.path.join(env['PREFIX'], 'lib'), protean))

# Copy dependent shared objects to be near the built binary for easy packaging
# Downstream projects could be built in a different docker container that does
# not have all the dependencies
def _copy_user_libs():
    dst_dir = 'third-party'
    try:
        os.makedirs(dst_dir)
    except OSError as ex:
        if ex.errno != errno.EEXIST:
            raise
    for lib in user_libs:
        src = '/usr/local/lib/{}'.format(lib)
        dst = '{}/{}'.format(dst_dir,lib)
        if not os.path.isfile(dst) or \
           os.stat(dst).st_size != os.stat(src).st_size or \
           os.stat(dst).st_mtime != os.stat(src).st_mtime:
            shutil.copy2(src, dst)
_copy_user_libs()
