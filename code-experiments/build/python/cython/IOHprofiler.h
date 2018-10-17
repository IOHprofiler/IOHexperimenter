/**
 * @file IOHprofiler.h
 * @brief All public IOHprofiler functions and variables are defined in this file.
 *
 * It is the authoritative reference, if any function deviates from the documented behavior it is considered
 * a bug. See the function definitions for their detailed descriptions.
 */

#ifndef __IOHprofiler_H__
#define __IOHprofiler_H__

#include <stddef.h>

/* Definitions of some 32 and 64-bit types (used by the random number generator) */
#ifdef _MSC_VER
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

/* Include definition for NAN among other things */
#include <float.h>
#include <math.h>
#ifndef NAN
/** @brief Definition of NAN to be used only if undefined by the included headers */
#define NAN 8.8888e88
#endif
#ifndef isnan
/** @brief Definition of isnan to be used only if undefined by the included headers */
#define isnan(x) (0)
#endif
#ifndef INFINITY
/** @brief Definition of INFINITY to be used only if undefined by the included headers */
#define INFINITY 1e22
/* why not using 1e99? */
#endif
#ifndef isinf
/** @brief Definition of isinf to be used only if undefined by the included headers */
#define isinf(x) (0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief IOHprofiler's version.
 *
 * Automatically updated by do.py.
 */
/**@{*/
static const char IOHprofiler_version[32] = "";
/**@}*/

/***********************************************************************************************************/
/**
 * @brief IOHprofiler's own pi constant. Simplifies the case, when the value of pi changes.
 */
/**@{*/
static const double IOHprofiler_pi = 3.14159265358979323846;
/**@}*/

/***********************************************************************************************************/

/** @brief Logging level type. */
typedef enum {
    IOHprofiler_ERROR,   /**< @brief only error messages are output */
    IOHprofiler_WARNING, /**< @brief error and warning messages are output */
    IOHprofiler_INFO,    /**< @brief error, warning and info messages are output */
    IOHprofiler_DEBUG    /**< @brief error, warning, info and debug messages are output */
} IOHprofiler_log_level_type_e;

/***********************************************************************************************************/

/** @brief Structure containing a IOHprofiler problem. */
struct IOHprofiler_problem_s;

/**
 * @brief The IOHprofiler problem type.
 *
 * See IOHprofiler_problem_s for more information on its fields. */
typedef struct IOHprofiler_problem_s IOHprofiler_problem_t;

/** @brief Structure containing a IOHprofiler suite. */
struct IOHprofiler_suite_s;

/**
 * @brief The IOHprofiler suite type.
 *
 * See IOHprofiler_suite_s for more information on its fields. */
typedef struct IOHprofiler_suite_s IOHprofiler_suite_t;

/** @brief Structure containing a IOHprofiler observer. */
struct IOHprofiler_observer_s;

/**
 * @brief The IOHprofiler observer type.
 *
 * See IOHprofiler_observer_s for more information on its fields. */
typedef struct IOHprofiler_observer_s IOHprofiler_observer_t;

/** @brief Structure containing a IOHprofiler archive. */
struct IOHprofiler_archive_s;

/**
 * @brief The IOHprofiler archive type.
 *
 * See IOHprofiler_archive_s for more information on its fields. */
typedef struct IOHprofiler_archive_s IOHprofiler_archive_t;

/** @brief Structure containing a IOHprofiler random state. */
struct IOHprofiler_random_state_s;

/**
 * @brief The IOHprofiler random state type.
 *
 * See IOHprofiler_random_state_s for more information on its fields. */
typedef struct IOHprofiler_random_state_s IOHprofiler_random_state_t;

/***********************************************************************************************************/

/**
 * @name Methods regarding IOHprofiler suite
 */
/**@{*/

/**
 * @brief Constructs a IOHprofiler suite.
 */
IOHprofiler_suite_t *IOHprofiler_suite(const char *suite_name, const char *suite_instance, const char *suite_options);

/**
 * @brief Frees the given suite.
 */
void IOHprofiler_suite_free(IOHprofiler_suite_t *suite);

/**
 * @brief Returns the next (observed) problem of the suite or NULL if there is no next problem left.
 */
IOHprofiler_problem_t *IOHprofiler_suite_get_next_problem(IOHprofiler_suite_t *suite, IOHprofiler_observer_t *observer);

