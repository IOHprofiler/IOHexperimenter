/**
 * @file transform_obj_shift.c
 * @brief Implementation of shifting the objective value by the given offset.
 */

#include <assert.h>

#include "IOHprofiler.h"
#include "IOHprofiler_problem.c"

/**
 * @brief Data type for transform_obj_shift.
 */
typedef struct {
  double offset;
} transform_obj_shift_data_t;

/**
 * @brief Evaluates the transformation.
 */
static void transform_obj_shift_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  transform_obj_shift_data_t *data;
  size_t i;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
  	IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
  	return;
  }

  data = (transform_obj_shift_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  IOHprofiler_evaluate_function(IOHprofiler_problem_transformed_get_inner_problem(problem), x, y);
  
  for (i = 0; i < problem->number_of_objectives; i++) {
      y[i] += data->offset;
      problem->raw_fitness[i] = IOHprofiler_problem_transformed_get_inner_problem(problem)->raw_fitness[i];
  }

  assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_obj_shift(IOHprofiler_problem_t *inner_problem, const double offset) {
  IOHprofiler_problem_t *problem;
  transform_obj_shift_data_t *data;
  size_t i;
  data = (transform_obj_shift_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = offset;

  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, NULL, "transform_obj_shift");
  problem->evaluate_function = transform_obj_shift_evaluate;
  for (i = 0; i < problem->number_of_objectives; i++) {
      problem->best_value[0] += offset;
  }
  return problem;
}
