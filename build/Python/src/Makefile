SUBDIRS=
DIR_OBJ=../build
CPP_SOURCE=${wildcard *.cpp}
CPP_OBJS=${patsubst %.cpp, $(DIR_OBJ)/%.o, $(CPP_SOURCE)}
HPP_SOURCE=${wildcard *.hpp}
HPP_OBJS=${patsubst %.hpp, $(DIR_OBJ)/%.o, $(HPP_SOURCE)}
all:$(SUBDIRS) $(CPP_OBJS) $(HPP_OBJS)
$(SUBDIRS):ECHO
	make -C $@
$(CPP_OBJS):$(DIR_OBJ)/%.o:%.cpp
	g++ -g -std=c++11 -Wall -Wno-unused-variable -Wno-sign-compare -Wno-unused-function -O2 -c -x c++ -fPIC $^ -o $@ ${LDFLAGS}
$(HPP_OBJS):$(DIR_OBJ)/%.o:%.hpp
	g++ -g -std=c++11 -Wall -Wno-unused-variable -Wno-sign-compare -Wno-unused-function -O2 -c  -x c++ -fPIC $^ -o $@ ${LDFLAGS}
