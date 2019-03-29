/**
 * @file logger_PBO.c
 * @brief Implementation of the IOHprofiler logger.
 *
 * Logs the performance of a single-objective optimizer on noisy or noiseless problems.
 * It produces four kinds of files:
 * - The "info" files ...
 * - The "dat" files ...
 * - The "tdat" files ...
 * - The "rdat" files ...
 */

/* TODO: Document this file in doxygen style! */

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <errno.h>
#include <stdarg.h>
#include "IOHprofiler.h"

#include "IOHprofiler_utilities.c"
#include "IOHprofiler_problem.c"
#include "IOHprofiler_string.c"
#include "observer_PBO.c"

/*static const size_t IOHprofiler_nbpts_nbevals = 20; Wassim: tentative, are now observer options with these default values*/
/*static const size_t IOHprofiler_nbpts_fval = 5;*/
static size_t PBO_current_dim = 0;
static size_t PBO_current_funId = 0;
static size_t PBO_infoFile_firstInstance = 0;
char PBO_infoFile_firstInstance_char[3];
/* a possible solution: have a list of dims that are already in the file, if the ones we're about to log
 * is != PBO_current_dim and the funId is currend_funId, create a new .info file with as suffix the
 * number of the first instance */
static const int PBO_number_of_dimensions = 6;
static size_t PBO_dimensions_in_current_infoFile[6] = { 0, 0, 0, 0, 0, 0 }; /* TODO should use dimensions from the suite */

/* The current_... mechanism fails if several problems are open.
 * For the time being this should lead to an error.
 *
 * A possible solution: PBO_logger_is_open becomes a reference
 * counter and as long as another logger is open, always a new info
 * file is generated.
 * TODO: Shouldn't the new way of handling observers already fix this?
 */
static int PBO_logger_is_open = 0; /* this could become lock-list of .info files */

/* TODO: add possibility of adding a prefix to the index files (easy to do through observer options) */
static double PBO_fmax(double a, double b) {
  return (a > b) ? a : b;
}

/**
 * @brief The IOHprofiler logger data type.
 */
typedef struct {
  IOHprofiler_observer_t *observer;
  int is_initialized;
  /*char *path;// relative path to the data folder. //Wassim: now fetched from the observer */
  /*const char *alg_name; the alg name, for now, temporarily the same as the path. Wassim: Now in the observer */
  FILE *index_file; /* index file */
  FILE *fdata_file; /* function value aligned data file */
  FILE *tdata_file; /* number of function evaluations aligned data file */
  /*
  FILE *rdata_file;  restart info data file */
  
  FILE *idata_file; /* fixed interval aligned data file */
  FILE *cdata_file; /* complete data file */

  int f_flag; /* symbol for writing fdata_file*/
  int t_flag; /* symbol for writing tdata_file*/
  int i_flag; /* symbol for writing idata_file*/
  int c_flag; /* symbol for writing cdata_file*/
  size_t number_of_evaluations;
  double best_fvalue;
  double last_fvalue;
  double raw_value;
  double best_raw_value;
  short written_last_eval; /* allows writing the the data of the final fun eval in the .tdat file if not already written by the t_trigger*/
  int *best_solution;
  /* The following are to only pass data as a parameter in the free function. The
   * interface should probably be the same for all free functions so passing the
   * problem as a second parameter is not an option even though we need info
   * form it.*/
  size_t function_id; /*TODO: consider changing name*/
  size_t instance_id;
  size_t number_of_variables;
  size_t inner_dimension;
  double optimal_fvalue;

  size_t number_of_parameters;
  double *parameters;

  IOHprofiler_observer_targets_t *targets;          /**< @brief Triggers based on target values. */
  IOHprofiler_observer_evaluations_t *evaluations;  /**< @brief Triggers based on the number of evaluations. */
  IOHprofiler_observer_update_t *updating;          /**< @brief Triggers based on the updating. */
  size_t interval;                    /**< @brief The value of triggers interval. */

} logger_PBO_data_t;

static const char *PBO_file_header_str = "\"function evaluation\" "
    "\"current f(x)\" "
    "\"best-so-far f(x)\" "
    "\"current af(x)+b\"  "
    "\"best af(x)+b\" ";

