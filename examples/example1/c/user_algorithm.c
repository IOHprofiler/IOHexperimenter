/** 
  implement your algorithm in this file.
 **/


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
 * The random seed. Change it if needed.
 */
static const uint32_t RANDOM_SEED = 0xdeadbeef;

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
                      IOHProfiler_random_state_t *random_generator) {

  /**
   * Add your algorithm in this function. You can invoke other self-defined functions,
   * but please remember this is the interface for IOHProfiler. Make sure your main
   * algorithm be inclueded in this function.
   *
   * The data of varibales and fitness will be stored once "evaluate()" works.
   *
   * If you want to store information of some self-defined parameters, use the statement
   * "set_parameters(size_t number_of_parameters,double *parameters)". The name of parameters
   * can be set in "config" file.
   */

  size_t number_of_parameters = 1;
  int *x = IOHProfiler_allocate_int_vector(dimension);
  double *y = IOHProfiler_allocate_vector(number_of_objectives);
  double *p = IOHProfiler_allocate_vector(number_of_parameters);
  size_t i, j;

  for (i = 0; i < max_budget; ++i) {
    for (j = 0; j < dimension; ++j) {

    /* Construct x as a random point between the lower (0) and upper (1) bounds */
      x[j] = (int)(IOHProfiler_random_uniform(random_generator) * 2);
    }
    p[0] = i + 1;
    /* Call the evaluate function to evaluate x on the current problem (this is where all the IOHProfiler logging
     * is performed) */
    set_parameters(number_of_parameters,p);
    evaluate(x, y);

  }

  IOHProfiler_free_memory(x);
  IOHProfiler_free_memory(y);
}


