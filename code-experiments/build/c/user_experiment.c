#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "IOHprofiler.h"

#include "readcfg.c"

char * cfgFile = "configuration.ini";
/**
 * A function type for setting parameters functions, where the first argument is the number of parameters to be stored and the
 * second argument the vector of parameters. The type of parameters is double, please modify the type of other types of variables.
 */
typedef void (*set_parameters_t)(const int number_of_parameters, const double * parameters) ;


/**
 * A function type for evaluation functions, where the first argument is the vector to be evaluated and the
 * second argument the vector to which the evaluation result is stored.
 */
typedef void (*evaluate_function_t)(const int *x, double *y);

/**
 * A pointer to the problem to be optimized (needed in order to simplify the interface between the optimization
 * algorithm and the IOHprofiler platform).
 */
static IOHprofiler_problem_t *PROBLEM;

/**
 * The function that calls the evaluation of the first vector on the problem to be optimized and stores the
 * evaluation result in the second vector.
 */
static void evaluate_function(const int *x, double *y) {
  IOHprofiler_evaluate_function(PROBLEM, x, y);
}

/**
 * The function returns 1 if the optimal is found. 
 */
static int  if_hit_optimal(){
  return IOHprofiler_problem_final_target_hit(PROBLEM);
}

/**
 * The function that set the parameters of the problem with the vector.
 */
static void set_parameters(const size_t number_of_parameters, const double * parameters) {
  IOHprofiler_problem_set_parameters(PROBLEM,number_of_parameters,parameters);
}



void User_Algorithm(evaluate_function_t evaluate,
                      const size_t dimension,
                      const size_t number_of_objectives,
                      const int *lower_bounds,
                      const int *upper_bounds,
                      const size_t max_budget,
                      IOHprofiler_random_state_t *random_generator);

void example_experiment(IOHprofiler_random_state_t *random_generator);

#include "user_algorithm.c"


/* Structure and functions needed for timing the experiment */
typedef struct {
  size_t number_of_dimensions;
  size_t current_idx;
  char **output;
  size_t previous_dimension;
  size_t cumulative_evaluations;
  time_t start_time;
  time_t overall_start_time;
} timing_data_t;
static timing_data_t *timing_data_initialize(IOHprofiler_suite_t *suite);
static void timing_data_time_problem(timing_data_t *timing_data, IOHprofiler_problem_t *problem);
static void timing_data_finalize(timing_data_t *timing_data);


/**
 * The main method initializes the random number generator and calls the example experiment on the
 * IOHprofiler suite.
 */
int main(void) {

  IOHprofiler_random_state_t *random_generator = IOHprofiler_random_new(RANDOM_SEED);

  /* Change the log level to "warning" to get less output */
  IOHprofiler_set_log_level("info");

  printf("Running the example experiment... (might take time, be patient)\n");
  fflush(stdout);

  /** 
   * Start the actual experiments on a test suite and use a matching logger, for
   * example one of the following: 
   *   PBO                 4 exectutable single-objective functions
   *
   * Adapt to your need. Note that the experiment is run according
   * to the settings, defined in example_experiment(...) below.
   */

  readcfg(cfgFile);
  example_experiment(random_generator);

  printf("Done!\n");
  fflush(stdout);

  IOHprofiler_random_free(random_generator);

  return 0;
}

/**
 * A simple example of benchmarking random search on a given suite with default instances
 * that can serve also as a timing experiment.
 *
 * @param suite_name Name of the suite (e.g."PBO").
 * @param observer_name Name of the observer matching with the chosen suite (e.g. "PBO" 
 * when using the "PBO" suite).
 * @param random_generator The random number generator.
 */
