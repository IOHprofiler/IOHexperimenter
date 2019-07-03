## Makefile to build C++ example programs included with the IOHprofiler distribution

LDFLAGS += -lm -lboost_system -lboost_filesystem
CCFLAGS = -g -std=c++11

########################################################################
## Toplevel targets
all: IOHprofiler_run_experiment IOHprofiler_run_suite IOHprofiler_run_problem

clean:
	rm -f IOHprofiler_run_experiment
	rm -f IOHprofiler_run_suite
	rm -f IOHprofiler_run_experiment

########################################################################
## Programs 
IOHprofiler_run_experiment : ../../common* ../../IOHprofiler* IOHprofiler_run_experiment.cpp
	g++ ${CCFLAGS} -o IOHprofiler_run_experiment ../../common* ../../IOHprofiler* IOHprofiler_run_experiment.cpp ${LDFLAGS}
IOHprofiler_run_suite : ../../common* ../../IOHprofiler* IOHprofiler_run_suite.cpp
	g++ ${CCFLAGS} -o IOHprofiler_run_suite ../../common* ../../IOHprofiler* IOHprofiler_run_suite.cpp ${LDFLAGS}
IOHprofiler_run_problem : ../../common* ../../IOHprofiler* IOHprofiler_run_problem.cpp
	g++ ${CCFLAGS} -o IOHprofiler_run_problem ../../common* ../../IOHprofiler* IOHprofiler_run_problem.cpp ${LDFLAGS}
