LANG=
CC=g++
LDFLAGS = 
CCFLAGS = -g -std=c++11 -Wall -Wno-unused-variable -Wno-sign-compare -Wno-unused-function -O2
SUBDIRS=src
ROOT_DIR=$(shell pwd)
OBJS_DIR=obj

T_DIR=
ifeq ($(LANG),c++)
	@echo 'Compile C++'
	T_DIR=build/Cpp
else
ifeq ($(LANG),python)
	T_DIR=build/Python
else
	T_DIR=build/Cpp
endif
endif
export CC T_DIR ROOT_DIR CCFLAGS LDFLAGS
all:$(SUBDIRS) DEBUG
$(SUBDIRS):ECHO
	mkdir -p $(T_DIR)/src
	make -C $@
DEBUG:ECHO
	make -C $(T_DIR)
ECHO:
	@echo $(SUBDIRS)
CLEAN:
	@rm -f $(T_DIR)/$(OBJS_DIR)/*.o
	@rm -f $(T_DIR)/$(OBJS_DIR)/*.a
