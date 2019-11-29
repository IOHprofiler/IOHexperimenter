SUBDIRS=
CPP_SOURCE=${wildcard *.cpp}
CPP_OBJS=${patsubst %.cpp, ./%.o, $(CPP_SOURCE)}
HPP_SOURCE=${wildcard *.hpp}
HPP_OBJS=${patsubst %.hpp, ./%.o, $(HPP_SOURCE)}
all:$(SUBDIRS) $(CPP_OBJS) $(HPP_OBJS)
$(CPP_OBJS):./%.o:%.cpp
	g++ -fPIC -g -std=c++11 -Wall -Wno-unused-variable -Wno-sign-compare -Wno-unused-function -O2 -c -x  c++ $^ -o $@ ${LDFLAGS}
$(HPP_OBJS):./%.o:%.hpp
	g++ -fPIC -g -std=c++11 -Wall -Wno-unused-variable -Wno-sign-compare -Wno-unused-function -O2 -c -x c++ $^ -o $@ ${LDFLAGS}
ECHO:
	@echo $(SUBDIRS)