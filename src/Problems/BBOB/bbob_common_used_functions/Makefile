CPP_SOURCE=${wildcard *.cpp}
CPP_OBJS=${patsubst %.cpp, $(ROOT_DIR)/$(OBJS_DIR)/%.o, $(CPP_SOURCE)}
HPP_SOURCE=${wildcard *.hpp}
HPP_OBJS=${patsubst %.hpp, $(ROOT_DIR)/$(OBJS_DIR)/%.o, $(HPP_SOURCE)}
all:$(SUBDIRS) $(CPP_OBJS) $(HPP_OBJS)
$(SUBDIRS):ECHO
	make -C $@
$(CPP_OBJS):$(ROOT_DIR)/$(OBJS_DIR)/%.o:%.cpp
	$(CC) ${CCFLAGS} -c -x c++ $^ -o $@ ${LDFLAGS}
$(HPP_OBJS):$(ROOT_DIR)/$(OBJS_DIR)/%.o:%.hpp
	$(CC) ${CCFLAGS} -c  -x c++ $^ -o $@ ${LDFLAGS}
ECHO:
	@echo $(SUBDIRS)