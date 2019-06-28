## Makefile to build C++ example programs included with the IOHprofiler distribution

LDFLAGS += -lm -lboost_system -lboost_filesystem
CCFLAGS = -g -std=c++11

########################################################################
## Toplevel targets
all: test

clean:
	rm -f test

########################################################################
## Programs 
test : common* IOHprofiler* test.cpp
	g++ ${CCFLAGS} -o test common* IOHprofiler* test.cpp ${LDFLAGS}