static const char *PBO_file_para_str = "\"%s\" ";

/**
 * adds a formated line to a data file
 */
static void logger_PBO_write_data(FILE *target_file,
                                   size_t number_of_evaluations,
                                   double fvalue,
                                   double best_fvalue,
                                   double best_raw_value,
                                   double raw_value,
                                   const double *parameter,
                                   size_t number_of_parameters) {
  /* for some reason, it's %.0f in the old code instead of the 10.9e
   * in the documentation
   */
  size_t i;
  fprintf(target_file, "%lu %+10.5e %+10.5e %+10.5e %+10.5e", (unsigned long) number_of_evaluations,
      raw_value, best_raw_value, fvalue , best_fvalue);
    
  for (i = 0; i < number_of_parameters; i++) {
    fprintf(target_file, " %.6f", parameter[i]);
  }

  fprintf(target_file, "\n");
}

/**
 * Error when trying to create the file "path"
 */
static void logger_PBO_error_io(FILE *path, int errnum) {
  const char *error_format = "Error opening file: %s\n ";
  IOHprofiler_error(error_format, strerror(errnum), path);
}

/**
 * Creates the data files or simply opens it
 */


static void logger_PBO_open_dataFile(FILE **target_file,
                                      const char *path,
                                      const char *dataFile_path,
                                      const char *file_extension) {
  char file_path[IOHprofiler_PATH_MAX] = { 0 };
  char relative_filePath[IOHprofiler_PATH_MAX] = { 0 };
  int errnum;
  strncpy(relative_filePath, dataFile_path,
  IOHprofiler_PATH_MAX - strlen(relative_filePath) - 1);
  strncat(relative_filePath, file_extension,
  IOHprofiler_PATH_MAX - strlen(relative_filePath) - 1);
  IOHprofiler_join_path(file_path, sizeof(file_path), path, relative_filePath, NULL);
  if (*target_file == NULL) {
    *target_file = fopen(file_path, "a+");
    errnum = errno;
    if (*target_file == NULL) {
      logger_PBO_error_io(*target_file, errnum);
    }
  }
}


/**
 * Creates the index file fileName_prefix+problem_id+file_extension in
 * folde_path
 */
