/**
 * @file IOHprofiler_problem.c
 * @brief Definitions of functions regarding IOHprofiler problems.
 */

#include <float.h>
#include "IOHprofiler.h"
#include "IOHprofiler_internal.h"

#include "IOHprofiler_utilities.c"

/**
 * Evaluates the problem function, increases the number of evaluations and updates the best observed value
 * and the best observed evaluation number.
 *
 * @note Both x and y must point to correctly sized allocated memory regions.
 *
 * @param problem The given IOHprofiler problem.
 * @param x The decision vector.
 * @param y The objective vector that is the result of the evaluation (in single-objective problems only the
 * first vector item is being set). Currently multi-objective is not supported.
 * @param number_of_parameters The number of parameters need to be logged. Set as zero by default.
 * @param parameters The list of parameters. Set as NULL by default.
 */
void IOHprofiler_evaluate_function(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem != NULL);
    assert(problem->evaluate_function != NULL);

    problem->evaluate_function(problem, x, y);
    problem->evaluations++; /* each derived class has its own counter, only the most outer will be visible */

    /* A little bit of bookkeeping */
    if (y[0] > problem->best_observed_fvalue[0]) {
        problem->best_observed_fvalue[0] = y[0];
        problem->best_observed_evaluation[0] = problem->evaluations;
    }
}

/**
 * Evaluates and logs the given solution (as the IOHprofiler_evaluate_function), but does not return the evaluated
 * value.
 *
 * @note None of the observers implements this function yet!
 * @note x must point to a correctly sized allocated memory region.

 * @param problem The given IOHprofiler problem.
 * @param x The decision vector.
 */
void IOHprofiler_recommend_solution(IOHprofiler_problem_t *problem, const int *x) {
    assert(problem != NULL);
    if (problem->recommend_solution == NULL) {
        IOHprofiler_error("IOHprofiler_recommend_solutions(): No recommend solution function implemented for problem %s",
                          problem->problem_id);
    }
    problem->recommend_solution(problem, x);
}

/**
 * @brief Allocates a new IOHprofiler_problem_t for the given number of variables and number of objectives.
 */
static IOHprofiler_problem_t *IOHprofiler_problem_allocate(const size_t number_of_variables,
                                                           const size_t number_of_objectives) {
    IOHprofiler_problem_t *problem;
    problem = (IOHprofiler_problem_t *)IOHprofiler_allocate_memory(sizeof(*problem));
    /* Initialize fields to sane/safe defaults */
    problem->initial_solution = NULL;
    problem->evaluate_function = NULL;
    problem->recommend_solution = NULL;
    problem->problem_free_function = NULL;
    problem->number_of_variables = number_of_variables;
    problem->number_of_objectives = number_of_objectives;
    problem->number_of_parameters = 0;
    problem->parameters = NULL;
    problem->smallest_values_of_interest = IOHprofiler_allocate_int_vector(number_of_variables);
    problem->largest_values_of_interest = IOHprofiler_allocate_int_vector(number_of_variables);
    problem->best_parameter = IOHprofiler_allocate_int_vector(number_of_variables);
    problem->best_value = IOHprofiler_allocate_vector(number_of_objectives);
    problem->raw_fitness = IOHprofiler_allocate_vector(number_of_objectives);
    if (number_of_objectives > 1)
        problem->nadir_value = IOHprofiler_allocate_vector(number_of_objectives);
    else
        problem->nadir_value = NULL;
    problem->problem_name = NULL;
    problem->problem_id = NULL;
    problem->problem_type = NULL;
    problem->evaluations = 0;
    problem->final_target_delta[0] = 1e-8; /* in case to be modified by the benchmark */
    problem->best_observed_fvalue[0] = DBL_MIN_EXP;
    problem->best_observed_evaluation[0] = 0;
    problem->suite = NULL; /* To be initialized in the IOHprofiler_suite_get_problem_from_indices() function */
    problem->suite_dep_index = 0;
    problem->suite_dep_function = 0;
    problem->suite_dep_instance = 0;
    problem->data = NULL;
    return problem;
}

