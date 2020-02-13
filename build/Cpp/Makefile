CC=g++
LDFLAGS = 
CLIBFLAGS = -soname
CCFLAGS = -g -fPIC -shared -std=c++11 -Wall -Wno-unused-variable -Wno-sign-compare -Wno-unused-function -O2
ODIR=obj
CPP_SOURCE=${wildcard IOH/*.cpp}
CPP_OBJS=${patsubst IOH/%.cpp, $(ODIR)/%.o, $(CPP_SOURCE)}
HPP_SOURCE=${wildcard IOH/*.hpp}
HPP_OBJS=${patsubst IOH/%.hpp, $(ODIR)/%.o, $(HPP_SOURCE)}
LIB_OBJ=libIOH.so
LIB_DIR=/usr/lib/
INCLUDE_DIR=/usr/include/
BIN1=IOHprofiler_run_experiment
BIN2=IOHprofiler_run_suite
BIN3=IOHprofiler_run_problem
UNAME_S = $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  LIB_DIR = /usr/local/lib
	INCLUDE_DIR=/usr/local/include/
	CLIBFLAGS = -install_name
endif
all: PREPARE bin/$(BIN1) bin/$(BIN2) bin/$(BIN3) CLEAN
PREPARE:
	mkdir -p ./$(ODIR)
	mkdir -p ./bin
$(CPP_OBJS):$(ODIR)/%.o:IOH/%.cpp
	$(CC) ${CCFLAGS} -c -x c++ $^ -o $@ ${LDFLAGS}
$(HPP_OBJS):$(ODIR)/%.o:IOH/%.hpp
	$(CC) ${CCFLAGS} -c -x c++ $^ -o $@ ${LDFLAGS}
$(LIB_OBJ):${CPP_OBJS} ${HPP_OBJS}
	$(CC) -shared -Wl,$(CLIBFLAGS),libIOH.so.0 -o libIOH.so.0.0 $^
	ln -sf libIOH.so.0.0 libIOH.so.0
	ln -sf libIOH.so.0 libIOH.so
#	sudo cp libIOH.so libIOH.so.0 libIOH.so.0.0 $(LIB_DIR)
# sudo cp -r ./IOH/ $(INCLUDE_DIR)
bin/$(BIN1):IOHprofiler_run_experiment.cpp $(LIB_OBJ)
	cp configuration.ini bin/
	$(CC) -std=c++11 -g -pthread -o $@ $^ -L. -lIOH
bin/$(BIN2):IOHprofiler_run_suite.cpp $(LIB_OBJ)
	$(CC) -std=c++11 -g -pthread -o $@ $^ -L. -lIOH
bin/$(BIN3):IOHprofiler_run_problem.cpp $(LIB_OBJ)
	$(CC) -std=c++11 -g -pthread -o $@ $^ -L. -lIOH
CLEAN:
	rm -rf ./$(ODIR)