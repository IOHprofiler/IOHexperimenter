## Makefile to build C example programs included with the IOHprofiler distribution
##
## NOTE: We have tried to make this Makefile as generic and portable
## as possible. However, there are many (incompatible) versions of
## make floating around. We regularly test using GNU make and BSD make
## from FreeBSD. If you have trouble compiling the examples, please
## try to use GNU make. 
##
## On Windows it is best to use either the included NMakefile by running
##
##   nmake -f NMakefile
##
## or installing Cygwin and running GNU make from within Cygwin.

LDFLAGS += -lm
CCFLAGS = -g -std=c++11

########################################################################
## Toplevel targets
all: test

clean:
	rm -f test

########################################################################
## Programs 
test: 
	g++ ${CCFLAGS} -o test test.cpp common* IOHprofiler* ${LDFLAGS} -lboost_system -lboost_filesystem