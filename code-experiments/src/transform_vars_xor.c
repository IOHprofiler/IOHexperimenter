/**
 * @file transform_vars_xor.c
 * @brief Implementation of xor all decision values by an offset.
 */

#include <assert.h>

#include "IOHprofiler.h"
#include "IOHprofiler_problem.c"

/**
 * @brief Data type for transform_vars_xor.
 */
typedef struct {
  int *offset;
  int *xor_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_xor_data_t;

/**
 * @brief Evaluates the transformation.
 */
static int xor_compute(const int x1, const int x2){
    return (int) x1 != x2;
}

static void transform_vars_xor_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  transform_vars_xor_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
  	IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
  	return;
  }

  data = (transform_vars_xor_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->xor_x[i] = xor_compute(x[i],data->offset[i]);
  }
  IOHprofiler_evaluate_function(inner_problem, data->xor_x, y);
  IOHprofiler_evaluate_function(inner_problem, data->xor_x, problem->raw_fitness);

  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_xor_free(void *thing) {
  transform_vars_xor_data_t *data = (transform_vars_xor_data_t *) thing;
  IOHprofiler_free_memory(data->xor_x);
  IOHprofiler_free_memory(data->offset);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_xor(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int xor_bounds) {
    
  transform_vars_xor_data_t *data;
  IOHprofiler_problem_t *problem;
  size_t i;
  if (xor_bounds)
    IOHprofiler_error("xor_bounds not implemented.");

  data = (transform_vars_xor_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, inner_problem->number_of_variables);
  data->xor_x = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);

  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_xor_free, "transform_vars_xor");
  problem->evaluate_function = transform_vars_xor_evaluate;
  /* Compute best parameter */
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = xor_compute(problem->best_parameter[i],data->offset[i]);
  }
  return problem;
}
