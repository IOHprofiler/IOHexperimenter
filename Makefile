CC=g++
LDFLAGS += -lm -lboost_system -lboost_filesystem
CCFLAGS = -g -std=c++11 -w
SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "build") print $$9}')
ROOT_DIR=$(shell pwd)
OBJS_DIR=build/C/obj
BIN_DIR=build/C/bin
CPP_SOURCE=${wildcard *.cpp}
CPP_OBJS=${patsubst %.cpp, %.o, $(CPP_SOURCE)}
HPP_SOURCE=${wildcard *.hpp}
HPP_OBJS=${patsubst %.hpp, %.o, $(HPP_SOURCE)}
export CC BIN OBJS_DIR BIN_DIR ROOT_DIR CCFLAGS LDFLAGS

all:$(SUBDIRS) $(CPP_OBJS) $(HPP_OBJS) DEBUG
$(SUBDIRS):ECHO
	make -C $@
DEBUG:ECHO
	make -C build/C
ECHO:
	@echo $(SUBDIRS)
$(CPP_OBJS):%.o:%.cpp
	$(CC) ${CCFLAGS} -c $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@ ${LDFLAGS}
$(HPP_OBJS):%.o:%.hpp
	$(CC) ${CCFLAGS} -c $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@ ${LDFLAGS}
CLEAN:
	@rm $(OBJS_DIR)/*.o
	@rm -rf $(BIN_DIR)/*