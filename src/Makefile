SUBDIRS=Suites \
		Problems \
	    Template
CPP_SOURCE=${wildcard *.cpp}
CPP_OBJS=${patsubst %.cpp, $(ROOT_DIR)/$(T_DIR)/IOH/%.cpp, $(CPP_SOURCE)}
HPP_SOURCE=${wildcard *.hpp}
HPP_OBJS=${patsubst %.hpp, $(ROOT_DIR)/$(T_DIR)/IOH/%.hpp, $(HPP_SOURCE)}
H_SOURCE=${wildcard *.h}
H_OBJS=${patsubst %.h, $(ROOT_DIR)/$(T_DIR)/IOH/%.h, $(H_SOURCE)}
all:$(SUBDIRS) $(CPP_OBJS) $(HPP_OBJS) $(H_OBJS)
$(SUBDIRS):ECHO
	make -C $@
$(CPP_OBJS):$(ROOT_DIR)/$(T_DIR)/IOH/%.cpp:%.cpp
	cp $^ $@
$(HPP_OBJS):$(ROOT_DIR)/$(T_DIR)/IOH/%.hpp:%.hpp
	cp $^ $@
$(H_OBJS):$(ROOT_DIR)/$(T_DIR)/IOH/%.h:%.h
	cp $^ $@
ECHO:
	@echo $(SUBDIRS)