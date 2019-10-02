CC=g++
LDFLAGS = 
CCFLAGS = -g -std=c++11 -Wall -Wno-unused-variable -Wno-sign-compare -Wno-unused-function -O2
SUBDIRS=src
ROOT_DIR=$(shell pwd)
OBJS_DIR=build/Cpp/obj
BIN_DIR=build/Cpp/bin
export CC BIN OBJS_DIR BIN_DIR ROOT_DIR CCFLAGS LDFLAGS
all:$(SUBDIRS) DEBUG
$(SUBDIRS):ECHO
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJS_DIR)
	make -C $@
DEBUG:ECHO
	make -C build/Cpp
ECHO:
	@echo $(SUBDIRS)
CLEAN:
	@rm $(OBJS_DIR)/*.o