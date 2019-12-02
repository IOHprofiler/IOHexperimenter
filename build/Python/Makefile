SUBDIRS=src
POBJ=_IOHprofiler.so
all:$(SUBDIRS) $(POBJ) 
$(SUBDIRS):ECHO
	make -C $@
$(POBJ):$(POBJ)
	swig -c++ -python IOHprofiler.i
	g++ -O2 -fPIC -std=c++11 -c IOHprofiler_wrap.cxx -I/usr/include/python2.7 -lboost_system -lboost_filesystem
	g++ -shared build/*.o IOHprofiler_wrap.o -o $@ -lboost_system -lboost_filesystem -std=c++11
ECHO:
	@echo $(SUBDIRS)

