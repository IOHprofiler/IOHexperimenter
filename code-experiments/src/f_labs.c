/**
 * @Onemax function
 */

/**
 * @file f_labs.c
 * @brief Implementation of the labs function and problem.
 */

#include <assert.h>
#include <stdio.h>

#include "IOHprofiler.h"
#include "IOHprofiler_problem.c"
#include "suite_PBO_legacy_code.c"
#include "transform_obj_shift.c"
#include "transform_vars_sigma.c"
#include "transform_vars_xor.c"
#include "transform_vars_shift.c"
#include "transform_obj_scale.c"

/**
 * @brief Implements the labs function without connections to any IOHprofiler structures.
 */
static double correlation(const int * x, const size_t number_of_variables, size_t k)
{
    size_t i;
    double result;
    result = 0.0;
    for(i = 0 ; i < number_of_variables - k; ++i)
        result += x[i] * x[i + k];
    return result;
}

static double f_labs_raw(const int *x, const size_t number_of_variables) {
    size_t k = 0;
    double result,cor;

    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;

    result = 0.0;
    for (k = 1; k < number_of_variables; ++k){
        cor = correlation(x,number_of_variables,k);
        result += cor * cor;
    }

    result = (double)(number_of_variables * number_of_variables) / 2.0 / result;
    return result;
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static void f_labs_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_labs_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic labs problem.
 */
static IOHprofiler_problem_t *f_labs_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("labs function",
                                                                               f_labs_evaluate, NULL, number_of_variables, -1, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "labs", number_of_variables);

    /* Compute best solution */
    problem->best_value[0] = DBL_MAX;
    return problem;
}

/**
 * @brief Creates the IOHprofiler labs problem.
 */
static IOHprofiler_problem_t *f_labs_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {

  

    int *xopt;
    size_t i;
    double fopt;
    double *xins;
    IOHprofiler_problem_t *problem;
    xopt = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);
    problem = f_labs_allocate(dimension);;
    
    for(i = 0; i < dimension; i++)
        xopt[i] = 0;
    fopt = 0.0;
    problem = transform_vars_shift(problem,xopt,0);
    problem = transform_obj_shift(problem,fopt);
    
    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");

    IOHprofiler_free_memory(xopt);
    IOHprofiler_free_memory(xins);
    return problem;
}
