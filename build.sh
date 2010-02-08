#!/bin/bash

THIRDPARTY=thirdparty
ARCHIVES=${THIRDPARTY}/archives

BOOST_DIR=boost_1_41_0
BOOST_DIST=${ARCHIVES}/${BOOST_DIR}.tar.gz
BOOST_HTTP='http://downloads.sourceforge.net/project/boost/boost/1.41.0/boost_1_41_0.tar.gz?use_mirror=kent'

XERCES_DIR=xerces-c-3.1.0
XERCES_DIST=${ARCHIVES}/${XERCES_DIR}.tar.gz
XERCES_HTTP="http://mirror.ox.ac.uk/sites/rsync.apache.org/xerces/c/3/sources/xerces-c-3.1.0.tar.gz"
XERCES_INSTALL_DIR=${THIRDPARTY}/xerces-install

ZLIB_DIR=zlib-1.2.3
ZLIB_DIST=${ARCHIVES}/${ZLIB_DIR}.tar.gz
ZLIB_HTTP="http://downloads.sourceforge.net/project/libpng/zlib/1.2.3/zlib-1.2.3.tar.gz?use_mirror=kent"

function help ()
{
cat <<EOF
Protean depends on Xerces and Boost. Boost in turn depends on
zlib. It is not difficult to install the prerequisites correctly,
but it is certainly tedious, especially if you just want to try
Protean. This script will download everything it needs to compile
Protean, compile it, and run the unit tests.

Example command lines:

demo.sh msvc 9.0         (9.0 and 9.0express *are* different)
demo.sh msvc 9.0express  
demo.sh gcc              (use default version of g++)
demo.sh gcc 4            (will probably work under Cygwin)
demo.sh gcc 4.3          (will work under Linux if gcc 4.3 installed)
demo.sh package          (make a tarball that can be compiled offline)

Environment variables:

PROCESSORS - number of files to compile concurrently
             (passed as -j to bjam and make)

NO_XERCES  - Xerces is used by Protean or compiled if set
             (useful for testing or if the compilation fails)

http_proxy - e.g. squid.mynetwork.com:3128
             (saves bandwidth if frequently rebuilding from scratch)

This script can run under Windows if started in a Cygwin window
(http://www.cygwin.com/). It requires subversion, make, wget and tar.
You can use a MSVC project in./project after running
the script if you prefer the MSVC GUI to the command line.

Compiler support:

MSVC : 8.0 and 9.0express are known to work under XP
       8.0express and 9.0 are likely to work
       10.0 preliminary support

g++  : 3.3 or earlier are not supported as they don't cope with boost::mpl 
       3.4.4 (maybe also earlier versions and 3.4.5) needs a workaround
          that turned out to be ugly and broke compilation under MSVC. It is
          svn diff -r75:76 https://protean.svn.sourceforge.net/svnroot/protean/protean
       3.4.6 seems to work on some platforms, but there are known link
          issues on others, e.g. see
          http://gcc.gnu.org/bugzilla/show_bug.cgi?id=23589
          http://gcc.gnu.org/bugzilla/show_bug.cgi?id=16276
       4.0 and later are believed to work, 4.2.4 works 

EOF
exit 0
}

function check-prerequisites ()
{
   for exe in "$@"; do
      which "$exe" 1>&- 2>&-
      if [ "$?" != 0 ]; then
         echo $exe is required but does not appear on the path
         exit 1
      fi
   done
}

function default-gcc-version ()
{
#    if [ "`uname -o`" = Cygwin ]; then
#        PATTERN='\1'
#    else
#        PATTERN='\1.\2'
#    fi
#    # if this fails then just specify the gcc version by hand
#    check-prerequisites g++ grep awk sed
#    echo `g++ -v 2>&1 | grep "gcc version" | awk '{ print $3 }'` | sed 's/\([[:digit:]]*\).\([[:digit:]]*\).*/'"$PATTERN"/
     g++ -v 2>&1 | grep "gcc version" | awk '{ print $3 }'
}

# sets several global variables ...
function parse-command-line ()
{
case "$1" in
    help|-help|-h)
        help
    ;;

    msvc)
        COMPILER=msvc
        COMPILER_VERSION=$2
        case "$COMPILER_VERSION" in
        
            10.0|10.0express)
                MSVC_ENV='VS100COMNTOOLS'
                XERCES_MSVC_VERSION=9
                if [ -z "$NO_XERCES" ]; then
                    echo Xerces does not have a MSVC10 project. I will try to use the MSVC9
                    echo project, but it will probably fail to compile. Please set NO_XERCES if so.
                fi
            ;;
       
            9.0|9.0express)
                MSVC_ENV='VS90COMNTOOLS'
                XERCES_MSVC_VERSION=9
            ;;
      
            8.0|8.0express)
                MSVC_ENV='VS80COMNTOOLS'
                XERCES_MSVC_VERSION=8
            ;;
      
            *)
                echo Unrecognised version of MSVC, try e.g. 8.0, 9.0, 9.0express
                exit 1;;
        esac
        BJAM_TOOL="--toolset=${COMPILER}-${COMPILER_VERSION}"
    ;;

    gcc)
        COMPILER=gcc
        COMPILER_VERSION=$2

        if [ -z "$COMPILER_VERSION" ]; then
            COMPILER_VERSION=`default-gcc-version`
            echo Default version of g++ is $COMPILER_VERSION
            BJAM_TOOL="--toolset=gcc"
        else
            export CC=gcc-$COMPILER_VERSION
            export CXX=g++-$COMPILER_VERSION
            export CPP=cpp-$COMPILER_VERSION
            export CXXCPP=$CPP
            check-prerequisites $CC $CXX $CPP $CXXCPP
            #BJAM_TOOL="--toolset=${COMPILER}-${COMPILER_VERSION}"
        fi       
    ;;

    *)
        echo First argument not understood.
        echo
        help 
    ;;