/**
 * @brief Creates a duplicate of the 'other' problem for all fields except for data, which points to NULL.
 */
static IOHprofiler_problem_t *IOHprofiler_problem_duplicate(const IOHprofiler_problem_t *other) {
    size_t i;
    IOHprofiler_problem_t *problem;
    problem = IOHprofiler_problem_allocate(other->number_of_variables, other->number_of_objectives);

    problem->initial_solution = other->initial_solution;
    problem->evaluate_function = other->evaluate_function;
    problem->recommend_solution = other->recommend_solution;
    problem->problem_free_function = other->problem_free_function;

    for (i = 0; i < problem->number_of_variables; ++i) {
        problem->smallest_values_of_interest[i] = other->smallest_values_of_interest[i];
        problem->largest_values_of_interest[i] = other->largest_values_of_interest[i];
        if (other->best_parameter)
            problem->best_parameter[i] = other->best_parameter[i];
    }

    if (other->best_value)
        for (i = 0; i < problem->number_of_objectives; ++i) {
            problem->best_value[i] = other->best_value[i];
        }

    if (other->nadir_value)
        for (i = 0; i < problem->number_of_objectives; ++i) {
            problem->nadir_value[i] = other->nadir_value[i];
        }

    if (other->raw_fitness)
        for (i = 0; i < problem->number_of_objectives; ++i) {
            problem->raw_fitness[i] = other->raw_fitness[i];
        }

    problem->number_of_parameters = other->number_of_parameters;
    for (i = 0; i < problem->number_of_parameters; ++i) {
        problem->parameters[i] = other->parameters[i];
    }

    problem->problem_name = IOHprofiler_strdup(other->problem_name);
    problem->problem_id = IOHprofiler_strdup(other->problem_id);
    problem->problem_type = IOHprofiler_strdup(other->problem_type);

    problem->evaluations = other->evaluations;
    problem->final_target_delta[0] = other->final_target_delta[0];
    problem->best_observed_fvalue[0] = other->best_observed_fvalue[0];
    problem->best_observed_evaluation[0] = other->best_observed_evaluation[0];

    problem->suite = other->suite;
    problem->suite_dep_index = other->suite_dep_index;
    problem->suite_dep_function = other->suite_dep_function;
    problem->suite_dep_instance = other->suite_dep_instance;

    problem->data = NULL;

    return problem;
}

/**
 * @brief Allocates a problem using scalar values for smallest_value_of_interest, largest_value_of_interest
 * and best_parameter.
 * Note that the number of objectives is set as 1 by default.
 */
static IOHprofiler_problem_t *IOHprofiler_problem_allocate_from_scalars(const char *problem_name,
                                                                        IOHprofiler_evaluate_function_t evaluate_function,
                                                                        IOHprofiler_problem_free_function_t problem_free_function,
                                                                        const size_t number_of_variables,
                                                                        const int smallest_value_of_interest,
                                                                        const int largest_value_of_interest,
                                                                        const int best_parameter) {
    size_t i;

    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate(number_of_variables, 1);

    problem->problem_name = IOHprofiler_strdup(problem_name);
    problem->number_of_variables = number_of_variables;
    problem->number_of_objectives = 1;
    problem->evaluate_function = evaluate_function;
    problem->problem_free_function = problem_free_function;

    for (i = 0; i < number_of_variables; ++i) {
        problem->smallest_values_of_interest[i] = smallest_value_of_interest;
        problem->largest_values_of_interest[i] = largest_value_of_interest;
        problem->best_parameter[i] = best_parameter;
    }
    return problem;
}

