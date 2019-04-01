/**
 * @file transform_obj_ruggedness1.c
 * @brief Implementation of ruggedness1ing the objective value by the given offset.
 */

#include <assert.h>

#include "IOHprofiler.h"
#include "IOHprofiler_problem.c"

/**
 * @brief Data type for transform_obj_ruggedness1.
 */
typedef struct {
  double offset;
} transform_obj_ruggedness1_data_t;

double compute_ruggedness(double y, size_t dimension){
  double ruggedness_y, s;
  s = (double)dimension;
  if(y == s){
    ruggedness_y = ceil(y/2.0) + 1.0;
  }
  else if(y < s && dimension % 2 == 0){
    ruggedness_y = floor(y / 2.0) + 1.0;
  }
  else if(y < s && dimension % 2 != 0){
    ruggedness_y = ceil(y / 2.0) + 1.0;
  }
  else{
    ruggedness_y = y;
    assert(y <= s);
  }
  
  return ruggedness_y;
}

/**
 * @brief Evaluates the transformation.
 */
static void transform_obj_ruggedness1_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
  	IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
  	return;
  }

  IOHprofiler_evaluate_function(IOHprofiler_problem_transformed_get_inner_problem(problem), x, y);
  for (i = 0; i < problem->number_of_objectives; i++) {
      y[i] = compute_ruggedness(y[i],problem->number_of_variables);
      problem->raw_fitness[i] = y[i];
      
  }
  assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_obj_ruggedness1(IOHprofiler_problem_t *inner_problem) {
  IOHprofiler_problem_t *problem;
  transform_obj_ruggedness1_data_t *data;
  size_t i;
  data = (transform_obj_ruggedness1_data_t *) IOHprofiler_allocate_memory(sizeof(*data));

  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, NULL, "transform_obj_ruggedness1");
  problem->evaluate_function = transform_obj_ruggedness1_evaluate;
  for (i = 0; i < problem->number_of_objectives; i++) {
      problem->best_value[i] = compute_ruggedness(problem->best_value[i],problem->number_of_variables); 
  }
  return problem;
}
