LANG=
CC=g++
LDFLAGS = 
CXXFLAGS = -g -fPIC -shared -std=c++11 -Wall -Wno-unused-variable -Wno-sign-compare -Wno-unused-function -O2
SUB_DIR=src
ROOT_DIR=$(shell pwd)

TAR_DIR=
ifeq ($(LANG),c++)
	@echo 'Compile C++'
	TAR_DIR=build/Cpp
else
ifeq ($(LANG),python)
	TAR_DIR=build/Python
else
	TAR_DIR=build/Cpp
endif
endif
export CC TAR_DIR ROOT_DIR CXXFLAGS LDFLAGS
all:$(SUB_DIR) DEBUG
$(SUB_DIR):ECHO
	mkdir -p $(TAR_DIR)/IOH
	mkdir -p $(TAR_DIR)/bin
	mkdir -p $(TAR_DIR)/obj
	make -C $@
DEBUG:ECHO
	make -C $(TAR_DIR)
ECHO:
	@echo $(SUB_DIR)
