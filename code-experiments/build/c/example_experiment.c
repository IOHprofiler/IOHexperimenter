/**
 * An example of benchmarking random search on a IOHprofiler suite. A one plus lambda evolutionary algorithm
 * optimizer is also implemented and can be used instead of random search.
 *
 * Set the global parameter BUDGET_MULTIPLIER to suit your needs.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "IOHprofiler.h"
/**
 * The maximal budget for evaluations done by an optimization algorithm equals dimension * BUDGET_MULTIPLIER.
 * Increase the budget multiplier value gradually to see how it affects the runtime.
 */
static const size_t BUDGET_MULTIPLIER = 50;

/**
 * The maximal number of independent restarts allowed for an algorithm that restarts itself.
 */
static const size_t INDEPENDENT_RESTARTS = 1;

/**
 * The random seed. Change if needed.
 */
static const uint32_t RANDOM_SEED = 0xdeadbeef;

/**
 * A function type for setting parameters functions, where the first argument is the number of parameters to be stored and the
 * second argument the vector of parameters. The type of parameters is double, please modify the type of other types of variables.
 */
typedef void (*set_parameters_t)(const size_t number_of_parameters, const double * parameters) ;


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
 * The function that set the parameters of the problem with the vector.
 */
static void set_parameters(const size_t number_of_parameters, const double * parameters) {
  IOHprofiler_problem_set_parameters(PROBLEM,number_of_parameters,parameters);
}

/* Declarations of all functions implemented in this file (so that their order is not important): */
void example_experiment(const char *suite_name,
                        const char *observer_name,
                        IOHprofiler_random_state_t *random_generator);
void my_random_search(evaluate_function_t evaluate,
                      const size_t dimension,
                      const size_t number_of_objectives,
                      const int *lower_bounds,
                      const int *upper_bounds,
                      const size_t max_budget,
                      IOHprofiler_random_state_t *random_generator);
