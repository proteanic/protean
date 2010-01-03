Run ./demo.sh to compile Protean and run the unit tests. There are detailed
comments in the script, and there are also manual build instructions in
checkout/protean/Jamroot.jam.

If you have more than one processor, you may want to set the environment
variable PROCESSORS beforehand, e.g:

PROCESSORS=4 ./demo.sh

If you want to try the most recent version of Protean, cd to checkouts/protean
and run svn update. The most recent version of the demo script can be obtained
as follows. It will run in a fresh directory if you don't want to reuse any
existing files.

svn export https://protean.svn.sourceforge.net/svnroot/protean/protean/demo/demo.sh
