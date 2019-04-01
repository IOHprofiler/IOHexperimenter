/**
 * @file transform_vars_epistasis_sigma.c
 * @brief Implementation of epistasis_sigma all decision values by an offset.
 */

#include <assert.h>

#include "IOHprofiler.h"
#include "IOHprofiler_problem.c"

/**
 * @brief Data type for transform_vars_epistasis_sigma.
 */
typedef struct {
  int *offset;
  int *offset1;
  int *epistasis_x;
  int *epistasis_sigma_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_epistasis_sigma_data_t;


static void epistasis_compute_beforesigma(const int *x, int * epistasis_x,  int block_size, const size_t dimension){
  int i,j,h,epistasis_result;

  h = 0;
  while(h + block_size - 1 < dimension){
    i = 0;
    while(i < block_size){
      epistasis_result = -1;
      for(j = 0; j < block_size; ++j){
        if( (block_size - j - 1) != ((block_size - i - 1) - 1) % 4){
          if(epistasis_result == -1){
            epistasis_result = x[h+j];
          }
          else{
            epistasis_result = (epistasis_result != x[h+j]);
          }
        }
      }
      epistasis_x[h+i] = epistasis_result;
      ++i;
    }
    h += block_size;
  }
  if((int)dimension - h > 0){
    block_size = (int)dimension - h;
    i = 0;
    while(i < block_size){
      epistasis_result = -1;
      for(j = 0; j < block_size; ++j){
        if( (block_size - j - 1) != ((block_size - i - 1) - 1) % 4){
          if(epistasis_result == -1){
            epistasis_result = x[h+j];
          }
          else{
            epistasis_result = (epistasis_result != x[h+j]);
          }
        }
      }
      epistasis_x[h+i] = epistasis_result;
      ++i;
    }
  }
}

static int epistasis_sigma_compute(const int *x, const int pos){
  return x[pos];
}


static void transform_vars_epistasis_sigma_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  transform_vars_epistasis_sigma_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (transform_vars_epistasis_sigma_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);

  epistasis_compute_beforesigma(x, data->epistasis_x, data->offset[0], problem->number_of_variables);
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->epistasis_sigma_x[i] = epistasis_sigma_compute(data->epistasis_x,data->offset1[i]);
  }

  IOHprofiler_evaluate_function(inner_problem, data->epistasis_sigma_x, y);
  problem->raw_fitness[0] = y[0];

  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_epistasis_sigma_free(void *thing) {
  transform_vars_epistasis_sigma_data_t *data = (transform_vars_epistasis_sigma_data_t *) thing;
  IOHprofiler_free_memory(data->epistasis_sigma_x);
  IOHprofiler_free_memory(data->offset);
  IOHprofiler_free_memory(data->offset1);
  IOHprofiler_free_memory(data->epistasis_x);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_epistasis_sigma(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int *offset1,
                                            const int epistasis_sigma_bounds) {
    
  transform_vars_epistasis_sigma_data_t *data;
  IOHprofiler_problem_t *problem;
  
  if (epistasis_sigma_bounds)
    IOHprofiler_error("epistasis_sigma_bounds not implemented.");

 
  data = (transform_vars_epistasis_sigma_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, 1);
  data->offset1 = IOHprofiler_duplicate_int_vector(offset1, inner_problem->number_of_variables);
  data->epistasis_x = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);
  data->epistasis_sigma_x = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);


  
  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_epistasis_sigma_free, "transform_vars_epistasis_sigma");
  problem->evaluate_function = transform_vars_epistasis_sigma_evaluate;
  /* Compute best parameter */
  /*Best parameter will not change with this transformation*/

  return problem;
}
