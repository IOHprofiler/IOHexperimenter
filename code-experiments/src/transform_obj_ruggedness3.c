/**
 * @file transform_obj_ruggedness3.c
 * @brief Implementation of ruggedness3ing the objective value by the given offset.
 */

#include <assert.h>

#include "IOHprofiler.h"
#include "IOHprofiler_problem.c"

/**
 * @brief Data type for transform_obj_ruggedness3.
 */
typedef struct {
  double offset;
  double *ruggedness_mapping;
} transform_obj_ruggedness3_data_t;

static void compute_ruggedness3(double * y, size_t dimension){
  size_t j,k;
  for(j = 1; j <= dimension/5; ++j){
    for(k = 0; k < 5; ++k){
      y[dimension-5*j+k] = (double)(dimension - 5 * j + (4-k));
    }
  }
  for(j = 0; j < dimension - dimension / 5 * 5; ++j){
    y[j] = dimension - dimension / 5 * 5 - j;
  }
  y[dimension] = (double)dimension;
}

/**
 * @brief Evaluates the transformation.
 */
static void transform_obj_ruggedness3_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  transform_obj_ruggedness3_data_t *data;
  size_t i;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }

  data = (transform_obj_ruggedness3_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  IOHprofiler_evaluate_function(IOHprofiler_problem_transformed_get_inner_problem(problem), x, y);
  for (i = 0; i < problem->number_of_objectives; i++) {
      problem->raw_fitness[i] = y[i];
      y[i] = data->ruggedness_mapping[(int)(y[i]+0.5)];
      
  }
  assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_obj_ruggedness3(IOHprofiler_problem_t *inner_problem, double offset) {
  IOHprofiler_problem_t *problem;
  transform_obj_ruggedness3_data_t *data;
  size_t i;
  data = (transform_obj_ruggedness3_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = offset;
  data->ruggedness_mapping = IOHprofiler_allocate_vector(inner_problem->number_of_variables+1);
  compute_ruggedness3(data->ruggedness_mapping,inner_problem->number_of_variables);
  
  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, NULL, "transform_obj_ruggedness3");
  problem->evaluate_function = transform_obj_ruggedness3_evaluate;

  for (i = 0; i < problem->number_of_objectives; i++) {
      problem->best_value[i] = (double)problem->number_of_variables;
  }
  return problem;
}