/**
 *
 * @returns The current problem of the suite.
 */
IOHprofiler_problem_t *IOHprofiler_suite_reset_problem(IOHprofiler_suite_t *suite, IOHprofiler_observer_t *observer);

/**
 * @brief Returns the problem of the suite defined by problem_index.
 */
IOHprofiler_problem_t *IOHprofiler_suite_get_problem(IOHprofiler_suite_t *suite, const size_t problem_index);

/**
 * @brief Returns the number of problems in the given suite.
 */
size_t IOHprofiler_suite_get_number_of_problems(const IOHprofiler_suite_t *suite);

/**
 * @brief Returns the function number in the suite in position function_idx (counting from 0).
 */
size_t IOHprofiler_suite_get_function_from_function_index(const IOHprofiler_suite_t *suite, const size_t function_idx);

/**
 * @brief Returns the dimension number in the suite in position dimension_idx (counting from 0).
 */
size_t IOHprofiler_suite_get_dimension_from_dimension_index(const IOHprofiler_suite_t *suite, const size_t dimension_idx);

/**
 * @brief Returns the instance number in the suite in position instance_idx (counting from 0).
 */
size_t IOHprofiler_suite_get_instance_from_instance_index(const IOHprofiler_suite_t *suite, const size_t instance_idx);
/**@}*/

/**
 * @name Encoding/decoding problem index
 *
 * General schema for encoding/decoding a problem index. Note that the index depends on the number of
 * instances a suite is defined with (it should be called a suite-instance-depending index...).
 * Also, while functions, instances and dimensions start from 1, function_idx, instance_idx and dimension_idx
 * as well as suite_dep_index start from 0!
 *
 * Showing an example with 2 dimensions (2, 3), 5 instances (6, 7, 8, 9, 10) and 2 functions (1, 2):
 *
   \verbatim
   index | instance | function | dimension
   ------+----------+----------+-----------
       0 |        6 |        1 |         2
       1 |        7 |        1 |         2
       2 |        8 |        1 |         2
       3 |        9 |        1 |         2
       4 |       10 |        1 |         2
       5 |        6 |        2 |         2
       6 |        7 |        2 |         2
       7 |        8 |        2 |         2
       8 |        9 |        2 |         2
       9 |       10 |        2 |         2
      10 |        6 |        1 |         3
      11 |        7 |        1 |         3
      12 |        8 |        1 |         3
      13 |        9 |        1 |         3
      14 |       10 |        1 |         3
      15 |        6 |        2 |         2
      16 |        7 |        2 |         3
      17 |        8 |        2 |         3
      18 |        9 |        2 |         3
      19 |       10 |        2 |         3

   index | instance_idx | function_idx | dimension_idx
   ------+--------------+--------------+---------------
       0 |            0 |            0 |             0
       1 |            1 |            0 |             0
       2 |            2 |            0 |             0
       3 |            3 |            0 |             0
       4 |            4 |            0 |             0
       5 |            0 |            1 |             0
       6 |            1 |            1 |             0
       7 |            2 |            1 |             0
       8 |            3 |            1 |             0
       9 |            4 |            1 |             0
      10 |            0 |            0 |             1
      11 |            1 |            0 |             1
      12 |            2 |            0 |             1
      13 |            3 |            0 |             1
      14 |            4 |            0 |             1
      15 |            0 |            1 |             1
      16 |            1 |            1 |             1
      17 |            2 |            1 |             1
      18 |            3 |            1 |             1
      19 |            4 |            1 |             1
   \endverbatim
 */
/**@{*/
/**
 * @brief Computes the index of the problem in the suite that corresponds to the given function, dimension
 * and instance indices.
 */
size_t IOHprofiler_suite_encode_problem_index(const IOHprofiler_suite_t *suite,
                                              const size_t function_idx,
                                              const size_t dimension_idx,
                                              const size_t instance_idx);

/**
 * @brief Computes the function, dimension and instance indexes of the problem with problem_index in the
 * given suite.
 */
void IOHprofiler_suite_decode_problem_index(const IOHprofiler_suite_t *suite,
                                            const size_t problem_index,
                                            size_t *function_idx,
                                            size_t *dimension_idx,
                                            size_t *instance_idx);