void example_experiment(IOHprofiler_random_state_t *random_generator) {

  size_t run;
  IOHprofiler_suite_t *suite;
  IOHprofiler_observer_t *observer;
  timing_data_t *timing_data;

  /* Set some options for the observer. See IOHprofiler_observer.c to find information of options */
  char *observer_options = IOHprofiler_strdupf( "result_folder:  %s "
                                                "algorithm_name: %s "
                                                "algorithm_info: %s "
                                                "complete_triggers: %s "
                                                "number_interval_triggers: %d "
                                                "base_evaluation_triggers: %s "
                                                "number_target_triggers: %d "
                                                "parameters_name: %s", 
                                                result_folder,algorithm_name,algorithm_info,complete_triggers,
                                                number_interval_triggers, base_evaluation_triggers,
                                                number_target_triggers,parameters_name);
  char *suite_instances  = IOHprofiler_strdupf( "instances:  %s ",instances_id);
  char *suite_options    = IOHprofiler_strdupf( "function_indices: %s ""dimensions: %s ", functions_id,dimensions);
  
  /* Initialize the suite and observer.*/
  suite    = IOHprofiler_suite(suite_name, suite_instances,suite_options);
  observer = IOHprofiler_observer(observer_name, observer_options);
  
  IOHprofiler_free_memory(observer_options);
  IOHprofiler_free_memory(suite_instances);
  IOHprofiler_free_memory(suite_options);

  /* Initialize timing */
  timing_data = timing_data_initialize(suite);

  /* Iterate over all problems in the suite */
  while ((PROBLEM = IOHprofiler_suite_get_next_problem(suite, observer)) != NULL) {
    
    size_t dimension = IOHprofiler_problem_get_dimension(PROBLEM);

    /* Run the algorithm at least once */
    for (run = 1; run < 1 + INDEPENDENT_RESTARTS; run++) {
      PROBLEM = IOHprofiler_suite_reset_problem(suite, observer);
      size_t evaluations_done = IOHprofiler_problem_get_evaluations(PROBLEM);
      long evaluations_remaining = (long) (dimension * BUDGET_MULTIPLIER) - (long) evaluations_done;

      /* Break the loop if the target was hit or there are no more remaining evaluations */
      if (IOHprofiler_problem_final_target_hit(PROBLEM) || (evaluations_remaining <= 0)){
        continue;
      }

      /* Call the optimization algorithm for the remaining number of evaluations */
      User_Algorithm( evaluate_function,
                      dimension,
                      IOHprofiler_problem_get_number_of_objectives(PROBLEM),
                      IOHprofiler_problem_get_smallest_values_of_interest(PROBLEM),
                      IOHprofiler_problem_get_largest_values_of_interest(PROBLEM),
                      (size_t) evaluations_remaining,
                      random_generator);

      /* Break the loop if the algorithm performed no evaluations or an unexpected thing happened */
      if (IOHprofiler_problem_get_evaluations(PROBLEM) == evaluations_done) {
        printf("WARNING: Budget has not been exhausted (%lu/%lu evaluations done)!\n", (unsigned long) evaluations_done, (unsigned long) dimension * BUDGET_MULTIPLIER);
        continue;
      }
      else if (IOHprofiler_problem_get_evaluations(PROBLEM) < evaluations_done)
        IOHprofiler_error("Something unexpected happened - function evaluations were decreased!");
    }

    /* Keep track of time */
    timing_data_time_problem(timing_data, PROBLEM);
  }

  /* Output and finalize the timing data */
  timing_data_finalize(timing_data);
  IOHprofiler_observer_free(observer);
  IOHprofiler_suite_free(suite);

}

/**
 * Allocates memory for the timing_data_t object and initializes it.
 */
static timing_data_t *timing_data_initialize(IOHprofiler_suite_t *suite) {

  timing_data_t *timing_data = (timing_data_t *) IOHprofiler_allocate_memory(sizeof(*timing_data));
  size_t function_idx, dimension_idx, instance_idx, i;

  /* Find out the number of all dimensions */
  IOHprofiler_suite_decode_problem_index(suite, IOHprofiler_suite_get_number_of_problems(suite) - 1, &function_idx,
      &dimension_idx, &instance_idx);
  timing_data->number_of_dimensions = dimension_idx + 1;
  timing_data->current_idx = 0;
  timing_data->output = (char **) IOHprofiler_allocate_memory(timing_data->number_of_dimensions * sizeof(char *));
  for (i = 0; i < timing_data->number_of_dimensions; i++) {
    timing_data->output[i] = NULL;
  }
  timing_data->previous_dimension = 0;
  timing_data->cumulative_evaluations = 0;
  time(&timing_data->start_time);
  time(&timing_data->overall_start_time);

  return timing_data;
}

/**
 * Keeps track of the total number of evaluations and elapsed time. Produces an output string when the
 * current problem is of a different dimension than the previous one or when NULL.
 */
static void timing_data_time_problem(timing_data_t *timing_data, IOHprofiler_problem_t *problem) {

  double elapsed_seconds = 0;

  if ((problem == NULL) || (timing_data->previous_dimension != IOHprofiler_problem_get_dimension(problem))) {

    /* Output existing timing information */
    if (timing_data->cumulative_evaluations > 0) {
      time_t now;
      time(&now);
      elapsed_seconds = difftime(now, timing_data->start_time) / (double) timing_data->cumulative_evaluations;
      timing_data->output[timing_data->current_idx++] = IOHprofiler_strdupf("d=%lu done in %.2e seconds/evaluation\n",
          timing_data->previous_dimension, elapsed_seconds);
    }

    if (problem != NULL) {
      /* Re-initialize the timing_data */
      timing_data->previous_dimension = IOHprofiler_problem_get_dimension(problem);
      timing_data->cumulative_evaluations = IOHprofiler_problem_get_evaluations(problem);
      time(&timing_data->start_time);
    }

  } else {
    timing_data->cumulative_evaluations += IOHprofiler_problem_get_evaluations(problem);
  }
}

/**
 * Outputs and finalizes the given timing data.
 */
static void timing_data_finalize(timing_data_t *timing_data) {

  /* Record the last problem */
  timing_data_time_problem(timing_data, NULL);

  if (timing_data) {
    size_t i;
    double elapsed_seconds;
    time_t now;
    int hours, minutes, seconds;

    time(&now);
    elapsed_seconds = difftime(now, timing_data->overall_start_time);

    printf("\n");
    for (i = 0; i < timing_data->number_of_dimensions; i++) {
      if (timing_data->output[i]) {
        printf("%s", timing_data->output[i]);
        IOHprofiler_free_memory(timing_data->output[i]);
      }
    }
    hours = (int) elapsed_seconds / 3600;
    minutes = ((int) elapsed_seconds % 3600) / 60;
    seconds = (int)elapsed_seconds - (hours * 3600) - (minutes * 60);
    printf("Total elapsed time: %dh%02dm%02ds\n", hours, minutes, seconds);

    IOHprofiler_free_memory(timing_data->output);
    IOHprofiler_free_memory(timing_data);
  }
}
