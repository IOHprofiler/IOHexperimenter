CC=g++
LDFLAGS += -lm -lboost_system -lboost_filesystem
CCFLAGS = -g -std=c++11 -w
SUBDIRS=src
ROOT_DIR=$(shell pwd)
OBJS_DIR=build/Cpp/obj
BIN_DIR=build/Cpp/bin
CPP_SOURCE=${wildcard *.cpp}
CPP_OBJS=${patsubst %.cpp, %.o, $(CPP_SOURCE)}
HPP_SOURCE=${wildcard *.hpp}
HPP_OBJS=${patsubst %.hpp, %.o, $(HPP_SOURCE)}

export CC BIN OBJS_DIR BIN_DIR ROOT_DIR CCFLAGS LDFLAGS
all:$(SUBDIRS) $(CPP_OBJS) $(HPP_OBJS) DEBUG
$(SUBDIRS):ECHO
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJS_DIR)
	make -C $@
DEBUG:ECHO
	make -C build/Cpp
ECHO:
	@echo $(SUBDIRS)
$(CPP_OBJS):%.o:%.cpp
	$(CC) ${CCFLAGS} -c -x c++ $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@ ${LDFLAGS}
$(HPP_OBJS):%.o:%.hpp
	$(CC) ${CCFLAGS} -c -x c++ $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@ ${LDFLAGS}
CLEAN:
	@rm $(OBJS_DIR)/*.o
	@rm -rf $(BIN_DIR)/*