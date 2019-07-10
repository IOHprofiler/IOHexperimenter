CPP_SOURCE=${wildcard *.cpp}
CPP_OBJS=${patsubst %.cpp, %.o, $(CPP_SOURCE)}
HPP_SOURCE=${wildcard *.hpp}
HPP_OBJS=${patsubst %.hpp, %.o, $(HPP_SOURCE)}
all:$(CPP_OBJS) $(HPP_OBJS)
$(SUBDIRS):ECHO
	make -C $@
$(CPP_OBJS):%.o:%.cpp
	$(CC) ${CCFLAGS} -c -x c++ $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@ ${LDFLAGS}
$(HPP_OBJS):%.o:%.hpp
	$(CC) ${CCFLAGS} -c -x c++ $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@ ${LDFLAGS}
ECHO:
	@echo $(SUBDIRS)