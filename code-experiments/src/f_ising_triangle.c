/**
 * @ising_triangle function
 */

/**
 * @file f_ising_triangle.c
 * @brief Implementation of the ising_triangle function and problem.
 */

#include <assert.h>
#include <stdio.h>

#include "IOHprofiler.h"
#include "IOHprofiler_problem.c"
#include "suite_PBO_legacy_code.c"
#include "transform_obj_shift.c"
#include "transform_vars_sigma.c"
#include "transform_vars_xor.c"
#include "transform_obj_scale.c"

/**
 * @brief Implements the Ising_triangle function without connections to any IOHprofiler structures.
 */
int modulo_ising_triangle(int x,int N){
    return (x % N + N) %N;
}


/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */

static int f_ising_triangle_raw(const int *x, const size_t number_of_variables) {

    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;
    size_t i,j,neig;
    int result = 0;
    int neighbors[6];
    int lattice_size = (int)sqrt(number_of_variables);
    int (*spin_array)[lattice_size] = (int (*)[lattice_size])x;

    for (i = 0; i < lattice_size; ++i) {
           for (j = 0; j < lattice_size; ++j) {
                neighbors[0] = spin_array[modulo_ising_triangle((i - 1), lattice_size)][j] ;
                neighbors[1] = spin_array[modulo_ising_triangle((i + 1) , lattice_size)][j] ;
                neighbors[2] = spin_array[i][modulo_ising_triangle((j - 1) , lattice_size)] ;
                neighbors[3] = spin_array[i][modulo_ising_triangle((i + 1) , lattice_size)] ;
                neighbors[4] = spin_array[modulo_ising_triangle((i - 1) , lattice_size)][modulo_ising_triangle((j - 1) , lattice_size)] ;
                neighbors[5] = spin_array[modulo_ising_triangle((i + 1) , lattice_size)][modulo_ising_triangle((j + 1) , lattice_size)];

                for (neig=0; neig<6; neig++) {
                    result+= (spin_array[i][j] * neighbors[neig]) + ((1- spin_array[i][j])*(1- neighbors[neig]));
                }
           }
    }

    return result;
}



/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static void f_ising_triangle_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_ising_triangle_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic ising_triangle problem.
 */
static IOHprofiler_problem_t *f_ising_triangle_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("ising_triangle function",
                                                                               f_ising_triangle_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "ising_triangle", number_of_variables);

    /* Compute best solution */
    f_ising_triangle_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler ising_triangle problem.
 */
static IOHprofiler_problem_t *f_ising_triangle_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {


    int *z;
    int temp,t;
    size_t i;
    double a;
    double b;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);
    problem = f_ising_triangle_allocate(dimension);

    if(instance == 1){
        for(i = 0; i < dimension; i++)
            z[i] = 0;
        a = 0.0;
        problem = transform_vars_xor(problem,z,0);
        problem = transform_obj_shift(problem,a);
    }
    else if(instance > 1 && instance <= 100){
        for(i = 0; i < dimension; i++)
            z[i] = 0;
        problem = transform_vars_xor(problem,z,0);
        
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    } else {
        for (i = 0; i < dimension; i++)
            z[i] = 0;
        a = 0.0;
        problem = transform_vars_xor(problem, z, 0);
        problem = transform_obj_shift(problem, a);
    }
    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");

    IOHprofiler_free_memory(z);
    return problem;
}