void IOHprofiler_problem_free(IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    if (problem->problem_free_function != NULL) {
        problem->problem_free_function(problem);
    } else {
        /* Best guess at freeing all relevant structures */
        if (problem->smallest_values_of_interest != NULL)
            IOHprofiler_free_memory(problem->smallest_values_of_interest);
        if (problem->largest_values_of_interest != NULL)
            IOHprofiler_free_memory(problem->largest_values_of_interest);
        if (problem->best_parameter != NULL)
            IOHprofiler_free_memory(problem->best_parameter);
        if (problem->best_value != NULL)
            IOHprofiler_free_memory(problem->best_value);
        if (problem->raw_fitness != NULL)
            IOHprofiler_free_memory(problem->raw_fitness);
        if (problem->nadir_value != NULL)
            IOHprofiler_free_memory(problem->nadir_value);
        if (problem->problem_name != NULL)
            IOHprofiler_free_memory(problem->problem_name);
        if (problem->problem_id != NULL)
            IOHprofiler_free_memory(problem->problem_id);
        if (problem->problem_type != NULL)
            IOHprofiler_free_memory(problem->problem_type);
        if (problem->data != NULL)
            IOHprofiler_free_memory(problem->data);
        if (problem->parameters != NULL)
            IOHprofiler_free_memory(problem->parameters);
        problem->smallest_values_of_interest = NULL;
        problem->largest_values_of_interest = NULL;
        problem->best_parameter = NULL;
        problem->best_value = NULL;
        problem->raw_fitness = NULL;
        problem->nadir_value = NULL;
        problem->suite = NULL;
        problem->data = NULL;
        problem->parameters = NULL;
        IOHprofiler_free_memory(problem);
    }
}

/**
 * @brief Checks whether the given string is in the right format to be a problem_id.
 *
 * No non-alphanumeric characters besides '-', '_' and '.' are allowed.
 */
static int IOHprofiler_problem_id_is_fine(const char *id, ...) {
    va_list args;
    const int reject = 0;
    const int accept = 1;
    const char *cp;
    char *s;
    int result = accept;

    va_start(args, id);
    s = IOHprofiler_vstrdupf(id, args);
    va_end(args);
    for (cp = s; *cp != '\0'; ++cp) {
        if (('A' <= *cp) && (*cp <= 'Z'))
            continue;
        if (('a' <= *cp) && (*cp <= 'z'))
            continue;
        if ((*cp == '_') || (*cp == '-'))
            continue;
        if (('0' <= *cp) && (*cp <= '9'))
            continue;
        result = reject;
    }
    IOHprofiler_free_memory(s);
    return result;
}

/**
 * @brief Sets the problem_parameters.
 *
 */
void IOHprofiler_problem_set_parameters(IOHprofiler_problem_t *problem, const size_t number_of_parameters, const double *parameters) {
    size_t i;
    if (number_of_parameters != 0) {
        problem->number_of_parameters = number_of_parameters;
        if (problem->parameters != NULL) {
            IOHprofiler_free_memory(problem->parameters);
        }
        problem->parameters = IOHprofiler_allocate_vector(number_of_parameters);
        for (i = 0; i < problem->number_of_parameters; ++i) {
            problem->parameters[i] = parameters[i];
        }
    }
}

/**
 * @brief Sets the problem_id using formatted printing (as in printf).
 *
 * Takes care of memory (de-)allocation and verifies that the problem_id is in the correct format.
 */
static void IOHprofiler_problem_set_id(IOHprofiler_problem_t *problem, const char *id, ...) {
    va_list args;

    va_start(args, id);
    if (problem->problem_id != NULL)
        IOHprofiler_free_memory(problem->problem_id);
    problem->problem_id = IOHprofiler_vstrdupf(id, args);
    va_end(args);
    if (!IOHprofiler_problem_id_is_fine(problem->problem_id)) {
        IOHprofiler_error("Problem id should only contain standard chars, not like '%s'", problem->problem_id);
    }
}

/**
 * @brief Sets the problem_name using formatted printing (as in printf).
 *
 * Takes care of memory (de-)allocation.
 */
static void IOHprofiler_problem_set_name(IOHprofiler_problem_t *problem, const char *name, ...) {
    va_list args;

    va_start(args, name);
    if (problem->problem_name != NULL)
        IOHprofiler_free_memory(problem->problem_name);
    problem->problem_name = IOHprofiler_vstrdupf(name, args);
    va_end(args);
}

