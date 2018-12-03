/**
 * This file implements a random search algorithm.
 */


/**
 * The maximal budget for evaluations done by an optimization algorithm equals dimension * BUDGET_MULTIPLIER.
 * Increase the budget multiplier value gradually to see how it affects the runtime.
 */
static const size_t BUDGET_MULTIPLIER = 5;

/**
 * The maximal number of independent restarts allowed for an algorithm that restarts itself.
 */
static const size_t INDEPENDENT_RESTARTS = 1;

/**
 * The random seed. Change it if needed.
 */
static const uint32_t RANDOM_SEED = 1;

/**
 * An user defined algorithm.
 *
 * @param "evaluate" The function for evaluating variables' fitness. Invoking the 
 *        statement "evaluate(x,y)", then the fitness of 'x' will be stored in 'y[0]'.
 * @param "dimension" The dimension of problem.
 * @param "number_of_objectives" The number of objectives. The default is 1.
 * @param "lower_bounds" The lower bounds of the region of interested (a vector containing dimension values). 
 * @param "upper_bounds" The upper bounds of the region of interested (a vector containing dimension values). 
 * @param "max_budget" The maximal number of evaluations. You can set it by BUDGET_MULTIPLIER in "config" file.
 * @param "random_generator" Pointer to a random number generator able to produce uniformly and normally
 * distributed random numbers. You can set it by RANDOM_SEED in "config" file
 */

void User_Algorithm(evaluate_function_t evaluate,
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
      x[j] = (int)(IOHprofiler_random_uniform(random_generator) * 2);
    }
    p[0] = i + 1;
    /* Call the evaluate function to evaluate x on the current problem (this is where all the IOHprofiler logging
     * is performed) */
    set_parameters(number_of_parameters,p);
    evaluate(x, y);

  }

  IOHprofiler_free_memory(x);
  IOHprofiler_free_memory(y);
}