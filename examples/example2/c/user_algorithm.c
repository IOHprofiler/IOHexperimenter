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
static const size_t INDEPENDENT_RESTARTS = 10;

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

void generatingIndividual(int * individuals,
                            const size_t dimension, 
                            IOHProfiler_random_state_t *random_generator){
  size_t i;
  for(i = 0; i < dimension; ++i){
    individuals[i] = (int)(IOHProfiler_random_uniform(random_generator) * 2);
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
size_t randomBinomial(size_t n, double  probability,IOHProfiler_random_state_t *random_generator)
{
    size_t r, i;
    r = 0;
    for(i = 0; i < n; ++i){
        if(IOHProfiler_random_uniform(random_generator) < probability)
        {
            ++r;
        }
    }
    return r;
}

size_t mutateIndividual(int * individual, 
                      const size_t dimension, 
                      double mutation_rate, 
                      IOHProfiler_random_state_t *random_generator){
  size_t i,h, l;
  int flag,temp;
  int * flip;

  l = randomBinomial(dimension,mutation_rate,random_generator);
  while(l == 0){
    l = randomBinomial(dimension,mutation_rate,random_generator);
  }
  
  flip = IOHProfiler_allocate_int_vector(l);
  for(i = 0; i < l; ++i){
    while(1){
      flag = 0;
      temp = (int)(IOHProfiler_random_uniform(random_generator) * dimension);
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
  IOHProfiler_free_memory(flip);
  return l;
}


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

  int *parent = IOHProfiler_allocate_int_vector(dimension);
  int *offspring = IOHProfiler_allocate_int_vector(dimension);
  int *best = IOHProfiler_allocate_int_vector(dimension);
  double best_value;
  double *y = IOHProfiler_allocate_vector(number_of_objectives);
  size_t number_of_parameters = 3;
  double *p = IOHProfiler_allocate_vector(number_of_parameters);
  size_t i, j, l;
  int lambda = 1;
  double mutation_rate = 1/(double)dimension;
  l = 0;
  generatingIndividual(parent,dimension,random_generator);
  p[0] = mutation_rate; p[1] = (double)lambda; p[2] = (double)l;
  set_parameters(number_of_parameters,p);
  evaluate(parent,y);
  CopyIndividual(parent,best,dimension);
  best_value = y[0];

  for (i = 0; i < max_budget; ++i) {
    for(j = 0; j < lambda; ++j){
      CopyIndividual(parent,offspring,dimension);
      l = mutateIndividual(offspring,dimension,mutation_rate,random_generator);
      p[0] = mutation_rate; p[1] = (double)lambda; p[2] = (double)l;
      /* Call the evaluate function to evaluate x on the current problem (this is where all the IOHProfiler logging
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

  IOHProfiler_free_memory(parent);
  IOHProfiler_free_memory(offspring);
  IOHProfiler_free_memory(best);
  IOHProfiler_free_memory(p);
  IOHProfiler_free_memory(y);
}


