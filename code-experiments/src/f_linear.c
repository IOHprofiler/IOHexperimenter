/**
 * @Linear functions
 */

/**
 * @file f_liner.c
 * @brief Implementation of the linear function and problem.
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "IOHprofiler.h"
#include "IOHprofiler_problem.c"
#include "suite_PBO_legacy_code.c"
#include "transform_obj_shift.c"
#include "transform_vars_xor.c"
#include "transform_vars_sigma.c"
#include "transform_obj_scale.c"

/**
 * @brief Implements the linear function without connections to any IOHprofiler structures.
 */

long f_linear_ins;

static double f_linear_raw(const int *x, const size_t number_of_variables) {
    size_t i = 0;
    double result;
    double *weight;
    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;

    weight = IOHprofiler_allocate_vector(number_of_variables);
    IOHprofiler_unif(weight, number_of_variables, f_linear_ins * 10000);

    result = 0.0;
    for (i = 0; i < number_of_variables; ++i) {
        result += (double)x[i] * (double)(weight[i] * 5);
        assert(weight[i] * 5 > 0 && weight[i] * 5 < 5);
    }

    IOHprofiler_free_memory(weight);
    return result;
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static void f_linear_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_linear_raw(x, problem->number_of_variables);
    problem->raw_fitness[0] = y[0];
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic Linear problem.
 */
static IOHprofiler_problem_t *f_linear_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("Linear function",
                                                                               f_linear_evaluate, NULL, number_of_variables, 0, 1, 1);
    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "linear", number_of_variables);

    /* Compute best solution */
    f_linear_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler linearproblem.
 */
static IOHprofiler_problem_t *f_linear_IOHprofiler_problem_allocate(const size_t function,
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
  IOHprofiler_problem_t *problem;
  xopt = IOHprofiler_allocate_int_vector(dimension);
  problem =  f_linear_allocate(dimension);
  f_linear_ins = 1;

  if(instance == 1){
    for(i = 0; i < dimension; i++)
      xopt[i] = 0;
    fopt = 0.0;
    problem = transform_vars_xor(problem,xopt,0);
    problem = transform_obj_shift(problem,fopt);
  }
  else if(instance > 1 && instance <= 100){
    fopt = IOHprofiler_compute_fopt(function,instance);
    fopt1 = IOHprofiler_compute_fopt(function,instance + 100);
    fopt1 = fabs(fopt1) / 1000 * 4.8 + 0.2;
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
  return problem;
}
