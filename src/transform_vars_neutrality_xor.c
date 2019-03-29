/**
 * @file transform_vars_neutrality_xor.c
 * @brief Implementation of neutrality_xor all decision values by an offset.
 */

#include <assert.h>

#include "IOHprofiler.h"
#include "IOHprofiler_problem.c"

/**
 * @brief Data type for transform_vars_neutrality_xor.
 */
typedef struct {
  int *offset;
  int *offset1;
  int *neutrality_x;
  int *xor_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_neutrality_xor_data_t;

static int neutrality_compute_before_xor(const int *x, const int index, const int block_size){
  size_t number_of_one, number_of_zero, i;
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

static int neutrality_xor_compute(const int x1, const int x2){
    return (int) x1 != x2;
}

static void transform_vars_neutrality_xor_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  transform_vars_neutrality_xor_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (transform_vars_neutrality_xor_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);
  
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->neutrality_x[i] = neutrality_compute_before_xor(x, i*data->offset[0], data->offset[0]);
  }
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->xor_x[i] = neutrality_xor_compute(data->neutrality_x[i],data->offset1[i]);
  }
  IOHprofiler_evaluate_function(inner_problem, data->xor_x, y);
  problem->raw_fitness[0] = y[0];
  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_neutrality_xor_free(void *thing) {
  transform_vars_neutrality_xor_data_t *data = (transform_vars_neutrality_xor_data_t *) thing;
  IOHprofiler_free_memory(data->neutrality_x);
  IOHprofiler_free_memory(data->offset);
  IOHprofiler_free_memory(data->offset1);
  IOHprofiler_free_memory(data->xor_x);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_neutrality_xor(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int *offset1,
                                            const int neutrality_xor_bounds) {
    
  transform_vars_neutrality_xor_data_t *data;
  IOHprofiler_problem_t *problem;
  size_t i;
  size_t new_dimension;
  if (neutrality_xor_bounds)
    IOHprofiler_error("neutrality_xor_bounds not implemented.");

  if(inner_problem->number_of_variables >= offset[0])
  {
    new_dimension = inner_problem->number_of_variables / offset[0];
  }
  else{
    return inner_problem;
  }
  
  assert(new_dimension > 0);
  data = (transform_vars_neutrality_xor_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, 1);
  data->offset1 = IOHprofiler_duplicate_int_vector(offset1,new_dimension);
  data->neutrality_x = IOHprofiler_allocate_int_vector(new_dimension);
  data->xor_x = IOHprofiler_allocate_int_vector(new_dimension);

  
  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_neutrality_xor_free, "transform_vars_neutrality_xor");
  problem->number_of_variables = new_dimension;
  inner_problem->number_of_variables = problem->number_of_variables;
  problem->evaluate_function = transform_vars_neutrality_xor_evaluate;
  while(inner_problem->data != NULL){
    inner_problem = IOHprofiler_problem_transformed_get_inner_problem(inner_problem);
    inner_problem->number_of_variables = problem->number_of_variables;
  }
  /* Compute best parameter */
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = neutrality_compute_before_xor(inner_problem->best_parameter, i*data->offset[0], data->offset[0]);
  }

  IOHprofiler_evaluate_function(inner_problem, problem->best_parameter, problem->best_value);
  return problem;
}
