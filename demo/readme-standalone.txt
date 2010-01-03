To explore Protean under Linux or Cygwin, cd to a fresh directory and run the
following commands. They will compile Protean and run the unit tests. There are
more detailed comments in the script demo.sh.

svn export https://protean.svn.sourceforge.net/svnroot/protean/protean/demo/demo.sh; ./demo.sh

At the time of writing only a small number of developers have write access to
the repository, however this may change in the future. You should check the
script before running it if you are at all concerned about what it might do.

If you have more than one processor, you may want to set the environment
variable PROCESSORS beforehand, e.g:

svn export https://protean.svn.sourceforge.net/svnroot/protean/protean/demo/demo.sh ; PROCESSORS=4 ./demo.sh