static void logger_PBO_openIndexFile(logger_PBO_data_t *logger,
                                      const char *folder_path,
                                      const char *indexFile_prefix,
                                      const char *function_id,
                                      const char *dataFile_path,
                                      const char *suite_name) {
  /* to add the instance number TODO: this should be done outside to avoid redoing this for the .*dat files */
  char used_dataFile_path[IOHprofiler_PATH_MAX] = { 0 };
  int errnum, newLine; /* newLine is at 1 if we need a new line in the info file */
  char function_id_char[3]; /* TODO: consider adding them to logger */
  char file_name[IOHprofiler_PATH_MAX] = { 0 };
  char file_path[IOHprofiler_PATH_MAX] = { 0 };
  FILE **target_file;
  FILE *tmp_file;
  strncpy(used_dataFile_path, dataFile_path, IOHprofiler_PATH_MAX - strlen(used_dataFile_path) - 1);
  if (PBO_infoFile_firstInstance == 0) {
    PBO_infoFile_firstInstance = logger->instance_id;
  }
  sprintf(function_id_char, "%lu", (unsigned long) logger->function_id);
  sprintf(PBO_infoFile_firstInstance_char, "%lu", (unsigned long) PBO_infoFile_firstInstance);
  target_file = &(logger->index_file);
  tmp_file = NULL; /* to check whether the file already exists. Don't want to use target_file */
  strncpy(file_name, indexFile_prefix, IOHprofiler_PATH_MAX - strlen(file_name) - 1);
  strncat(file_name, "_f", IOHprofiler_PATH_MAX - strlen(file_name) - 1);
  strncat(file_name, function_id_char, IOHprofiler_PATH_MAX - strlen(file_name) - 1);
  strncat(file_name, "_i", IOHprofiler_PATH_MAX - strlen(file_name) - 1);
  strncat(file_name, PBO_infoFile_firstInstance_char, IOHprofiler_PATH_MAX - strlen(file_name) - 1);
  strncat(file_name, ".info", IOHprofiler_PATH_MAX - strlen(file_name) - 1);
  IOHprofiler_join_path(file_path, sizeof(file_path), folder_path, file_name, NULL);
  if (*target_file == NULL) {
    tmp_file = fopen(file_path, "r"); /* to check for existence */
    if ((tmp_file) && (PBO_current_dim == logger->inner_dimension)
        && (PBO_current_funId == logger->function_id)) {
        /* new instance of current funId and current dim */
      newLine = 0;
      *target_file = fopen(file_path, "a+");
      if (*target_file == NULL) {
        errnum = errno;
        logger_PBO_error_io(*target_file, errnum);
      }
      fclose(tmp_file);
    } else { /* either file doesn't exist (new funId) or new Dim */
      /* check that the dim was not already present earlier in the file, if so, create a new info file */
      if (PBO_current_dim != logger->inner_dimension) {
        int i, j;
        for (i = 0;
            i < PBO_number_of_dimensions && PBO_dimensions_in_current_infoFile[i] != 0
                && PBO_dimensions_in_current_infoFile[i] != logger->inner_dimension; i++) {
          ; /* checks whether dimension already present in the current infoFile */
        }
        if (i < PBO_number_of_dimensions && PBO_dimensions_in_current_infoFile[i] == 0) {
          /* new dimension seen for the first time */
          PBO_dimensions_in_current_infoFile[i] = logger->inner_dimension;
          newLine = 1;
        } else {
          if (i < PBO_number_of_dimensions) { /* dimension already present, need to create a new file */
            newLine = 0;
            file_path[strlen(file_path) - strlen(PBO_infoFile_firstInstance_char) - 7] = 0; /* truncate the instance part */
            PBO_infoFile_firstInstance = logger->instance_id;
            sprintf(PBO_infoFile_firstInstance_char, "%lu", (unsigned long) PBO_infoFile_firstInstance);
            strncat(file_path, "_i", IOHprofiler_PATH_MAX - strlen(file_name) - 1);
            strncat(file_path, PBO_infoFile_firstInstance_char, IOHprofiler_PATH_MAX - strlen(file_name) - 1);
            strncat(file_path, ".info", IOHprofiler_PATH_MAX - strlen(file_name) - 1);
          } else {/*we have all dimensions*/
            newLine = 1;
          }
          for (j = 0; j < PBO_number_of_dimensions; j++) { /* new info file, reinitialize list of dims */
            PBO_dimensions_in_current_infoFile[j] = 0;
          }
          PBO_dimensions_in_current_infoFile[i] = logger->inner_dimension;
        }
      } else {
        if ( PBO_current_funId != logger->function_id ) {
          /*new function in the same file */
          newLine = 1;
        }
      }
      *target_file = fopen(file_path, "a+"); /* in any case, we append */
      if (*target_file == NULL) {
        errnum = errno;
        logger_PBO_error_io(*target_file, errnum);
      }
      if (tmp_file) { /* File already exists, new dim so just a new line. Also, close the tmp_file */
        if (newLine) {
          fprintf(*target_file, "\n");
        }
        fclose(tmp_file);
      }

      fprintf(*target_file,
          "suite = '%s', funcId = %d, DIM = %lu, algId = '%s', algInfo = '%s'\n",
          suite_name, (int) strtol(function_id, NULL, 10), (unsigned long) logger->inner_dimension,
          logger->observer->algorithm_name, logger->observer->algorithm_info);

      fprintf(*target_file, "%%\n");
      strncat(used_dataFile_path, "_i", IOHprofiler_PATH_MAX - strlen(used_dataFile_path) - 1);
      strncat(used_dataFile_path, PBO_infoFile_firstInstance_char,
      IOHprofiler_PATH_MAX - strlen(used_dataFile_path) - 1);
      fprintf(*target_file, "%s.dat", used_dataFile_path); /* dataFile_path does not have the extension */
      PBO_current_dim = logger->inner_dimension;
      PBO_current_funId = logger->function_id;
    }
  }
}

