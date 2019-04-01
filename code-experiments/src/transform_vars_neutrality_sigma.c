/**
 * @file transform_vars_neutrality_sigma_sigma.c
 * @brief Implementation of neutrality_sigma all decision values by an offset.
 */

#include <assert.h>

#include "IOHprofiler.h"
#include "IOHprofiler_problem.c"

/**
 * @brief Data type for transform_vars_neutrality_sigma.
 */
typedef struct {
  int *offset;
  int *offset1;
  int *neutrality_x;
  int *sigma_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_neutrality_sigma_data_t;

static int neutrality_compute_before_sigma(const int *x, const int index, const int block_size){
  int number_of_one, number_of_zero, i;
  number_of_zero = 0;
  number_of_one = 0;
  i = 0;
  while(i < block_size){
    if(x[index + i] == 0){
      number_of_zero++;
    }
    if(x[index + i] == 1){
      number_of_one++;
    }
    ++i;
  }
  return (number_of_zero >= number_of_one ? 0 : 1);
}

static int neutrality_sigma_compute(const int *x, const int pos){
  return x[pos];
}

static void transform_vars_neutrality_sigma_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  int i;
  transform_vars_neutrality_sigma_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (transform_vars_neutrality_sigma_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);
  
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->neutrality_x[i] = neutrality_compute_before_sigma(x, i*data->offset[0], data->offset[0]);
  }
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->sigma_x[i] = neutrality_sigma_compute(data->neutrality_x,data->offset1[i]);
  }
  IOHprofiler_evaluate_function(inner_problem, data->sigma_x, y);
  problem->raw_fitness[0] = y[0];

  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_neutrality_sigma_free(void *thing) {
  transform_vars_neutrality_sigma_data_t *data = (transform_vars_neutrality_sigma_data_t *) thing;
  IOHprofiler_free_memory(data->neutrality_x);
  IOHprofiler_free_memory(data->offset);
  IOHprofiler_free_memory(data->offset1);
  IOHprofiler_free_memory(data->sigma_x);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_neutrality_sigma(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int *offset1,
                                            const int neutrality_sigma_bounds) {
    
  transform_vars_neutrality_sigma_data_t *data;
  IOHprofiler_problem_t *problem;
  int * temp_best;
  int i;
  size_t new_dimension;
  if (neutrality_sigma_bounds)
    IOHprofiler_error("neutrality_sigma_bounds not implemented.");

  if(inner_problem->number_of_variables >= offset[0])
  {
    new_dimension = inner_problem->number_of_variables / (size_t)offset[0];
  }
  else{
    return inner_problem;
  }
  
  assert(new_dimension > 0);
  data = (transform_vars_neutrality_sigma_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, 1);
  data->offset1 = IOHprofiler_duplicate_int_vector(offset1,new_dimension);
  data->neutrality_x = IOHprofiler_allocate_int_vector(new_dimension);
  data->sigma_x = IOHprofiler_allocate_int_vector(new_dimension);
  temp_best = IOHprofiler_allocate_int_vector(new_dimension);

  
  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_neutrality_sigma_free, "transform_vars_neutrality_sigma");
  problem->number_of_variables = new_dimension;
  inner_problem->number_of_variables = problem->number_of_variables;
  problem->evaluate_function = transform_vars_neutrality_sigma_evaluate;
  while(inner_problem->data != NULL){
    inner_problem = IOHprofiler_problem_transformed_get_inner_problem(inner_problem);
    inner_problem->number_of_variables = problem->number_of_variables;
  }
  /* Compute best parameter */
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = neutrality_compute_before_sigma(inner_problem->best_parameter, i*data->offset[0], data->offset[0]);
  }
  for (i = 0; i < problem->number_of_variables; i++) {
      temp_best[i] = neutrality_sigma_compute(problem->best_parameter,data->offset1[i]);
  }
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = temp_best[i];
  }
  IOHprofiler_evaluate_function(inner_problem, problem->best_parameter, problem->best_value);
  IOHprofiler_free_memory(temp_best);
  return problem;
}
