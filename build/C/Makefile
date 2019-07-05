OBJS=*.o
ODIR=obj
BIN1=IOHprofiler_run_experiment
BIN2=IOHprofiler_run_suite
BIN3=IOHprofiler_run_problem
all: $(ROOT_DIR)/$(BIN_DIR)/$(BIN1) $(ROOT_DIR)/$(BIN_DIR)/$(BIN2) $(ROOT_DIR)/$(BIN_DIR)/$(BIN3)
$(ROOT_DIR)/$(BIN_DIR)/$(BIN1):$(ODIR)/$(OBJS)
	$(CC) ${CCFLAGS} -pthread -o $@ $^ IOHprofiler_run_experiment.cpp ${LDFLAGS}
	cp configuration.ini $(ROOT_DIR)/$(BIN_DIR)/
$(ROOT_DIR)/$(BIN_DIR)/$(BIN2):$(ODIR)/$(OBJS)
	$(CC) ${CCFLAGS} -pthread -o $@ $^ IOHprofiler_run_suite.cpp ${LDFLAGS}
$(ROOT_DIR)/$(BIN_DIR)/$(BIN3):$(ODIR)/$(OBJS)
	$(CC) ${CCFLAGS} -pthread -o $@ $^ IOHprofiler_run_problem.cpp ${LDFLAGS}