#!/bin/bash

# Protean depends on Xerces and Boost. Boost in turn depends on
# zlib. It is not difficult to install the prerequisites correctly,
# but it is certainly tedious, especially if you just want to try
# Protean. This script will download everything it needs to compile
# Protean, compile it, and run the unit tests.

# demo.sh              - compile Protean and run unit tests
# demo.sh package      - make a tarball that can be compiled offline

# environment variable PROCESSORS can be used to specify the number
# of processors to be used when running make and bjam, e.g:
# PROCESSORS=4 demo.sh

# This script can run under Windows if started in a Cygwin window
# (http://www.cygwin.com/). It uses g++ on all architectures, and also
# requires subversion, make, wget and tar. If you want to use MSVC,
# you can still use the script to download everything. Then you should
# follow either the documentation or the quick-start instructions in
# checkout/protean/Jamroot.jam. Once you have prepared the
# environment, you will be able to invoke bjam from either a Windows
# command prompt of from a Cygwin shell and build Protean using MSVC.
# You can also use an MSVC project in checkout/protedan/project if you
# would prefer to use the MSVC GUI


ARCHIVES=archives

BOOST_DIR=boost_1_41_0
BOOST_DIST=${ARCHIVES}/${BOOST_DIR}.tar.gz
BOOST_HTTP='http://downloads.sourceforge.net/project/boost/boost/1.41.0/boost_1_41_0.tar.gz?use_mirror=kent'

XERCES_DIR=xerces-c-3.0.1
XERCES_DIST=${ARCHIVES}/${XERCES_DIR}.tar.gz
XERCES_HTTP="http://mirror.ox.ac.uk/sites/rsync.apache.org/xerces/c/3/sources/xerces-c-3.0.1.tar.gz"

ZLIB_DIR=zlib-1.2.3
ZLIB_DIST=${ARCHIVES}/${ZLIB_DIR}.tar.gz
ZLIB_HTTP="http://downloads.sourceforge.net/project/libpng/zlib/1.2.3/zlib-1.2.3.tar.gz?use_mirror=kent"

PROTEAN_HTTP=https://protean.svn.sourceforge.net/svnroot/protean

PREREQUISITES="g++ svn tar make wget"
if [ ! "which $PREREQUISITES" ]; then
   echo At least one prerequisite not found
   which $PREREQUISITES
   exit 1
fi

[ -n "$PROCESSORS" ] && J="-j $PROCESSORS"

mkdir -p $ARCHIVES

echo Getting boost ...
[ -f $BOOST_DIST ] || (cd $ARCHIVES; wget "$BOOST_HTTP")
[ -d $BOOST_DIR ] || tar -xzf ${BOOST_DIST}

echo Getting xerces ...
[ -f $XERCES_DIST ] || (cd $ARCHIVES; wget "$XERCES_HTTP")
[ -d $XERCES_DIR ] || tar -xzf $XERCES_DIST

echo Getting zlib ...
[ -f $ZLIB_DIST ] || (cd $ARCHIVES; wget "$ZLIB_HTTP")
[ -d $ZLIB_DIR ] || tar -xzf $ZLIB_DIST

echo Getting protean ...
PROTEAN_CHECKOUT=checkout/protean
if [ ! -d "$PROTEAN_CHECKOUT" ]; then
     
    #Any existing Protean checkout that stores this script probably caches
    #a password, and might contain local changes that would confuse
    #anyone relying on the revision in the filename being correct. It is
    #easier to get a fresh copy than to decide if the exiting one would do.

    PARENT="`dirname "$PROTEAN_CHECKOUT"`"
    mkdir -p "$PARENT"
    echo Checking out Protean ...
    svn co "$PROTEAN_HTTP" "$PARENT"
else
    echo Using the existing Protean checkout in $PROTEAN_CHECKOUT
fi

DIR="`dirname "$0"`"
DIR="`cd "$DIR"; pwd`"

if [ "$1" == package ]; then
   
   THIS=`basename "$0"`
   [ -f $THIS ] || cp "$0" .   
   VERSION=`svnversion $PROTEAN_CHECKOUT`
   TARGET=protean_demo_distribution_rev${VERSION}.tar.gz
   tar -cvzf $TARGET $ARCHIVES $PROTEAN_CHECKOUT $THIS
   echo Wrote $TARGET

   exit 0
fi

if [ ! -d $BOOST_DIR ]; then
   cd "$BOOST_DIR"
   ./bootstrap.sh
   cd "$DIR"
fi

XERCES_INSTALL=$DIR/xerces-install
cd $XERCES_DIR
if [ ! -d "$XERCES_INSTALL" ]; then
   ./configure --prefix=${XERCES_INSTALL}
   make $J
   make install
fi
cd $DIR

USER_CONFIG=${PROTEAN_CHECKOUT}/user-config.jam
if [ ! -f "$USER_CONFIG" ]; then 
cat > $USER_CONFIG <<EOF
# see user-config.jam.examples
using gcc ;
EOF
fi

cat > settings.sh <<EOF
export PATH="$DIR/$BOOST_DIR":"\$PATH"
export BOOST_BUILD_PATH="$DIR/$BOOST_DIR"
export XERCES_INCLUDE="$XERCES_INSTALL/include"
export XERCES_LIBPATH="$XERCES_INSTALL/lib"
export ZLIB_SOURCE="$DIR/$ZLIB_DIR"
export NO_BZIP2=1
EOF

source settings.sh

cd "$PROTEAN_CHECKOUT"
if bjam $J; then

cat <<EOF



Done, everything has built fine and the unit tests pass. To explore
protean you can now try:

    source settings.sh
    cd $PROTEAN_CHECKOUT
    ... tweak some of the (as yet non-existent) examples ...
    bjam

EOF

else

cat <<EOF



Something seems to have gone wrong. You can report bugs or ask for
help at http://sourceforge.net/projects/protean/support. There are
manual build instructions in the documentation and also in
checkout/protean/Jamroot.jam. If the attempt to build Xerces failed,
you may want to try with NO_XERCES set.

You can re-run the final stage of the build as follows:
source settings.sh; cd $PROTEAN_CHECKOUT; bjam

EOF

fi


cd "$DIR"