/**
 * Generates the different files and folder needed by the logger to store the
 * data if these don't already exist
 */
static void logger_PBO_initialize(logger_PBO_data_t *logger, IOHprofiler_problem_t *inner_problem) {
  /*
   Creates/opens the data and index files
   */
  char dataFile_path[IOHprofiler_PATH_MAX] = { 0 }; /* relative path to the .dat file from where the .info file is */
  char folder_path[IOHprofiler_PATH_MAX] = { 0 };
  char *tmpc_funId; /* serves to extract the function id as a char *. There should be a better way of doing this! */
  char *tmpc_dim; /* serves to extract the dimension as a char *. There should be a better way of doing this! */
  char indexFile_prefix[15] = "IOHprofiler"; /* TODO (minor): make the prefix IOHprofilerexp a parameter that the user can modify */
  size_t str_length_funId, str_length_dim;
  
  str_length_funId = IOHprofiler_double_to_size_t(PBO_fmax(1, ceil(log10((double) IOHprofiler_problem_get_suite_dep_function(inner_problem)))));
  str_length_dim = IOHprofiler_double_to_size_t(PBO_fmax(1, ceil(log10((double) inner_problem->dimension))));
  tmpc_funId = IOHprofiler_allocate_string(str_length_funId);
  tmpc_dim = IOHprofiler_allocate_string(str_length_dim);

  assert(logger != NULL);
  assert(inner_problem != NULL);
  assert(inner_problem->problem_id != NULL);

  sprintf(tmpc_funId, "%lu", (unsigned long) IOHprofiler_problem_get_suite_dep_function(inner_problem));
  sprintf(tmpc_dim, "%lu", (unsigned long) inner_problem->dimension);

  /* prepare paths and names */
  strncpy(dataFile_path, "data_f", IOHprofiler_PATH_MAX);
  strncat(dataFile_path, tmpc_funId,
  IOHprofiler_PATH_MAX - strlen(dataFile_path) - 1);
  IOHprofiler_join_path(folder_path, sizeof(folder_path), logger->observer->result_folder, dataFile_path,
  NULL);
  IOHprofiler_create_directory(folder_path);
  strncat(dataFile_path, "/IOHprofiler_f",
  IOHprofiler_PATH_MAX - strlen(dataFile_path) - 1);
  strncat(dataFile_path, tmpc_funId,
  IOHprofiler_PATH_MAX - strlen(dataFile_path) - 1);
  strncat(dataFile_path, "_DIM", IOHprofiler_PATH_MAX - strlen(dataFile_path) - 1);
  strncat(dataFile_path, tmpc_dim, IOHprofiler_PATH_MAX - strlen(dataFile_path) - 1);

  /* index/info file */
  assert(IOHprofiler_problem_get_suite(inner_problem));
  logger_PBO_openIndexFile(logger, logger->observer->result_folder, indexFile_prefix, tmpc_funId,
      dataFile_path, IOHprofiler_problem_get_suite(inner_problem)->suite_name);
  fprintf(logger->index_file, ", %lu", (unsigned long) IOHprofiler_problem_get_suite_dep_instance(inner_problem));
  /* data files */
  /* TODO: definitely improvable but works for now */
  strncat(dataFile_path, "_i", IOHprofiler_PATH_MAX - strlen(dataFile_path) - 1);
  strncat(dataFile_path, PBO_infoFile_firstInstance_char,
  IOHprofiler_PATH_MAX - strlen(dataFile_path) - 1);

  if(logger->f_flag == 1){
    logger_PBO_open_dataFile(&(logger->fdata_file), logger->observer->result_folder, dataFile_path, ".dat");
    fprintf(logger->fdata_file,"%s", PBO_file_header_str);
    if(logger->observer->parameters_name != NULL)
      fprintf(logger->fdata_file, PBO_file_para_str,logger->observer->parameters_name);
    fprintf(logger->fdata_file, "\n");
  }

  if(logger->t_flag == 1){
    logger_PBO_open_dataFile(&(logger->tdata_file), logger->observer->result_folder, dataFile_path, ".tdat");
    fprintf(logger->tdata_file,"%s", PBO_file_header_str);
    if(logger->observer->parameters_name != NULL)
      fprintf(logger->tdata_file, PBO_file_para_str,logger->observer->parameters_name);
    fprintf(logger->tdata_file, "\n");
  }

  if(logger->i_flag == 1){
    logger_PBO_open_dataFile(&(logger->idata_file), logger->observer->result_folder, dataFile_path, ".idat");
    fprintf(logger->idata_file, "%s",PBO_file_header_str);
    if(logger->observer->parameters_name != NULL)
      fprintf(logger->idata_file, PBO_file_para_str,logger->observer->parameters_name);
    fprintf(logger->idata_file, "\n");
  }

  if(logger->c_flag == 1){
    logger_PBO_open_dataFile(&(logger->cdata_file), logger->observer->result_folder, dataFile_path, ".cdat");
    fprintf(logger->cdata_file, "%s",PBO_file_header_str);
    if(logger->observer->parameters_name != NULL)
      fprintf(logger->cdata_file, PBO_file_para_str,logger->observer->parameters_name);
    fprintf(logger->cdata_file, "\n");
  }
  /*
  logger_PBO_open_dataFile(&(logger->rdata_file), logger->observer->result_folder, dataFile_path, ".rdat");
  fprintf(logger->rdata_file,"%s", PBO_file_header_str);
  if(logger->observer->parameters_name != NULL)
    fprintf(logger->rdata_file, PBO_file_para_str,logger->observer->parameters_name);
  fprintf(logger->rdata_file, "\n");
  */
  logger->is_initialized = 1;
  IOHprofiler_free_memory(tmpc_dim);
  IOHprofiler_free_memory(tmpc_funId);
}

