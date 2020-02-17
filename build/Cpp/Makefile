CC=g++
LDFLAGS = 
CLIBFLAGS = -soname
CXXFLAGS = -g -fPIC -shared -std=c++11 -Wall -Wno-unused-variable -Wno-sign-compare -Wno-unused-function -O2
OBJ_DIR=obj
CPP_SOURCE=${wildcard IOH/*.cpp}
CPP_OBJS=${patsubst IOH/%.cpp, $(OBJ_DIR)/%.o, $(CPP_SOURCE)}
HPP_SOURCE=${wildcard IOH/*.hpp}
HPP_OBJS=${patsubst IOH/%.hpp, $(OBJ_DIR)/%.o, $(HPP_SOURCE)}
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
all: PREPARE bin/$(BIN1) bin/$(BIN2) bin/$(BIN3) CLEAN ECHO
PREPARE:
	mkdir -p ./$(OBJ_DIR)
	mkdir -p ./bin
$(CPP_OBJS):$(OBJ_DIR)/%.o:IOH/%.cpp
	$(CC) ${CXXFLAGS} -c -x c++ $^ -o $@ ${LDFLAGS}
$(HPP_OBJS):$(OBJ_DIR)/%.o:IOH/%.hpp
	$(CC) ${CXXFLAGS} -c -x c++ $^ -o $@ ${LDFLAGS}
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
	rm -rf ./$(OBJ_DIR)
ECHO:
	@echo "Please copy the `libIOH.so` files to the the LD_LIBRARY_PATH and `IOH` folder .h, .hpp files to CPLUS_INCLUDE_PATH."
	@echo For Linux:
	@echo "sudo cp libIOH.so libIOH.so.0 libIOH.so.0.0 /usr/lib/"
	@echo "sudo cp -rf ./IOH/ /usr/include/"
	@echo For MacOS:
	@echo "sudo cp libIOH.so libIOH.so.0 libIOH.so.0.0 /usr/local/lib"
	@echo "sudo cp -rf ./IOH/*.h ./IOH/*.hpp /usr/local/include/"
	@echo 
	@echo "Or add the path to LD_LIBRARY_PATH and CPLUS_INCLUDE_PATH"
	@echo "echo \"export LD_LIBRARY_PATH={path where libIOH.so files locate: /build/Cpp/}:$LD_LIBRARY_PATH\" >> ~/.bashrc"
	@echo "echo \"export CPLUS_INCLUDE_PATH={path where the /IOH folder locates: /build/Cpp}:$CPLUS_INCLUDE_PATH\" >> ~/.bashrc"
	@echo "source ~/.bashrc"