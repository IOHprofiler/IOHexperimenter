/**
 * @file transform_vars_epistasis_xor.c
 * @brief Implementation of epistasis_xor all decision values by an offset.
 */

#include <assert.h>

#include "IOHprofiler.h"
#include "IOHprofiler_problem.c"

/**
 * @brief Data type for transform_vars_epistasis_xor.
 */
typedef struct {
  int *offset;
  int *offset1;
  int *epistasis_x;
  int *epistasis_xor_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_epistasis_xor_data_t;


static void epistasis_compute_beforexor(const int *x, int * epistasis_x,  int block_size, const size_t dimension){
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

static int epistasis_xor_compute(const int x1, const int x2){
    return (int) x1 != x2;
}


static void transform_vars_epistasis_xor_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  transform_vars_epistasis_xor_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (transform_vars_epistasis_xor_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);

  epistasis_compute_beforexor(x, data->epistasis_x, data->offset[0], problem->number_of_variables);
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->epistasis_xor_x[i] = epistasis_xor_compute(data->epistasis_x[i],data->offset1[i]);
  }

  IOHprofiler_evaluate_function(inner_problem, data->epistasis_xor_x, y);
  problem->raw_fitness[0] = y[0];

  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_epistasis_xor_free(void *thing) {
  transform_vars_epistasis_xor_data_t *data = (transform_vars_epistasis_xor_data_t *) thing;
  IOHprofiler_free_memory(data->epistasis_xor_x);
  IOHprofiler_free_memory(data->offset);
  IOHprofiler_free_memory(data->offset1);
  IOHprofiler_free_memory(data->epistasis_x);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_epistasis_xor(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int *offset1,
                                            const int epistasis_xor_bounds) {
    
  transform_vars_epistasis_xor_data_t *data;
  IOHprofiler_problem_t *problem;

  if (epistasis_xor_bounds)
    IOHprofiler_error("epistasis_xor_bounds not implemented.");

 
  data = (transform_vars_epistasis_xor_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, 1);
  data->offset1 = IOHprofiler_duplicate_int_vector(offset1, inner_problem->number_of_variables);
  data->epistasis_x = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);
  data->epistasis_xor_x = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);


  
  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_epistasis_xor_free, "transform_vars_epistasis_xor");
  problem->evaluate_function = transform_vars_epistasis_xor_evaluate;
  /* Compute best parameter */
  /*Best parameter will not change with this transformation*/

  return problem;
}