/**
 * Layer added to the transformed-problem evaluate_function by the logger
 */
static void logger_PBO_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  logger_PBO_data_t *logger = (logger_PBO_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  IOHprofiler_problem_t * inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);


  size_t i;
  if (!logger->is_initialized) {
    logger_PBO_initialize(logger, inner_problem);
  }
  if ((IOHprofiler_log_level >= IOHprofiler_DEBUG) && logger->number_of_evaluations == 0) {
    IOHprofiler_debug("%4lu: ", (unsigned long) inner_problem->suite_dep_index);
    IOHprofiler_debug("on problem %s ... ", IOHprofiler_problem_get_id(inner_problem));
  }
  IOHprofiler_evaluate_function(inner_problem, x, y);
  
  /*Update parameters' info*/
  logger->number_of_parameters = problem->number_of_parameters;
  if(logger->number_of_parameters > 0){
    if(logger->parameters != NULL){
        IOHprofiler_free_memory(logger->parameters);
    }
    logger->parameters = IOHprofiler_allocate_vector(logger->number_of_parameters);
    for(i = 0; i < logger->number_of_parameters; ++i)
      logger->parameters[i] = problem->parameters[i];
  }
  logger->last_fvalue = y[0];
  logger->written_last_eval = 0;
  if (logger->number_of_evaluations == 0 || y[0] > logger->best_fvalue) {
    logger->best_fvalue = y[0];

    for (i = 0; i < problem->number_of_variables; i++)
      logger->best_solution[i] = x[i];
  }
  logger->raw_value = inner_problem->raw_fitness[0];
  if(logger->number_of_evaluations == 0 || logger->raw_value > logger->best_raw_value)
    logger->best_raw_value = logger->raw_value;

  
  logger->number_of_evaluations++;
  if(logger->f_flag == 1){
    if (IOHprofiler_observer_update_trigger(logger->updating,y[0])) {
      logger->best_fvalue = y[0];
      for (i = 0; i < problem->number_of_variables; i++)
        logger->best_solution[i] = x[i];
        logger_PBO_write_data(
          logger->fdata_file, 
          logger->number_of_evaluations,
          y[0], 
          logger->best_fvalue,
          logger->best_raw_value,
          logger->raw_value,
          problem->parameters, 
          problem->number_of_parameters);
    }
  }
  if(logger->t_flag == 1){
    /* Add a line in the .tdat file each time an fevals trigger is reached.*/
    if (IOHprofiler_observer_evaluations_trigger(logger->evaluations, logger->number_of_evaluations)) {
      logger_PBO_write_data(
          logger->tdata_file, 
          logger->number_of_evaluations,
          y[0], 
          logger->best_fvalue,
          logger->best_raw_value,
          logger->raw_value,
          problem->parameters, 
          problem->number_of_parameters);
      logger->written_last_eval = 1;
    }
  }

  if(logger->i_flag == 1){
    if ( logger->number_of_evaluations % logger->interval == 0) {
      logger_PBO_write_data(
          logger->idata_file, 
          logger->number_of_evaluations,
          y[0], 
          logger->best_fvalue,         
          logger->best_raw_value,
          logger->raw_value,
          problem->parameters, 
          problem->number_of_parameters);
    }
  }

  if(logger->c_flag == 1){
    logger_PBO_write_data(
        logger->cdata_file, 
        logger->number_of_evaluations,
        y[0], 
        logger->best_fvalue,          
        logger->best_raw_value,
        logger->raw_value,
        problem->parameters, 
        problem->number_of_parameters);
  }
  /* Add sanity check for optimal f value */
  assert(y[0] <= logger->optimal_fvalue);

  /* Flush output so that impatient users can see progress. */
  if(logger->f_flag == 1)
    fflush(logger->fdata_file);
}