/**@}*/

/***********************************************************************************************************/

/**
 * @name Methods regarding IOHprofiler observer
 */
/**@{*/
/**
 * @brief Constructs a IOHprofiler observer.
 */
IOHprofiler_observer_t *IOHprofiler_observer(const char *observer_name, const char *options);

/**
 * @brief Frees the given observer.
 */
void IOHprofiler_observer_free(IOHprofiler_observer_t *observer);

/**
 * @brief Adds an observer to the given problem.
 */
IOHprofiler_problem_t *IOHprofiler_problem_add_observer(IOHprofiler_problem_t *problem, IOHprofiler_observer_t *observer);

/**
 * @brief Removes an observer from the given problem.
 */
IOHprofiler_problem_t *IOHprofiler_problem_remove_observer(IOHprofiler_problem_t *problem, IOHprofiler_observer_t *observer);

/**@}*/

/***********************************************************************************************************/

/**
 * @name Methods regarding IOHprofiler problem
 */
/**@{*/
/**
 * @brief Evaluates the problem function in point x and save the result in y.
  logger_** functions use number_of_parameters and parameters to log extra informations of algorithms' adaptive parameters.
  If there is no need to log parameters, set number_of_parameters and parameters as 0 and NULL respectively.
 */
void IOHprofiler_evaluate_function(IOHprofiler_problem_t *problem, const int *x, double *y);

/**
 * @brief Evaluates the problem constraints in point x and save the result in y.
 */
void IOHprofiler_evaluate_constraint(IOHprofiler_problem_t *problem, const int *x, double *y);

/**
 * @brief Recommends a solution as the current best guesses to the problem.
 */
void IOHprofiler_recommend_solution(IOHprofiler_problem_t *problem, const int *x);

/**
 * @brief Frees the given problem.
 */
void IOHprofiler_problem_free(IOHprofiler_problem_t *problem);

void IOHprofiler_problem_set_parameters(IOHprofiler_problem_t *problem, const size_t number_of_parameters, const double *parameters);

/**
 * @brief Returns the name of the problem.
 */
const char *IOHprofiler_problem_get_name(const IOHprofiler_problem_t *problem);

/**
 * @brief Returns the ID of the problem.
 */
const char *IOHprofiler_problem_get_id(const IOHprofiler_problem_t *problem);

/**
 * @brief Returns the number of variables i.e. the dimension of the problem.
 */
size_t IOHprofiler_problem_get_dimension(const IOHprofiler_problem_t *problem);

/**
 * @brief Returns the number of objectives of the problem.
 */
size_t IOHprofiler_problem_get_number_of_objectives(const IOHprofiler_problem_t *problem);

/**
 * @brief Returns the number of evaluations done on the problem.
 */
size_t IOHprofiler_problem_get_evaluations(const IOHprofiler_problem_t *problem);

/**
 * @brief Returns 1 if the final target was hit, 0 otherwise.
 */
int IOHprofiler_problem_final_target_hit(const IOHprofiler_problem_t *problem);

/**
 * @brief Returns the best observed value for the first objective.
 */
double IOHprofiler_problem_get_best_observed_fvalue1(const IOHprofiler_problem_t *problem);

/**
 * @brief Returns the target value for the first objective.
 */
double depreciated_IOHprofiler_problem_get_final_target_fvalue1(const IOHprofiler_problem_t *problem);

/**
 * @brief Returns a vector of size 'dimension' with lower bounds of the region of interest in
 * the decision space.
 */
const int *IOHprofiler_problem_get_smallest_values_of_interest(const IOHprofiler_problem_t *problem);

/**
 * @brief Returns a vector of size 'dimension' with upper bounds of the region of interest in
 * the decision space.
 */
const int *IOHprofiler_problem_get_largest_values_of_interest(const IOHprofiler_problem_t *problem);

/**
 * @brief Returns the problem_index of the problem in its current suite.
 */
size_t IOHprofiler_problem_get_suite_dep_index(const IOHprofiler_problem_t *problem);

/**
 * @brief Returns an initial solution, i.e. a feasible variable setting, to the problem.
 */
void IOHprofiler_problem_get_initial_solution(const IOHprofiler_problem_t *problem, int *initial_solution);
/**@}*/

/***********************************************************************************************************/