/**
 * @brief Sets the problem_type using formatted printing (as in printf).
 *
 * Takes care of memory (de-)allocation.
 */
static void IOHprofiler_problem_set_type(IOHprofiler_problem_t *problem, const char *type, ...) {
    va_list args;

    va_start(args, type);
    if (problem->problem_type != NULL)
        IOHprofiler_free_memory(problem->problem_type);
    problem->problem_type = IOHprofiler_vstrdupf(type, args);
    va_end(args);
}

size_t IOHprofiler_problem_get_evaluations(const IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    return problem->evaluations;
}

/**
 * @brief Returns 1 if the best parameter is not (close to) zero and 0 otherwise.
 */
static int IOHprofiler_problem_best_parameter_not_zero(const IOHprofiler_problem_t *problem) {
    size_t i = 0;
    int best_is_zero = 1;

    if (IOHprofiler_vector_contains_nan(problem->best_parameter, problem->number_of_variables))
        return 1;

    while (i < problem->number_of_variables && best_is_zero) {
        best_is_zero = IOHprofiler_double_almost_equal(problem->best_parameter[i], 0, 1e-9);
        i++;
    }

    return !best_is_zero;
}

/**
 * @note Can be used to prevent unnecessary burning of CPU time.
 */
int IOHprofiler_problem_final_target_hit(const IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    if (IOHprofiler_problem_get_number_of_objectives(problem) != 1 ||
        IOHprofiler_problem_get_evaluations(problem) < 1)
        return 0;
    if (problem->best_value == NULL)
        return 0;
    return problem->best_observed_fvalue[0] >= problem->best_value[0] - problem->final_target_delta[0] ? 1 : 0;
}

/**
 * @note Tentative...
 */
double IOHprofiler_problem_get_best_observed_fvalue1(const IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    return problem->best_observed_fvalue[0];
}

/**
 * @note This function breaks the black-box property: the returned  value is not
 * meant to be used by the optimization algorithm.
 */
double IOHprofiler_problem_get_final_target_fvalue1(const IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    assert(problem->best_value != NULL);
    assert(problem->final_target_delta != NULL);
    return problem->best_value[0] + problem->final_target_delta[0];
}

/**
 * @note Do not modify the returned string! If you free the problem, the returned pointer becomes invalid.
 * When in doubt, use IOHprofiler_strdup() on the returned value.
 */
const char *IOHprofiler_problem_get_name(const IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    assert(problem->problem_name != NULL);
    return problem->problem_name;
}

/**
 * The ID is guaranteed to contain only characters in the set [a-z0-9_-]. It should therefore be safe to use
 * it to construct filenames or other identifiers.
 *
 * Each problem ID should be unique within each benchmark suite.
 *
 * @note Do not modify the returned string! If you free the problem, the returned pointer becomes invalid.
 * When in doubt, use IOHprofiler_strdup() on the returned value.
 */
const char *IOHprofiler_problem_get_id(const IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    assert(problem->problem_id != NULL);
    return problem->problem_id;
}

const char *IOHprofiler_problem_get_type(const IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    assert(problem->problem_type != NULL);
    return problem->problem_type;
}

size_t IOHprofiler_problem_get_dimension(const IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    assert(problem->number_of_variables > 0);
    return problem->number_of_variables;
}

size_t IOHprofiler_problem_get_number_of_objectives(const IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    assert(problem->number_of_objectives > 0);
    return problem->number_of_objectives;
}
const int *IOHprofiler_problem_get_smallest_values_of_interest(const IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    assert(problem->smallest_values_of_interest != NULL);
    return problem->smallest_values_of_interest;
}

const int *IOHprofiler_problem_get_largest_values_of_interest(const IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    assert(problem->largest_values_of_interest != NULL);
    return problem->largest_values_of_interest;
}

/**
 * If a special method for setting an initial solution to the problem does not exist, the center of the
 * problem's region of interest is the initial solution.
 * @param problem The given IOHprofiler problem.
 * @param initial_solution The pointer to the initial solution being set by this method.
 */