/**
 * Also serves as a finalize run method so. Must be called at the end
 * of Each run to correctly fill the index file
 *
 * TODO: make sure it is called at the end of each run or move the
 * writing into files to another function
 */
static void logger_PBO_free(void *stuff) {
  /* TODO: do all the "non simply freeing" stuff in another function
   * that can have problem as input
   */
  logger_PBO_data_t *logger = (logger_PBO_data_t *) stuff;

  if ((IOHprofiler_log_level >= IOHprofiler_DEBUG) && logger && logger->number_of_evaluations > 0) {
    IOHprofiler_debug("best f=%e after %lu fevals (done observing)\n", logger->best_fvalue,
        (unsigned long) logger->number_of_evaluations);
  }
  if (logger->index_file != NULL) {
    fprintf(logger->index_file, ":%lu|%.5e", (unsigned long) logger->number_of_evaluations,
        logger->best_fvalue);
    fclose(logger->index_file);
    logger->index_file = NULL;
  }
  if (logger->fdata_file != NULL) {
    fclose(logger->fdata_file);
    logger->fdata_file = NULL;
  }
  if (logger->tdata_file != NULL) {
    /* TODO: make sure it handles restarts well. i.e., it writes
     * at the end of a single run, not all the runs on a given
     * instance. Maybe start with forcing it to generate a new
     * "instance" of problem for each restart in the beginning
     */
    if (!logger->written_last_eval) {
      logger_PBO_write_data(logger->tdata_file, logger->number_of_evaluations, logger->last_fvalue,
          logger->best_fvalue, logger->best_raw_value, logger->raw_value, logger->parameters, logger->number_of_parameters);
    }
    fclose(logger->tdata_file);
    logger->tdata_file = NULL;
  }
  /*
  if (logger->rdata_file != NULL) {
    fclose(logger->rdata_file);
    logger->rdata_file = NULL;
  }*/
  if (logger->idata_file != NULL) {
    logger_PBO_write_data(logger->idata_file, logger->number_of_evaluations, logger->last_fvalue,
          logger->best_fvalue, logger->best_raw_value, logger->raw_value, logger->parameters, logger->number_of_parameters);
    fclose(logger->idata_file);
    logger->idata_file = NULL;
  }
  
  if (logger->cdata_file != NULL) {
    fclose(logger->cdata_file);
    logger->cdata_file = NULL;
  }

  if (logger->best_solution != NULL) {
    IOHprofiler_free_memory(logger->best_solution);
    logger->best_solution = NULL;
  }

  if (logger->targets != NULL){
    IOHprofiler_free_memory(logger->targets);
    logger->targets = NULL;
  }

  if (logger->evaluations != NULL){
    IOHprofiler_observer_evaluations_free(logger->evaluations);
    logger->evaluations = NULL;
  }

  if (logger->updating != NULL){
    IOHprofiler_observer_update_free(logger->updating);
    logger->updating = NULL;
  }

  if (logger->parameters != NULL){
    IOHprofiler_free_memory(logger->parameters);
    logger->parameters = NULL;
  }

  PBO_logger_is_open = 0;
}

