/** 
 * This file implements an evolutionary algorithm with normalized standard bit mutation
 * Initialized mutation rate is set as 0.5.
 * Size of parents: 1
 * Size of offsprings: 10
 **/


/**
 * The maximal budget for evaluations done by an optimization algorithm equals dimension * BUDGET_MULTIPLIER.
 * Increase the budget multiplier value gradually to see how it affects the runtime.
 */
static const size_t BUDGET_MULTIPLIER = 5;

/**
 * The maximal number of independent restarts allowed for an algorithm that restarts itself.
 */
static const size_t INDEPENDENT_RESTARTS = 11;

/**
 * The random seed. Change it if needed.
 */
static const uint32_t RANDOM_SEED = 1;

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

size_t mutateIndividual_base_l(int * individual, 
                      const size_t dimension, 
                      int l, 
                      IOHprofiler_random_state_t *random_generator,
                      int count){
      size_t i,h;
  int flag,temp,templ;
  int * flip;
  double rands,c;
  double mr;

  mr = (double)l/(double)dimension;
  
   templ = l;
  l = templ + (int)(IOHprofiler_random_normal(random_generator) * sqrt(dimension * mr * (1-mr) ));
  while(l <= 0 || l > dimension){
    l = templ + (int)(IOHprofiler_random_normal(random_generator) * sqrt(dimension * mr * (1-mr) ));
  }


  if(l <= 0)
    return 0;
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

  /**
   * Add your algorithm in this function. You can invoke other self-defined functions,
   * but please remember this is the interface for IOHprofiler. Make sure your main
   * algorithm be inclueded in this function.
   *
   * The data of varibales and fitness will be stored once "evaluate()" works.
   *
   * If you want to store information of some self-defined parameters, use the statement
   * "set_parameters(size_t number_of_parameters,double *parameters)". The name of parameters
   * can be set in "config" file.
   */

  int *parent = IOHprofiler_allocate_int_vector(dimension);
  int *offspring = IOHprofiler_allocate_int_vector(dimension);
  int *best = IOHprofiler_allocate_int_vector(dimension);
  double best_value;
  double *y = IOHprofiler_allocate_vector(number_of_objectives);
  size_t number_of_parameters = 3;
  double *p = IOHprofiler_allocate_vector(number_of_parameters);
  size_t i, j, l,bestl,tempbl;
  double tempb;
  int hit_optimal = 0;
  int lambda = 10;
  double mutation_rate = 0.5;
  size_t last_bestl;
  int count = 0;
  l = 0;


  /*Initilization*/
  generatingIndividual(parent,dimension,random_generator);
  p[0] = (double)lambda; p[1] = 0.0; p[2] = 0.0; 
  set_parameters(number_of_parameters,p);
  evaluate(parent,y);
  CopyIndividual(parent,best,dimension);
  best_value = y[0];

  bestl = 1;
  last_bestl = 1;
  /*Iterations for the evolutioanry algorithm*/
  for (i = 1; i < max_budget; ) {
    tempb = -DBL_MAX;
    tempbl = bestl;
    for(j = 0; j < lambda; ++j){

      CopyIndividual(parent,offspring,dimension);
      mutation_rate = bestl / (double)dimension;
      l = mutateIndividual_base_l(offspring,dimension,bestl,random_generator,count);
      if(l == 0)
        continue;      
      if(l != 0){
        p[0] = (double)lambda; p[1] = mutation_rate; p[2] = (double)l;
        /* Call the evaluate function to evaluate x on the current problem (this is where all the IOHprofiler logging
         * is performed) */
        set_parameters(number_of_parameters,p);
        evaluate(offspring, y);
        ++i;
      }
      if(i == max_budget) break;
      if(if_hit_optimal()) {
        hit_optimal = 1;
        break;
      }

      if(y[0] > tempb && l != 0){
        tempb = y[0];
        tempbl = l;
      }
      if(y[0] >= best_value){
        best_value = y[0];
        CopyIndividual(offspring,best,dimension);
      }
    }
    if(hit_optimal) break;   
    /*Self Adaptation procedure*/
    bestl = tempbl;
    if(i > lambda && bestl == last_bestl){
        count++;
    }
    else{
      count = 0;
      last_bestl = bestl;
    }
    CopyIndividual(best,parent,dimension);
  }

  IOHprofiler_free_memory(parent);
  IOHprofiler_free_memory(offspring);
  IOHprofiler_free_memory(best);
  IOHprofiler_free_memory(p);
  IOHprofiler_free_memory(y);
}

