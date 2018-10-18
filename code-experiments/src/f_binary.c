/*
 * @Binary Funtion
 */

/**
 * @file f_binary.c
 * @brief Implementation of the binary function and problem.
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


/**
 * @brief Implements the binary function without connections to any IOHprofiler structures.
 */
static double f_binary_raw(const int *x, const size_t number_of_variables) {
    int i = 0;
    double result;

    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;

    result = 0.0;
    for (i = 0; i < number_of_variables; ++i) {
        result += (double)(pow(2, i) * x[i]);
    }

    return result;
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static void f_binary_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_binary_raw(x, problem->number_of_variables);
    problem->raw_fitness[0] = y[0];
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic binary problem.
 */
static IOHprofiler_problem_t *f_binary_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("binary function",
                                                                               f_binary_evaluate, NULL, number_of_variables, 0, 1, 1);
    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "binary", number_of_variables);

    /* Compute best solution */
    f_binary_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler binary problem.
 */
static IOHprofiler_problem_t *f_binary_IOHprofiler_problem_allocate(const size_t function,
                                                                    const size_t dimension,
                                                                    const size_t instance,
                                                                    const long rseed,
                                                                    const char *problem_id_template,
                                                                    const char *problem_name_template) {
    IOHprofiler_problem_t *problem = NULL;

    assert(dimension < 200);
    problem = f_binary_allocate(dimension);

    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");

    return problem;
}