static IOHprofiler_problem_t *logger_PBO(IOHprofiler_observer_t *observer, IOHprofiler_problem_t *inner_problem) {
  logger_PBO_data_t *logger_PBO;
  IOHprofiler_problem_t *problem;

  logger_PBO = (logger_PBO_data_t *) IOHprofiler_allocate_memory(sizeof(*logger_PBO));
  logger_PBO->observer = observer;

  if (inner_problem->number_of_objectives != 1) {
    IOHprofiler_warning("logger_PBO(): The IOHprofiler logger shouldn't be used to log a problem with %d objectives",
        inner_problem->number_of_objectives);
  }

  if (PBO_logger_is_open)
    IOHprofiler_error("The current IOHprofiler_logger (observer) must be closed before a new one is opened");
  /* This is the name of the folder which happens to be the algName */
  /*logger->path = IOHprofiler_strdup(observer->output_folder);*/
  logger_PBO->index_file = NULL;
  logger_PBO->fdata_file = NULL;
  logger_PBO->tdata_file = NULL;
  /* delete rdat files*/
  /*
  logger_PBO->rdata_file = NULL;
  */
  logger_PBO->cdata_file = NULL;
  logger_PBO->idata_file = NULL;
  logger_PBO->f_flag = 0;
  logger_PBO->t_flag = 0;
  logger_PBO->i_flag = 0;
  logger_PBO->c_flag = 0;



  
  logger_PBO->f_flag = 1;
  if(strcmp(observer->base_evaluation_triggers,"") != 0)
    logger_PBO->t_flag = 1;
  if(observer->number_interval_triggers != 0)
    logger_PBO->i_flag = 1;
  if(strcmp(observer->complete_triggers,"true") == 0)
    logger_PBO->c_flag = 1;
  logger_PBO->number_of_variables = inner_problem->number_of_variables;
  logger_PBO->inner_dimension = inner_problem->dimension;
  logger_PBO->number_of_parameters = inner_problem->number_of_parameters;
  logger_PBO->parameters = NULL;
  if (inner_problem->best_value == NULL) {
    /* IOHprofiler_error("Optimal f value must be defined for each problem in order for the logger to work properly"); */
    /* Setting the value to 0 results in the assertion y>=optimal_fvalue being susceptible to failure */
    IOHprofiler_warning("undefined optimal f value. Set to 0");
    logger_PBO->optimal_fvalue = 0;
  } else {
    logger_PBO->optimal_fvalue = *(inner_problem->best_value);
  }

  logger_PBO->number_of_evaluations = 0;
  logger_PBO->best_solution = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);
  /* TODO: the following inits are just to be in the safe side and
   * should eventually be removed. Some fields of the IOHprofiler_logger struct
   * might be useless
   */
  logger_PBO->function_id = IOHprofiler_problem_get_suite_dep_function(inner_problem);
  logger_PBO->instance_id = IOHprofiler_problem_get_suite_dep_instance(inner_problem);
  logger_PBO->written_last_eval = 1;
  logger_PBO->last_fvalue = -DBL_MAX;
  logger_PBO->is_initialized = 0;
  logger_PBO->best_fvalue = -DBL_MAX;
  logger_PBO->best_raw_value  = -DBL_MAX;
  /* Initialize triggers based on target values and number of evaluations */
  logger_PBO->targets = IOHprofiler_observer_targets(observer->number_target_triggers, observer->target_precision);
  logger_PBO->evaluations = IOHprofiler_observer_evaluations(observer->base_evaluation_triggers, inner_problem->number_of_variables);
  logger_PBO->updating = IOHprofiler_observer_update();
  logger_PBO->interval = observer->number_interval_triggers;
  problem = IOHprofiler_problem_transformed_allocate(inner_problem, logger_PBO, logger_PBO_free, observer->observer_name);

  problem->evaluate_function = logger_PBO_evaluate;
  PBO_logger_is_open = 1;
  return problem;
}
