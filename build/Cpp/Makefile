CC=g++
LDFLAGS += -lm -lboost_system -lboost_filesystem
CCFLAGS = -g -std=c++11 -Wall -Wno-unused-variable -Wno-sign-compare -Wno-unused-function -O2
OBJS=*.o
ODIR=obj
BIN1=IOHprofiler_run_experiment
BIN2=IOHprofiler_run_suite
BIN3=IOHprofiler_run_problem
all: bin/$(BIN1) bin/$(BIN2) bin/$(BIN3)
bin/$(BIN1):$(ODIR)/$(OBJS)
	cp configuration.ini bin/
	$(CC) ${CCFLAGS} -pthread -o $@ $^ IOHprofiler_run_experiment.cpp ${LDFLAGS}
bin/$(BIN2):$(ODIR)/$(OBJS)
	$(CC) ${CCFLAGS} -pthread -o $@ $^ IOHprofiler_run_suite.cpp ${LDFLAGS}
bin/$(BIN3):$(ODIR)/$(OBJS)
	$(CC) ${CCFLAGS} -pthread -o $@ $^ IOHprofiler_run_problem.cpp ${LDFLAGS}
CLEAN:
	@rm -rf bin/IOHprofiler_run_experiment
	@rm -rf bin/IOHprofiler_run_suite
	@rm -rf bin/IOHprofiler_run_problem
