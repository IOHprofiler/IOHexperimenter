
/**
 * @file f_N_queens.c
 * @brief Implementation of the N_queens function and problem.
 */

#include <assert.h>
#include <stdio.h>
#include <math.h>

#include "IOHprofiler.h"
#include "IOHprofiler_problem.c"
#include "suite_PBO_legacy_code.c"
#include "transform_obj_shift.c"
#include "transform_vars_sigma.c"
#include "transform_vars_xor.c"
#include "transform_obj_scale.c"

/**
 * @brief Implements the N Queens one dimension function without connections to any IOHprofiler structures.
 */

static double max(double element1 , double element2) {
    if (element1>element2) return element1;
    else return element2;
}
/**
 * @brief Implements the N_queens function without connections to any IOHprofiler structures.
 */
static double f_N_queens_raw(const int *x, const size_t number_of_variables) {
    if(floor(sqrt(number_of_variables))!=sqrt(number_of_variables)){
      fprintf(stderr, "Number of parameters in the N Queen problem must be a square number\n");
      exit(-1);
    }

    double result;
    int index,j,i,k,l;
    int N_queens=sqrt(number_of_variables);
    int number_of_queens_on_board = 0;
    double k_penalty=0.0;
    double l_penalty =0.0;
    double raws_penalty=0.0;
    double columns_penalty=0.0;
    int indx=0;
    float C = N_queens;
    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;


    for (index=0; index<number_of_variables; index++){
        if (x[index]==1){
            number_of_queens_on_board+=1;
        }
    }

    for(j=1; j <=N_queens; j++){
        double sum_column = 0.0;
        for(i=1; i<=N_queens; i++){
            indx=((i-1)*N_queens) + ((j-1)%N_queens);
            sum_column+=(double)x[indx];
        }
        columns_penalty+=max(0.0, (-1.0+sum_column));
    }

    for(i=1; i<=N_queens; i++){
        double sum_raw = 0.0;
        double sum_k = 0.0;
        double sum_l = 0.0;
        for(j=1; j <=N_queens; j++){
            indx=((i-1)*N_queens) + ((j-1)%N_queens);
            sum_raw+=(double)x[indx];
        }
        raws_penalty+=max(0.0, (-1.0+sum_raw));
    }

    for(k=2-N_queens; k<=N_queens-2; k++){
        double sum_k=0.0;
        for(i=1; i<=N_queens; i++){
            if (k+i>=1 && k+i<=N_queens){
                indx=((i-1)*N_queens) + ((k+i-1)%N_queens);
                sum_k += (double)x[indx];
            }
        }
        k_penalty+=max(0.0, (-1.0+sum_k));
    }
    for(l=3; l<=2*N_queens-1; l++){
        double sum_l=0.0;
        for(i=1; i<=N_queens; i++){
            if (l-i>=1 && l-i<=N_queens){
                indx=((i-1)*N_queens) + ((l-i-1)%N_queens);
                sum_l += (double)x[indx];
            }
        }
        l_penalty+=max(0.0, (-1.0+sum_l));
    }
    result = number_of_queens_on_board - columns_penalty - raws_penalty - k_penalty - l_penalty;
    return result;
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static void f_N_queens_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_N_queens_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic N_queens problem.
 */
static IOHprofiler_problem_t *f_N_queens_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("N_queens function",
                                                                               f_N_queens_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "N_queens", number_of_variables);

    /* Compute best solution */
    /*f_N_queens_evaluate(problem, problem->best_parameter, problem->best_value);*/
    problem->best_value[0] = sqrt(number_of_variables);
    return problem;
}

/**
 * @brief Creates the IOHprofiler N_queens problem.
 */
static IOHprofiler_problem_t *f_N_queens_IOHprofiler_problem_allocate(const size_t function,
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
    problem = f_N_queens_allocate(dimension);

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

    IOHprofiler_free_memory(z);;
    return problem;
}