esac

}

function check_exists()
{
    if [ ! -f "$1" ]; then
        echo Could not download $1
        exit 1
    fi
}

function get-archives ()
{
mkdir -p $ARCHIVES

#echo Getting boost ...
[ -f $BOOST_DIST ] || (cd $ARCHIVES; wget "$BOOST_HTTP")
check_exists "$BOOST_DIST"
[ -d $BOOST_DIR ] || (cd "$THIRDPARTY" && tar -xzf "${DIR}/${BOOST_DIST}")

#echo Getting xerces ...
[ -f $XERCES_DIST ] || (cd $ARCHIVES; wget "$XERCES_HTTP")
check_exists "$XERCES_DIST"
[ -d $XERCES_DIR ] || (cd "$THIRDPARTY" && tar -xzf "${DIR}/$XERCES_DIST")

#echo Getting zlib ...
[ -f $ZLIB_DIST ] || (cd "$ARCHIVES"; wget "$ZLIB_HTTP")
check_exists "$ZLIB_DIST"
[ -d $ZLIB_DIR ] || (cd "$THIRDPARTY" && tar -xzf "$DIR/$ZLIB_DIST")

}

function run_with_bat()
{
    batfile=$MSVC_ENV
    dir=$1; shift
    drive=`cygpath -m \`pwd\` | head -c 1` 
    tmpfile="$TMP/tmp$$.bat"
    echo "@echo off" > $tmpfile
    echo "call \"%$batfile%vsvars32.bat\" >NUL:" >> $tmpfile
    echo ${drive}: >> $tmpfile
    echo cd `cygpath -w $dir` >> $tmpfile
    #echo dir >> $tmpfile
    echo call $* >> $tmpfile
    cat $tmpfile
    "`cygpath "$COMSPEC"`" /c "`cygpath -w "$tmpfile"`" "$@"
    status=$?
    rm -f $tmpfile
    return $status
}

function xerces_install_dir()
{
    local SUBDIR="${COMPILER}-${COMPILER_VERSION}"
    local INSTALL="$DIR/$XERCES_INSTALL_DIR/$SUBDIR"
    echo "$INSTALL"
}

function set_xerces_dirs()
{
    case "$COMPILER" in
        msvc)
              XERCES_INCLUDE_DIR="$DIR/$XERCES_DIR/src"
              XERCES_LIB_DIR="`xerces_install_dir`"
        ;;

        gcc)
              XERCES_INCLUDE_DIR="`xerces_install_dir`/include"
              XERCES_LIB_DIR="`xerces_install_dir`/lib"
        ;;

        *)
              echo Compiler not recognised while figuring out the installation directory: $COMPILER
              exit 1
        ;;
    esac
}

function compile_xerces_using_msvc()
{
    local SUBDIR="VC${XERCES_MSVC_VERSION}"
    local SOLUTION="${DIR}/${THIRDPARTY}/${XERCES_DIR}/projects/Win32/${SUBDIR}/xerces-all/xerces-all.sln"
    if [ ! -f "$SOLUTION" ]; then
        echo Cannot find a Xerces project file, maybe the compiler version is wrong: $SOLUTION
        exit 1
    fi

    case "$COMPILER_VERSION" in
        *express*) DEVENV=vcexpress ;;
        *) DEVENV=devenv ;;
    esac

    for CONF in Debug Release; do
      run_with_bat "${DIR}/${THIRDPARTY}/${XERCES_DIR}" $DEVENV `cygpath -w "$SOLUTION"` /build $CONF
    done

    local INSTALL="`xerces_install_dir`"
    local BUILD="$DIR/$THIRDPARTY/$XERCES_DIR/Build/Win32/$SUBDIR"
    mkdir -p "$INSTALL"
    for EXT in dll lib; do
       /usr/bin/find "$BUILD" -name \*.$EXT -exec cp {} "$INSTALL" \;
    done
}

function compile_xerces_using_gcc()
{
    pushd "$DIR/$THIRDPARTY/$XERCES_DIR"
    ./configure --prefix="`xerces_install_dir`"
    make $J
    make install
    popd
}

