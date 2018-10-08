/**
 * @Leading Ones
 */

/**
 * @file f_leading_ones.c
 * @brief Implementation of the leading ones function and problem.
 */

#include <assert.h>
#include <math.h>

#include "IOHprofiler.h"
#include "IOHprofiler_problem.c"
#include "suite_PBO_legacy_code.c"
#include "transform_obj_shift.c"
#include "transform_vars_xor.c"
#include "transform_vars_sigma.c"
#include "transform_obj_scale.c"


/**
 * @brief Implements the leading ones function without connections to any IOHprofiler structures.
 */
static double f_leading_ones_raw(const int *x, const size_t number_of_variables) {
    size_t i;
    double result;

    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;

    result = 0.0;
    for (i = 0; i < number_of_variables; ++i) {
        if (x[i] == 1.0) {
            result = (double)(i + 1);
        } else {
            break;
        }
    }
    return result;
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static void f_leading_ones_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_leading_ones_raw(x, problem->number_of_variables);
    problem->raw_fitness[0] = y[0];
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic leading ones problem.
 */
static IOHprofiler_problem_t *f_leading_ones_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("Leading Ones function",
                                                                               f_leading_ones_evaluate, NULL, number_of_variables, 0, 1, 1);
    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "leading_ones", number_of_variables);

    /* Compute best solution */
    f_leading_ones_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler leading ones problem.
 */
static IOHprofiler_problem_t *f_leading_ones_IOHprofiler_problem_allocate(const size_t function,
                                                                          const size_t dimension,
                                                                          const size_t instance,
                                                                          const long rseed,
                                                                          const char *problem_id_template,
                                                                          const char *problem_name_template) {
    int *xopt;
    int temp,t;
    size_t i;
    double fopt;
    double fopt1;
    double *xins;
    IOHprofiler_problem_t *problem;
    xopt = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);
    problem = f_leading_ones_allocate(dimension);
  



  if(instance == 1){
    for(i = 0; i < dimension; i++)
      xopt[i] = 0;
    fopt = 0.0;
    problem = transform_vars_xor(problem,xopt,0);
    problem = transform_obj_shift(problem,fopt);
  }
  else if(instance > 1 && instance <= 50){
    IOHprofiler_compute_xopt(xopt,rseed,dimension);
    fopt = IOHprofiler_compute_fopt(function,instance);
    fopt1 = IOHprofiler_compute_fopt(function,instance + 100);
    fopt1 = fabs(fopt1) / 1000 * 4.8 + 0.2;
    problem = transform_vars_xor(problem,xopt,0);
    assert(fopt1 <= 5.0 && fopt1 >= 0.2);
    problem = transform_obj_scale(problem,fopt1);
    problem = transform_obj_shift(problem,fopt);
  }
  else if(instance <= 100 && instance > 50)
  {
    IOHprofiler_compute_xopt_double(xins,rseed,dimension);
    for(i = 0; i < dimension; i++)
      xopt[i] = (int)i;
    for(i = 0; i < dimension; i++){
      t = (int)(xins[i] * (double)dimension);
      assert(t >= 0 && t < dimension);
      temp = xopt[0];
      xopt[0] = xopt[t];
      xopt[t] = temp; 
    }
    fopt = IOHprofiler_compute_fopt(function,instance);
    fopt1 = IOHprofiler_compute_fopt(function,instance + 100);
    fopt1 = fabs(fopt1) / 1000 * 4.8 + 0.2;
    problem = transform_vars_sigma(problem,xopt,0);
    assert(fopt1 <= 5.0 && fopt1 >= 0.2);
    problem = transform_obj_scale(problem,fopt1);
    problem = transform_obj_shift(problem,fopt);
  }else{
    for(i = 0; i < dimension; i++)
      xopt[i] = 0;
    fopt = 0.0;
    problem = transform_vars_sigma(problem,xopt,0);
    problem = transform_obj_shift(problem,fopt);
  }

  
  IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
  IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
  IOHprofiler_problem_set_type(problem, "pseudo-Boolean");
  
  IOHprofiler_free_memory(xopt);
  IOHprofiler_free_memory(xins);
  return problem;
}