void my_one_plus_lambda_EA(evaluate_function_t evaluate,
                      const size_t dimension,
                      const size_t number_of_objectives,
                      const int *lower_bounds,
                      const int *upper_bounds,
                      const size_t max_budget,
                      IOHprofiler_random_state_t *random_generator);


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

  IOHprofiler_random_state_t *random_generator = IOHprofiler_random_new((unsigned)time(NULL));

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
  example_experiment("PBO", "PBO", random_generator);

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
void example_experiment(const char *suite_name,
                        const char *observer_name,
                        IOHprofiler_random_state_t *random_generator) {

  size_t run;
  IOHprofiler_suite_t *suite;
  IOHprofiler_observer_t *observer;
  timing_data_t *timing_data;

  /* Set some options for the observer. See IOHprofiler_observer.c to find information of options */
  char *observer_options =
      IOHprofiler_strdupf("result_folder:  %s "
                   "algorithm_name: RS "
                   "complete_triggers: True "
                   "number_interval_triggers: 50"
                   "algorithm_info: \"A simple random search algorithm\""
                   "parameters_name: \"lambda mutation_rate\"", suite_name);

  /* Initialize the suite and observer.
   *
   * For IOHprofiler suite, find more information (function_id, dimensions) from suite_PBO.c*/
  suite = IOHprofiler_suite(suite_name, "instances: 1-100", "function_indices:1,2 dimensions:100");
  observer = IOHprofiler_observer(observer_name, observer_options);
  IOHprofiler_free_memory(observer_options);

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
      my_random_search(evaluate_function,
                       dimension,
                       IOHprofiler_problem_get_number_of_objectives(PROBLEM),
                       IOHprofiler_problem_get_smallest_values_of_interest(PROBLEM),
                       IOHprofiler_problem_get_largest_values_of_interest(PROBLEM),
                       (size_t) evaluations_remaining,
                       random_generator);

      /* Break the loop if the algorithm performed no evaluations or an unexpected thing happened */
      if (IOHprofiler_problem_get_evaluations(PROBLEM) == evaluations_done) {
        printf("WARNING: Budget has not been exhausted (%lu/%lu evaluations done)!\n",
            (unsigned long) evaluations_done, (unsigned long) dimension * BUDGET_MULTIPLIER);
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
 * A random search algorithm.
 *
 * @param evaluate The evaluation function used to evaluate the solutions.
 * @param dimension The number of variables.
 * @param number_of_objectives The number of objectives.
 * @param lower_bounds The lower bounds of the region of interested (a vector containing dimension values). 
 * Set as 0 for pseudo boolean problems.
 * @param upper_bounds The upper bounds of the region of interested (a vector containing dimension values). 
 * Set as 1 for pseudo boolean problems.
 * @param max_budget The maximal number of evaluations.
 * @param random_generator Pointer to a random number generator able to produce uniformly and normally
 * distributed random numbers.
 */

void my_random_search(evaluate_function_t evaluate,
                      const size_t dimension,
                      const size_t number_of_objectives,
                      const int *lower_bounds,
                      const int *upper_bounds,
                      const size_t max_budget,
                      IOHprofiler_random_state_t *random_generator) {

  size_t number_of_parameters = 1;
  int *x = IOHprofiler_allocate_int_vector(dimension);
  double *y = IOHprofiler_allocate_vector(number_of_objectives);
  double *p = IOHprofiler_allocate_vector(number_of_parameters);
  size_t i, j;

  for (i = 0; i < max_budget; ++i) {
    for (j = 0; j < dimension; ++j) {

    /* Construct x as a random point between the lower (0) and upper (1) bounds */
      x[j] = (int)(IOHprofiler_random_uniform(random_generator) * 2) ;
    }
    p[0] = (double)i + 1.0;
    /* Call the evaluate function to evaluate x on the current problem (this is where all the IOHprofiler logging
     * is performed) */
    set_parameters(number_of_parameters,p);
    evaluate(x, y);

  }

  IOHprofiler_free_memory(x);
  IOHprofiler_free_memory(y);
}

void generatingIndividual(int * individuals,
                            const size_t dimension, 
                            IOHprofiler_random_state_t *random_generator){
  size_t i;
  for(i = 0; i < dimension; ++i){
    individuals[i] = (int)(IOHprofiler_random_uniform(random_generator) * 2);
  }
}

void CopyIndividual(int * old, int * new, const size_t dimension){
  size_t i;
  for(i = 0; i < dimension; ++i){
    new[i] = old[i];
  }
}

/**
 * Binomial
 */
size_t randomBinomial(size_t n, double  probability,IOHprofiler_random_state_t *random_generator)
{
    size_t r, i;
    r = 0;
    for(i = 0; i < n; ++i){
        if(IOHprofiler_random_uniform(random_generator) < probability)
        {
            ++r;
        }
    }
    return r;
}

size_t mutateIndividual(int * individual, 
                      const size_t dimension, 
                      double mutation_rate, 
                      IOHprofiler_random_state_t *random_generator){
  size_t i,h, l;
  int flag,temp;
  int * flip;

  l = randomBinomial(dimension,mutation_rate,random_generator);
  while(l == 0){
    l = randomBinomial(dimension,mutation_rate,random_generator);
  }
  
  flip = IOHprofiler_allocate_int_vector(l);
  for(i = 0; i < l; ++i){
    while(1){
      flag = 0;
      temp = (int)(IOHprofiler_random_uniform(random_generator) * dimension);
      for(h = 0; h < i; ++h)
      {
        if(temp == flip[h]){
          flag = 1;
          break;
        }
      }
      if(flag == 0)
        break;
    }
    flip[i] = temp;
  }

  for(i = 0; i < l; ++i){
    individual[flip[i]] =  ((int)(individual[flip[i]] + 1) % 2);
  }
  IOHprofiler_free_memory(flip);
  return l;
}
/**
 * A one plus lambda evolutionary algorithm.
 *
 * @param evaluate The evaluation function used to evaluate the solutions.
 * @param dimension The number of variables.
 * @param number_of_objectives The number of objectives.
 * @param lower_bounds The lower bounds of the region of interested (a vector containing dimension values). 
 * Set as 0 for pseudo boolean problems.
 * @param upper_bounds The upper bounds of the region of interested (a vector containing dimension values). 
 * Set as 1 for pseudo boolean problems.
 * @param max_budget The maximal number of evaluations.
 * @param random_generator Pointer to a random number generator able to produce uniformly and normally
 * distributed random numbers.
 */

void my_one_plus_lambda_EA(evaluate_function_t evaluate,
                      const size_t dimension,
                      const size_t number_of_objectives,
                      const int *lower_bounds,
                      const int *upper_bounds,
                      const size_t max_budget,
                      IOHprofiler_random_state_t *random_generator) {

  int *parent = IOHprofiler_allocate_int_vector(dimension);
  int *offspring = IOHprofiler_allocate_int_vector(dimension);
  int *best = IOHprofiler_allocate_int_vector(dimension);
  double best_value;
  double *y = IOHprofiler_allocate_vector(number_of_objectives);
  size_t number_of_parameters = 3;
  double *p = IOHprofiler_allocate_vector(number_of_parameters);
  size_t i, j, l;
  int lambda = 1;
  double mutation_rate = 1/(double)dimension;
  l = 0;
  generatingIndividual(parent,dimension,random_generator);
  p[0] = 1.0; p[1] = (double)lambda; p[2] = (double)l;
  set_parameters(number_of_parameters,p);
  evaluate(parent,y);
  CopyIndividual(parent,best,dimension);
  best_value = y[0];

  for (i = 0; i < max_budget; ++i) {
    for(j = 0; j < lambda; ++j){
      CopyIndividual(parent,offspring,dimension);
      l = mutateIndividual(offspring,dimension,mutation_rate,random_generator);
      p[0] = 1.0; p[1] = (double)lambda; p[2] = (double)l;
      /* Call the evaluate function to evaluate x on the current problem (this is where all the IOHprofiler logging
       * is performed) */
      set_parameters(number_of_parameters,p);
      evaluate(offspring, y);
      if(y[0] > best_value){
        best_value = y[0];
        CopyIndividual(offspring,best,dimension);
      }
    }
    CopyIndividual(best,parent,dimension);
  }

  IOHprofiler_free_memory(parent);
  IOHprofiler_free_memory(offspring);
  IOHprofiler_free_memory(best);
  IOHprofiler_free_memory(p);
  IOHprofiler_free_memory(y);
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
