/**
 * @Onemax function
 */

/**
 * @file f_leading_ones_ruggedness1.c
 * @brief Implementation of the leading_ones_ruggedness1 function and problem.
 */

#include <assert.h>
#include <stdio.h>

#include "IOHprofiler.h"
#include "IOHprofiler_problem.c"
#include "suite_PBO_legacy_code.c"
#include "transform_obj_shift.c"
#include "transform_obj_scale.c"
#include "transform_obj_ruggedness1.c"
/**
 * @brief Implements the leading_ones_ruggedness1 function without connections to any IOHprofiler structures.
 */
static double f_leading_ones_ruggedness1_raw(const int *x, const size_t number_of_variables) {
    size_t i = 0;
    double result;

    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;
    result = 0.0;
    for (i = 0; i < number_of_variables; ++i) {
        result += (double)x[i];
    }

    return result;
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static void f_leading_ones_ruggedness1_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_leading_ones_ruggedness1_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic leading_ones_ruggedness1 problem.
 */
static IOHprofiler_problem_t *f_leading_ones_ruggedness1_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("leading_ones_ruggedness1 function",
                                                                               f_leading_ones_ruggedness1_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "leading_ones_ruggedness1", number_of_variables);

    /* Compute best solution */
    f_leading_ones_ruggedness1_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler leading_ones_ruggedness1 problem.
 */
static IOHprofiler_problem_t *f_leading_ones_ruggedness1_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {
    int temp,t;
    double a;
    double b;
    IOHprofiler_problem_t *problem;

    problem = f_leading_ones_ruggedness1_allocate(dimension);
    if(instance == 1){
       problem = transform_obj_ruggedness1(problem);
    }
    else if(instance > 1 && instance <= 100){
        problem = transform_obj_ruggedness1(problem);

        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else {
        problem = transform_obj_ruggedness1(problem);
    }
    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");

    return problem;
}
