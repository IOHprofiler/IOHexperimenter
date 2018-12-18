/**
 * @file transform_vars_reduction.c
 * @brief Implementation of reductiond all decision values by an offset.
 */

#include <assert.h>

#include "IOHprofiler.h"
#include "IOHprofiler_problem.c"

/**
 * @brief Data type for transform_vars_reduction.
 */
typedef struct {
  int *offset;
  int *reduncted_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_reduction_data_t;

static void transform_vars_reduction_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  transform_vars_reduction_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (transform_vars_reduction_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->reduncted_x[i] = x[i];
  }
  IOHprofiler_evaluate_function(inner_problem, data->reduncted_x, y);
  IOHprofiler_evaluate_function(inner_problem, data->reduncted_x, problem->raw_fitness);

  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_reduction_free(void *thing) {
  transform_vars_reduction_data_t *data = (transform_vars_reduction_data_t *) thing;
  IOHprofiler_free_memory(data->reduncted_x);
  IOHprofiler_free_memory(data->offset);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_reduction(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int reduction_bounds) {
    
  transform_vars_reduction_data_t *data;
  IOHprofiler_problem_t *problem;
  size_t i;
  size_t new_dimension;
  if (reduction_bounds)
    IOHprofiler_error("reduction_bounds not implemented.");

  new_dimension = offset[0];
  data = (transform_vars_reduction_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, inner_problem->number_of_variables);
  data->reduncted_x = IOHprofiler_allocate_int_vector(new_dimension);

  
  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_reduction_free, "transform_vars_reduction");
  problem->number_of_variables = new_dimension;
  inner_problem->number_of_variables = problem->number_of_variables;
  problem->evaluate_function = transform_vars_reduction_evaluate;
  /* Compute best parameter */
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = problem->best_parameter[i];
  }
  problem->evaluate_function(problem, problem->best_parameter, problem->best_value);
  return problem;
}