void IOHprofiler_problem_get_initial_solution(const IOHprofiler_problem_t *problem, int *initial_solution) {
    assert(problem != NULL);
    if (problem->initial_solution != NULL) {
        problem->initial_solution(problem, initial_solution);
    } else {
        size_t i;
        assert(problem->smallest_values_of_interest != NULL);
        assert(problem->largest_values_of_interest != NULL);
        for (i = 0; i < problem->number_of_variables; ++i)
            initial_solution[i] = (int)(problem->smallest_values_of_interest[i] + 0.5 * (problem->largest_values_of_interest[i] - problem->smallest_values_of_interest[i]));
    }
}

static IOHprofiler_suite_t *IOHprofiler_problem_get_suite(const IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    return problem->suite;
}

static void IOHprofiler_problem_set_suite(IOHprofiler_problem_t *problem, IOHprofiler_suite_t *suite) {
    assert(problem != NULL);
    problem->suite = suite;
}

size_t IOHprofiler_problem_get_suite_dep_index(const IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    return problem->suite_dep_index;
}

static size_t IOHprofiler_problem_get_suite_dep_function(const IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    assert(problem->suite_dep_function > 0);
    return problem->suite_dep_function;
}

static size_t IOHprofiler_problem_get_suite_dep_instance(const IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    assert(problem->suite_dep_instance > 0);
    return problem->suite_dep_instance;
}

/**
 * @brief Returns the data of the transformed problem.
 */
static void *IOHprofiler_problem_transformed_get_data(const IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    assert(problem->data != NULL);
    assert(((IOHprofiler_problem_transformed_data_t *)problem->data)->data != NULL);

    return ((IOHprofiler_problem_transformed_data_t *)problem->data)->data;
}

/**
 * @brief Returns the inner problem of the transformed problem.
 */
static IOHprofiler_problem_t *IOHprofiler_problem_transformed_get_inner_problem(const IOHprofiler_problem_t *problem) {
    assert(problem != NULL);
    assert(problem->data != NULL);
    assert(((IOHprofiler_problem_transformed_data_t *)problem->data)->inner_problem != NULL);

    return ((IOHprofiler_problem_transformed_data_t *)problem->data)->inner_problem;
}

/**
 * @brief Calls the IOHprofiler_evaluate_function function on the inner problem.
 */
static void IOHprofiler_problem_transformed_evaluate_function(IOHprofiler_problem_t *problem, const int *x, double *y) {
    IOHprofiler_problem_transformed_data_t *data;
    assert(problem != NULL);
    assert(problem->data != NULL);
    data = (IOHprofiler_problem_transformed_data_t *)problem->data;
    assert(data->inner_problem != NULL);

    IOHprofiler_evaluate_function(data->inner_problem, x, y);
}

/**
 * @brief Calls the IOHprofiler_recommend_solution function on the inner problem.
 */
static void IOHprofiler_problem_transformed_recommend_solution(IOHprofiler_problem_t *problem, const int *x) {
    IOHprofiler_problem_transformed_data_t *data;
    assert(problem != NULL);
    assert(problem->data != NULL);
    data = (IOHprofiler_problem_transformed_data_t *)problem->data;
    assert(data->inner_problem != NULL);

    IOHprofiler_recommend_solution(data->inner_problem, x);
}

/**
 * @brief Frees only the data of the transformed problem leaving the inner problem intact.
 *
 * @note If there is no other pointer to the inner problem, access to it will be lost.
 */
static void IOHprofiler_problem_transformed_free_data(IOHprofiler_problem_t *problem) {
    IOHprofiler_problem_transformed_data_t *data;

    assert(problem != NULL);
    assert(problem->data != NULL);
    data = (IOHprofiler_problem_transformed_data_t *)problem->data;

    if (data->data != NULL) {
        if (data->data_free_function != NULL) {
            data->data_free_function(data->data);
            data->data_free_function = NULL;
        }
        IOHprofiler_free_memory(data->data);
        data->data = NULL;
    }
    /* Let the generic free problem code deal with the rest of the fields. For this we clear the free_problem
   * function pointer and recall the generic function. */
    problem->problem_free_function = NULL;
    IOHprofiler_problem_free(problem);
}

