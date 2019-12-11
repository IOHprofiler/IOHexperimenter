
PYTHONVERSION=/usr/include/python2.7
LDFLAGS += -lm -lboost_system -lboost_filesystem
CCFLAGS = -g -std=c++11 -Wall -Wno-unused-variable -Wno-sign-compare -Wno-unused-function -O2 -fPIC
ODIR=build
SOURCE=src
CPP_SOURCE=${wildcard $(SOURCE)/*.cpp}
CPP_OBJS=${patsubst $(SOURCE)/%.cpp, $(ODIR)/%.o, $(CPP_SOURCE)}
HPP_SOURCE=${wildcard $(SOURCE)/*.hpp}
HPP_OBJS=${patsubst $(SOURCE)/%.hpp, $(ODIR)/%.o, $(HPP_SOURCE)}
POBJ=_IOHprofiler.so
all: $(CPP_OBJS) $(HPP_OBJS) $(POBJ)
$(CPP_OBJS):$(ODIR)/%.o:$(SOURCE)/%.cpp
	$(CC) ${CCFLAGS} -c -x c++ $^ -o $@ ${LDFLAGS}
$(HPP_OBJS):$(ODIR)/%.o:$(SOURCE)/%.hpp
	$(CC) ${CCFLAGS} -c -x c++ $^ -o $@ ${LDFLAGS}
$(POBJ):$(HPP_OBJS) $(CPP_OBJS) IOHprofiler.i
	swig -c++ -python IOHprofiler.i
	g++ -O2 -fPIC -std=c++11 -c IOHprofiler_wrap.cxx -I$(PYTHONVERSION) -lboost_system -lboost_filesystem
	g++ -shared $(HPP_OBJS) $(CPP_OBJS) IOHprofiler_wrap.o -o $@ -lboost_system -lboost_filesystem -std=c++11
ECHO:
	@echo $(HPPSUBDIRS)