/**
 * @name Methods regarding random numbers
 */
/**@{*/

/**
 * @brief Creates and returns a new random number state using the given seed.
 */
IOHprofiler_random_state_t *IOHprofiler_random_new(uint32_t seed);

/**
 * @brief Frees all memory associated with the random state.
 */
void IOHprofiler_random_free(IOHprofiler_random_state_t *state);

/**
 * @brief Returns one uniform [0, 1) random value from the random number generator associated with the given
 * state.
 */
double IOHprofiler_random_uniform(IOHprofiler_random_state_t *state);

/**
 * @brief Generates an approximately normal random number.
 */
double IOHprofiler_random_normal(IOHprofiler_random_state_t *state);
/**@}*/

/***********************************************************************************************************/

/**
 * @name Methods managing memory
 */
/**@{*/
/**
 * @brief Safe memory allocation that either succeeds or triggers a IOHprofiler_error.
 */
void *IOHprofiler_allocate_memory(const size_t size);

/**
 * @brief Safe memory allocation for a vector of doubles that either succeeds or triggers a IOHprofiler_error.
 */
double *IOHprofiler_allocate_vector(const size_t size);

/**
 * @brief Safe memory allocation for a vector of int that either succeeds or triggers a IOHprofiler_error.
 */
int *IOHprofiler_allocate_int_vector(const size_t size);

/**
 * @brief Frees the allocated memory.
 */
void IOHprofiler_free_memory(void *data);
/**@}*/

/***********************************************************************************************************/

/**
 * @name Methods regarding IOHprofiler messages
 */
/**@{*/
/**
 * @brief Signals a fatal error.
 */
void IOHprofiler_error(const char *message, ...);

/**
 * @brief Warns about error conditions.
 */
void IOHprofiler_warning(const char *message, ...);

/**
 * @brief Outputs some information.
 */
void IOHprofiler_info(const char *message, ...);

/**
 * @brief Prints only the given message without any prefix and new line.
 *
 * A function similar to IOHprofiler_info but producing no additional text than
 * the given message.
 *
 * The output is only produced if IOHprofiler_log_level >= IOHprofiler_INFO.
 */
void IOHprofiler_info_partial(const char *message, ...);

/**
 * @brief Outputs detailed information usually used for debugging.
 */
void IOHprofiler_debug(const char *message, ...);

/**
 * @brief Sets the IOHprofiler log level to the given value and returns the previous value of the log level.
 */
const char *IOHprofiler_set_log_level(const char *level);
/**@}*/

/***********************************************************************************************************/

/**
 * @brief Constructs a IOHprofiler archive.
 */
IOHprofiler_archive_t *IOHprofiler_archive(const char *suite_name,
                                           const size_t function,
                                           const size_t dimension,
                                           const size_t instance);
/**
 * @brief Adds a solution with objectives (y1, y2) to the archive if none of the existing solutions in the
 * archive dominates it. In this case, returns 1, otherwise the archive is not updated and the method
 * returns 0.
 */

int IOHprofiler_archive_add_solution(IOHprofiler_archive_t *archive, const double y1, const double y2, const char *text);

/**
 * @brief Returns the number of (non-dominated) solutions in the archive (computed first, if needed).
 */
size_t IOHprofiler_archive_get_number_of_solutions(IOHprofiler_archive_t *archive);

/**
 * @brief Returns the hypervolume of the archive (computed first, if needed).
 */
double IOHprofiler_archive_get_hypervolume(IOHprofiler_archive_t *archive);

/**
 * @brief Returns the text of the next (non-dominated) solution in the archive and "" when there are no
 * solutions left. The first two solutions are always the extreme ones.
 */
const char *IOHprofiler_archive_get_next_solution_text(IOHprofiler_archive_t *archive);

/**
 * @brief Frees the archive.
 */
void IOHprofiler_archive_free(IOHprofiler_archive_t *archive);

/***********************************************************************************************************/

/**
 * @name Other useful methods
 */
/**@{*/
/**
 * @brief Removes the given directory and all its contents.
 */
int IOHprofiler_remove_directory(const char *path);

/**
 * @brief Formatted string duplication.
 */
char *IOHprofiler_strdupf(const char *str, ...);
/**@}*/

/***********************************************************************************************************/

#ifdef __cplusplus
}
#endif
#endif
