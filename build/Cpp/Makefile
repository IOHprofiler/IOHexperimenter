CC=g++
LDFLAGS += -lm
CCFLAGS = -g -std=c++11 -Wall -Wno-unused-variable -Wno-sign-compare -Wno-unused-function -O2
OBJS=*.o
ODIR=obj
CPP_SOURCE=${wildcard src/*.cpp}
CPP_OBJS=${patsubst src/%.cpp, $(ODIR)/%.o, $(CPP_SOURCE)}
HPP_SOURCE=${wildcard src/*.hpp}
HPP_OBJS=${patsubst src/%.hpp, $(ODIR)/%.o, $(HPP_SOURCE)}
BIN1=IOHprofiler_run_experiment
BIN2=IOHprofiler_run_suite
BIN3=IOHprofiler_run_problem
all: $(CPP_OBJS) $(HPP_OBJS) bin/$(BIN1) bin/$(BIN2) bin/$(BIN3)
$(CPP_OBJS):$(ODIR)/%.o:src/%.cpp
	$(CC) ${CCFLAGS} -c -x c++ $^ -o $@ ${LDFLAGS}
$(HPP_OBJS):$(ODIR)/%.o:src/%.hpp
	$(CC) ${CCFLAGS} -c -x c++ $^ -o $@ ${LDFLAGS}
bin/$(BIN1):$(ODIR)/$(OBJS) IOHprofiler_run_experiment.cpp
	cp configuration.ini bin/
	$(CC) ${CCFLAGS} -pthread -o $@ $^  ${LDFLAGS}
bin/$(BIN2):$(ODIR)/$(OBJS) IOHprofiler_run_suite.cpp
	$(CC) ${CCFLAGS} -pthread -o $@ $^  ${LDFLAGS}
bin/$(BIN3):$(ODIR)/$(OBJS) IOHprofiler_run_problem.cpp
	$(CC) ${CCFLAGS} -pthread -o $@ $^  ${LDFLAGS}
CLEAN:
	@rm -rf bin/IOHprofiler_run_experiment
	@rm -rf bin/IOHprofiler_run_suite
	@rm -rf bin/IOHprofiler_run_problem
