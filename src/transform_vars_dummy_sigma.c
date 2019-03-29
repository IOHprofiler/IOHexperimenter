/**
 * @file transform_vars_dummy.c
 * @brief Implementation of dummyd all decision values by an offset.
 */

#include <assert.h>

#include "IOHprofiler.h"
#include "IOHprofiler_problem.c"

/**
 * @brief Data type for transform_vars_dummy.
 */
typedef struct {
  int *offset;
  int *reduncted_x;
  int *postion_match;
  int *sigma_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_dummy_sigma_data_t;

static void compute_dummy_sigma_match(int * postion_match, const int old_dimension, const int new_dimension){
  size_t i,j;
  int temp;
  double *randN = IOHprofiler_allocate_vector(1);
  int flag;
  int dummyins = 10000;
  for(i = 0; i < new_dimension;){
    IOHprofiler_unif(randN, 1, dummyins++);
    temp = (int)(randN[0] * old_dimension);
    flag = 1;
    for(j = 0; j < i; ++j){
      if(postion_match[j] == temp){
        flag = 0;
        break;
      }
    }
    if(flag == 1){
      postion_match[i] = temp;
      ++i;
    }
    else{
      continue;
    }
  }
  IOHprofiler_free_memory(randN);
}

static int dummy_sigma_compute(const int *x, const int pos){
  return x[pos];
}


static void transform_vars_dummy_sigma_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  transform_vars_dummy_sigma_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (transform_vars_dummy_sigma_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->reduncted_x[i] = x[data->postion_match[i]];
  }
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->sigma_x[i] = dummy_sigma_compute(data->reduncted_x,data->offset[i]);
  }
  IOHprofiler_evaluate_function(inner_problem, data->sigma_x, y);
  problem->raw_fitness[0] = y[0];

  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_dummy_sigma_free(void *thing) {
  transform_vars_dummy_sigma_data_t *data = (transform_vars_dummy_sigma_data_t *) thing;
  IOHprofiler_free_memory(data->reduncted_x);
  IOHprofiler_free_memory(data->postion_match);
  IOHprofiler_free_memory(data->sigma_x);
  IOHprofiler_free_memory(data->offset);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_dummy_sigma(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int *offset1,
                                            const int dummy_bounds) {
    
  transform_vars_dummy_sigma_data_t *data;
  IOHprofiler_problem_t *problem;
  int * temp_best;
  size_t i;
  size_t new_dimension;
  if (dummy_bounds)
    IOHprofiler_error("dummy_bounds not implemented.");

  new_dimension = offset[0];
  temp_best = IOHprofiler_allocate_int_vector(new_dimension);
  data = (transform_vars_dummy_sigma_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset1, new_dimension);
  data->reduncted_x = IOHprofiler_allocate_int_vector(new_dimension);
  data->postion_match = IOHprofiler_allocate_int_vector(new_dimension);
  data->sigma_x = IOHprofiler_allocate_int_vector(new_dimension);
  compute_dummy_sigma_match(data->postion_match,inner_problem->number_of_variables,new_dimension);
  
  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_dummy_sigma_free, "transform_vars_dummy_sigma");
  problem->number_of_variables = new_dimension;
  inner_problem->number_of_variables = problem->number_of_variables;
  
  while(inner_problem->data != NULL){
    inner_problem = IOHprofiler_problem_transformed_get_inner_problem(inner_problem);
    inner_problem->number_of_variables = problem->number_of_variables;
  }

  problem->evaluate_function = transform_vars_dummy_sigma_evaluate;
  /* Compute best parameter */
  for (i = 0; i < problem->number_of_variables; i++) {
      temp_best[i] = problem->best_parameter[data->postion_match[i]];
  }
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = temp_best[i];
  }
  for (i = 0; i < problem->number_of_variables; i++) {
      temp_best[i] = dummy_sigma_compute(problem->best_parameter,data->offset[i]);
  }
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = temp_best[i];
  }
  problem->evaluate_function(problem, problem->best_parameter, problem->best_value);
  IOHprofiler_free_memory(temp_best);
  return problem;
}
