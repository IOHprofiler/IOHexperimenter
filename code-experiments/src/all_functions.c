#include "IOHProfiler.h"

#include "f_binary.c"
#include "f_jump.c"
#include "f_leading_ones.c"
#include "f_linear.c"
#include "f_one_max.c"


enum IOHProblem {one_max=1, leading_ones=2, jump=3, linear=4, binary=5}



static IOHProfiler_problem_t *get_problem(const size_t function,
                                              const size_t dimension,
                                              const size_t instance) {
  IOHProfiler_problem_t *problem = NULL;

  const char *problem_id_template = "PBO_f%03lu_i%02lu_d%02lu";
  const char *problem_name_template = "PBO suite problem f%lu instance %lu in %luD";

  const long rseed = (long) (function + 10000 * instance);

  if (function == one_max) {
    problem = f_one_max_IOHProfiler_problem_allocate(function, dimension, instance, rseed,
        problem_id_template, problem_name_template);
  } else if (function == leading_ones) {
    problem = f_leading_ones_IOHProfiler_problem_allocate(function, dimension, instance, rseed,
        problem_id_template, problem_name_template);
  } else if (function == jump) {
    problem = f_jump_IOHProfiler_problem_allocate(function, dimension, instance, rseed,
        problem_id_template, problem_name_template);
  } else if (function == linear) {
    problem = f_linear_IOHProfiler_problem_allocate(function, dimension, instance, rseed,
        problem_id_template, problem_name_template);
  } else if (function == binary) {
    problem = f_binary_IOHProfiler_problem_allocate(function, dimension, instance, rseed,
        problem_id_template, problem_name_template);
  } else {
    IOHProfiler_error("get_IOHProfiler_problem(): cannot retrieve problem f%lu instance %lu in %luD",
    		(unsigned long) function, (unsigned long) instance, (unsigned long) dimension);
    return NULL; /* Never reached */
  }

  return problem;
}


// Example usage:
//
// #include "all_functions.c"
// IOHProfiler_problem_t* problem = get_problem(one_max);