/**
 * @brief Frees the transformed problem.
 */
static void IOHprofiler_problem_transformed_free(IOHprofiler_problem_t *problem) {
    IOHprofiler_problem_transformed_data_t *data;

    assert(problem != NULL);
    assert(problem->data != NULL);
    data = (IOHprofiler_problem_transformed_data_t *)problem->data;
    assert(data->inner_problem != NULL);
    if (data->inner_problem != NULL) {
        IOHprofiler_problem_free(data->inner_problem);
        data->inner_problem = NULL;
    }
    IOHprofiler_problem_transformed_free_data(problem);
}

/**
 * @brief Allocates a transformed problem that wraps the inner_problem.
 *
 * By default all methods will dispatch to the inner_problem. A prefix is prepended to the problem name
 * in order to reflect the transformation somewhere.
 */
static IOHprofiler_problem_t *IOHprofiler_problem_transformed_allocate(IOHprofiler_problem_t *inner_problem,
                                                                       void *user_data,
                                                                       IOHprofiler_data_free_function_t data_free_function,
                                                                       const char *name_prefix) {
    IOHprofiler_problem_transformed_data_t *problem;
    IOHprofiler_problem_t *inner_copy;
    char *old_name = IOHprofiler_strdup(inner_problem->problem_name);

    problem = (IOHprofiler_problem_transformed_data_t *)IOHprofiler_allocate_memory(sizeof(*problem));
    problem->inner_problem = inner_problem;
    problem->data = user_data;
    problem->data_free_function = data_free_function;

    inner_copy = IOHprofiler_problem_duplicate(inner_problem);
    inner_copy->evaluate_function = IOHprofiler_problem_transformed_evaluate_function;
    inner_copy->recommend_solution = IOHprofiler_problem_transformed_recommend_solution;
    inner_copy->problem_free_function = IOHprofiler_problem_transformed_free;
    inner_copy->data = problem;

    IOHprofiler_problem_set_name(inner_copy, "%s(%s)", name_prefix, old_name);
    IOHprofiler_free_memory(old_name);

    return inner_copy;
}

/**
 * @brief Calls the IOHprofiler_evaluate_function function on the underlying problems.
 */
static void IOHprofiler_problem_stacked_evaluate_function(IOHprofiler_problem_t *problem, const int *x, double *y) {
    IOHprofiler_problem_stacked_data_t *data = (IOHprofiler_problem_stacked_data_t *)problem->data;

    assert(
        IOHprofiler_problem_get_number_of_objectives(problem) == IOHprofiler_problem_get_number_of_objectives(data->problem1) + IOHprofiler_problem_get_number_of_objectives(data->problem2));

    IOHprofiler_evaluate_function(data->problem1, x, &y[0]);
    IOHprofiler_evaluate_function(data->problem2, x, &y[IOHprofiler_problem_get_number_of_objectives(data->problem1)]);
}

/**
 * @brief Frees the stacked problem.
 */
static void IOHprofiler_problem_stacked_free(IOHprofiler_problem_t *problem) {
    IOHprofiler_problem_stacked_data_t *data;

    assert(problem != NULL);
    assert(problem->data != NULL);
    data = (IOHprofiler_problem_stacked_data_t *)problem->data;

    if (data->problem1 != NULL) {
        IOHprofiler_problem_free(data->problem1);
        data->problem1 = NULL;
    }
    if (data->problem2 != NULL) {
        IOHprofiler_problem_free(data->problem2);
        data->problem2 = NULL;
    }
    /* Let the generic free problem code deal with the rest of the fields. For this we clear the free_problem
   * function pointer and recall the generic function. */
    problem->problem_free_function = NULL;
    IOHprofiler_problem_free(problem);
}
