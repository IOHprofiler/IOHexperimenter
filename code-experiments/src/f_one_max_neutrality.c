/**
 * @Onemax function
 */

/**
 * @file f_one_max_neutrality.c
 * @brief Implementation of the one_max_neutrality function and problem.
 */

#include <assert.h>
#include <stdio.h>

#include "IOHprofiler.h"
#include "IOHprofiler_problem.c"
#include "suite_PBO_legacy_code.c"
#include "transform_obj_shift.c"
#include "transform_vars_neutrality.c"
#include "transform_obj_scale.c"
#include "transform_vars_neutrality_sigma.c"
#include "transform_vars_neutrality_xor.c"
/**
 * @brief Implements the one_max_neutrality function without connections to any IOHprofiler structures.
 */
static double f_one_max_neutrality_raw(const int *x, const size_t number_of_variables) {
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
static void f_one_max_neutrality_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_one_max_neutrality_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic one_max_neutrality problem.
 */
static IOHprofiler_problem_t *f_one_max_neutrality_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("one_max_neutrality function",
                                                                               f_one_max_neutrality_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "one_max_neutrality", number_of_variables);

    /* Compute best solution */
    f_one_max_neutrality_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler one_max_neutrality problem.
 */
static IOHprofiler_problem_t *f_one_max_neutrality_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {


    int * neutrality;
    int *z, *sigma;
    int temp,t;
    size_t new_dim;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;

  
    neutrality = IOHprofiler_allocate_int_vector(1);
    problem = f_one_max_neutrality_allocate(dimension);
    if(instance == 1){
        neutrality[0] = 3;
        problem = transform_vars_neutrality(problem, neutrality, 0);
    }
    else if(instance > 1 && instance <= 50){
        neutrality[0] = 3;
        new_dim = dimension/(size_t)neutrality[0];
        z = IOHprofiler_allocate_int_vector(new_dim);

        IOHprofiler_compute_xopt(z,rseed,new_dim);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_vars_neutrality_xor(problem, neutrality,z, 0);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
        IOHprofiler_free_memory(z);
    }
    else if(instance > 50 && instance <= 100)
    {
        neutrality[0] = 3;
        new_dim = dimension / 3;
        sigma = IOHprofiler_allocate_int_vector(new_dim);
        xins = IOHprofiler_allocate_vector(new_dim);

        
        IOHprofiler_compute_xopt_double(xins,rseed,new_dim);
        for(i = 0; i < new_dim; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < new_dim; i++){
            t = (int)(xins[i] * (double)new_dim);
            assert(t >= 0 && t < new_dim);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp; 
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        assert(a <= 5.0 && a >= 0.2);

        problem = transform_vars_neutrality_sigma(problem, neutrality,sigma, 0);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);

        IOHprofiler_free_memory(sigma);
        IOHprofiler_free_memory(xins);
    } else {
        neutrality[0] = 3;
        problem = transform_vars_neutrality(problem, neutrality, 0);
    }
    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");

    IOHprofiler_free_memory(neutrality);


    return problem;
}