function build_xerces()
{
    case "$COMPILER" in
       msvc)
              compile_xerces_using_msvc;;
       gcc)
              compile_xerces_using_gcc;;
       *)
              echo Unrecognised compiler $COMPILER when building Xerces
              exit 1
       ;;
    esac
}

function build_bjam()
{
   #convenient in case the compiler has changed
   rm -f "$THIRDPARTY/$BOOST_DIR/bjam" "$THIRDPARTY/$BOOST_DIR/bjam.exe"  

   if [ "$COMPILER" = msvc ]; then
       echo Running $CMD to build bjam
       run_with_bat "${DIR}/$THIRDPARTY/${BOOST_DIR}" ".\\bootstrap.bat"

       echo Compiling all of boost as a workaround for build problems ...
       pushd "$BOOST_DIR"
       run_with_bat "${DIR}/$THIRDPARTY/${BOOST_DIR}" bjam --build-type=complete --without-mpi "$BJAM_TOOL"
       popd
   else
       pushd "$THIRDPARTY/$BOOST_DIR"
       ./bootstrap.sh
       popd
   fi
}

function write_settings()
{
set_xerces_dirs

local NEW_BUILD_PATH="$DIR/$THIRDPARTY/$BOOST_DIR"
local NEW_XERCES_INCLUDE="$XERCES_INCLUDE_DIR"
local NEW_XERCES_LIB="$XERCES_LIB_DIR"
local NEW_ZLIB_SOURCE="$DIR/$THIRDPARTY/$ZLIB_DIR"

if [ "$COMPILER" = msvc ]; then
   NEW_BUILD_PATH=`cygpath -m "$NEW_BUILD_PATH"`
   NEW_XERCES_INCLUDE=`cygpath -m "$NEW_XERCES_INCLUDE"`
   NEW_XERCES_LIB=`cygpath -m "$NEW_XERCES_LIB"`
   NEW_ZLIB_SOURCE=`cygpath -m "$NEW_ZLIB_SOURCE"`
fi

cat > settings.sh <<EOF
# generated by $0 at `date`
export PATH="$DIR/$THIRDPARTY/$BOOST_DIR":"\$PATH"
export BOOST_BUILD_PATH="$NEW_BUILD_PATH"
export XERCES_INCLUDE="$NEW_XERCES_INCLUDE"
export XERCES_LIBPATH="$NEW_XERCES_LIB"
export ZLIB_SOURCE="$NEW_ZLIB_SOURCE"
export NO_BZIP2=1
EOF
}

function report_success()
{
cat <<EOF


Done, everything has built fine and the unit tests pass. To explore
protean you can now try:

    source settings.sh
    ... tweak some of the (as yet non-existent) examples ...
    bjam $J "$BJAM_TOOL"

EOF
}

function report_failure()
{
cat <<EOF



Something seems to have gone wrong. You can report bugs or ask for
help at http://sourceforge.net/projects/protean/support. There are
manual build instructions in the documentation and also in
Jamroot.jam. If the attempt to build Xerces failed, you may want to
try with NO_XERCES set.

You can re-run the final stage of the build as follows:
source settings.sh; bjam $J "$BJAM_TOOL"

EOF
}

function backup ()
{
    if [ -f "$1" ]; then
       local i=0
       while [ -f "${1}.backup.$i" ]; do
           i=$(( $i + 1 ))
       done
       echo mv "$1" "${1}.backup.$i"
       mv "$1" "${1}.backup.$i"
    fi 
}

function build_and_test_protean_and_exit()
{
  echo Running bjam to compile Protean and run the unit tests ...

  local USER_CONFIG="user-config.jam"

  backup "$USER_CONFIG"

  local TOOLSPEC

  #bjam seems to use the wrong version of g++ unless it is specified
  #in the jamfile (but the default version works fine)
  if [ "$COMPILER" = gcc ] && [ ! -z "$CXX" ]; then
     cat > "$USER_CONFIG" <<EOF
        #see user-config.jam.examples
        using gcc : $COMPILER_VERSION : $CXX ;
EOF
  elif [ "$COMPILER" = gcc ]; then
     TOOLSPEC=
  else
     TOOLSPEC="$BJAM_TOOL"
  fi

  cd "$DIR"
  if bjam $J "$BJAM_TOOL"; then
    report_success
    exit 0
  else
    report_failure
    exit 1
  fi
}

# there might be more, but won't be known until a bit later
check-prerequisites svn tar make wget head awk

[ -n "$PROCESSORS" ] && J="-j $PROCESSORS"

DIR="`dirname "$0"`"
DIR="`cd "$DIR"; pwd`"

parse-command-line "$@"

get-archives

#do this before sourcing setttings. It puts the boost directory on the
#path, and Cygwin will then inexplicably execute the install text file
#in there despite it not having an execute permission 
[ -z "$NO_XERCES" ] && build_xerces

write_settings
source settings.sh

build_bjam

build_and_test_protean_and_exit
