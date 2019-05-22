## Makefile to build C++ example programs included with the IOHprofiler distribution

LDFLAGS += -lm -lboost_system -lboost_filesystem
CCFLAGS = -g -std=c++11

########################################################################
## Toplevel targets
all: IOHprofiler_experiment

clean:
	rm -f IOHprofiler_experiment

########################################################################
## Programs 
IOHprofiler_experiment : ../../common* ../../IOHprofiler* IOHprofiler_experiment.cpp
	g++ ${CCFLAGS} -o IOHprofiler_experiment ../../common* ../../IOHprofiler* IOHprofiler_experiment.cpp ${LDFLAGS}
