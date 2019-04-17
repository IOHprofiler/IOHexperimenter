#include "IOHprofiler.h"

#line 12 "code-experiments/src/IOHprofiler_random.c"

#define IOHprofiler_NORMAL_POLAR /* Use polar transformation method */

#define IOHprofiler_SHORT_LAG 273
#define IOHprofiler_LONG_LAG 607

/**
 * @brief A structure containing the state of the IOHprofiler random generator.
 */
struct IOHprofiler_random_state_s {
    double x[IOHprofiler_LONG_LAG];
    size_t index;
};

/**
 * @brief A lagged Fibonacci random number generator.
 *
 * This generator is nice because it is reasonably small and directly generates double values. The chosen
 * lags (607 and 273) lead to a generator with a period in excess of 2^607-1.
 */
static void IOHprofiler_random_generate(IOHprofiler_random_state_t *state) {
    size_t i;
    for (i = 0; i < IOHprofiler_SHORT_LAG; ++i) {
        double t = state->x[i] + state->x[i + (IOHprofiler_LONG_LAG - IOHprofiler_SHORT_LAG)];
        if (t >= 1.0)
            t -= 1.0;
        state->x[i] = t;
    }
    for (i = IOHprofiler_SHORT_LAG; i < IOHprofiler_LONG_LAG; ++i) {
        double t = state->x[i] + state->x[i - IOHprofiler_SHORT_LAG];
        if (t >= 1.0)
            t -= 1.0;
        state->x[i] = t;
    }
    state->index = 0;
}

IOHprofiler_random_state_t *IOHprofiler_random_new(uint32_t seed) {
    IOHprofiler_random_state_t *state = (IOHprofiler_random_state_t *)IOHprofiler_allocate_memory(sizeof(*state));
    size_t i;
    /* Expand seed to fill initial state array. */
    for (i = 0; i < IOHprofiler_LONG_LAG; ++i) {
        /* Uses uint64_t to silence the compiler ("shift count negative or too big, undefined behavior" warning) */
        state->x[i] = ((double)seed) / (double)(((uint64_t)1UL << 32) - 1);
        /* Advance seed based on simple RNG from TAOCP */
        seed = (uint32_t)1812433253UL * (seed ^ (seed >> 30)) + ((uint32_t)i + 1);
    }
    state->index = 0;
    return state;
}

void IOHprofiler_random_free(IOHprofiler_random_state_t *state) {
    IOHprofiler_free_memory(state);
}

double IOHprofiler_random_uniform(IOHprofiler_random_state_t *state) {
    /* If we have consumed all random numbers in our archive, it is time to run the actual generator for one
   * iteration to refill the state with 'LONG_LAG' new values. */
    if (state->index >= IOHprofiler_LONG_LAG)
        IOHprofiler_random_generate(state);
    return state->x[state->index++];
}

/**
 * Instead of using the (expensive) polar method, we may cheat and abuse the central limit theorem. The sum
 * of 12 uniform random values has mean 6, variance 1 and is approximately normal. Subtract 6 and you get
 * an approximately N(0, 1) random number.
 */
double IOHprofiler_random_normal(IOHprofiler_random_state_t *state) {
    double normal;
#ifdef IOHprofiler_NORMAL_POLAR
    const double u1 = IOHprofiler_random_uniform(state);
    const double u2 = IOHprofiler_random_uniform(state);
    normal = sqrt(-2 * log(u1)) * cos(2 * IOHprofiler_pi * u2);
#else
    int i;
    normal = 0.0;
    for (i = 0; i < 12; ++i) {
        normal += IOHprofiler_random_uniform(state);
    }
    normal -= 6.0;
#endif
    return normal;
}

/* Be hygienic (for amalgamation) and undef lags. */
#undef IOHprofiler_SHORT_LAG
#undef IOHprofiler_LONG_LAG
#line 1 "code-experiments/src/IOHprofiler_suite.c"
/**
 * @file IOHprofiler_suite.c
 * @brief Definitions of functions regarding IOHprofiler suites.
 *
 * When a new suite is added, the functions IOHprofiler_suite_initialize, IOHprofiler_suite_get_instances_by_year and
 * IOHprofiler_suite_get_problem_from_indices need to be updated.
 *
 */

#include <time.h>
#line 12 "code-experiments/src/IOHprofiler_suite.c"

#line 13 "code-experiments/src/IOHprofiler_suite.c"
#line 1 "code-experiments/src/IOHprofiler_utilities.c"
/**
 * @file IOHprofiler_utilities.c
 * @brief Definitions of miscellaneous functions used throughout the IOHprofiler framework.
 */


#line 7 "code-experiments/src/IOHprofiler_utilities.c"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#line 16 "code-experiments/src/IOHprofiler_utilities.c"
#line 17 "code-experiments/src/IOHprofiler_utilities.c"
#line 1 "code-experiments/src/IOHprofiler_string.c"
/**
 * @file IOHprofiler_string.c
 * @brief Definitions of functions that manipulate strings.
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#line 11 "code-experiments/src/IOHprofiler_string.c"

static size_t *IOHprofiler_allocate_vector_size_t(const size_t number_of_elements);

/**
 * @brief Creates a duplicate copy of string and returns a pointer to it.
 *
 * The caller is responsible for freeing the allocated memory using IOHprofiler_free_memory().
 */
static char *IOHprofiler_strdup(const char *string) {
    size_t len;
    char *duplicate;
    if (string == NULL)
        return NULL;
    len = strlen(string);
    duplicate = (char *)IOHprofiler_allocate_memory(len + 1);
    memcpy(duplicate, string, len + 1);
    return duplicate;
}

/**
 * @brief The length of the buffer used in the IOHprofiler_vstrdupf function.
 *
 * @note This should be handled differently!
 */
#define IOHprofiler_VSTRDUPF_BUFLEN 444

/**
 * @brief Formatted string duplication, with va_list arguments.
 */
static char *IOHprofiler_vstrdupf(const char *str, va_list args) {
    static char buf[IOHprofiler_VSTRDUPF_BUFLEN];
    long written;
    /* apparently args can only be used once, therefore
   * len = vsnprintf(NULL, 0, str, args) to find out the
   * length does not work. Therefore we use a buffer
   * which limits the max length. Longer strings should
   * never appear anyway, so this is rather a non-issue. */

#if 0
  written = vsnprintf(buf, IOHprofiler_VSTRDUPF_BUFLEN - 2, str, args);
  if (written < 0)
  IOHprofiler_error("IOHprofiler_vstrdupf(): vsnprintf failed on '%s'", str);
#else /* less safe alternative, if vsnprintf is not available */
    assert(strlen(str) < IOHprofiler_VSTRDUPF_BUFLEN / 2 - 2);
    if (strlen(str) >= IOHprofiler_VSTRDUPF_BUFLEN / 2 - 2)
        IOHprofiler_error("IOHprofiler_vstrdupf(): string is too long");
    written = vsprintf(buf, str, args);
    if (written < 0)
        IOHprofiler_error("IOHprofiler_vstrdupf(): vsprintf failed on '%s'", str);
#endif
    if (written > IOHprofiler_VSTRDUPF_BUFLEN - 3)
        IOHprofiler_error("IOHprofiler_vstrdupf(): A suspiciously long string is tried to being duplicated '%s'", buf);
    return IOHprofiler_strdup(buf);
}

#undef IOHprofiler_VSTRDUPF_BUFLEN

/**
 * Optional arguments are used like in sprintf.
 */
char *IOHprofiler_strdupf(const char *str, ...) {
    va_list args;
    char *s;

    va_start(args, str);
    s = IOHprofiler_vstrdupf(str, args);
    va_end(args);
    return s;
}

/**
 * @brief Returns a concatenate copy of string1 + string2.
 *
 * The caller is responsible for freeing the allocated memory using IOHprofiler_free_memory().
 */
static char *IOHprofiler_strconcat(const char *s1, const char *s2) {
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char *s = (char *)IOHprofiler_allocate_memory(len1 + len2 + 1);

    memcpy(s, s1, len1);
    memcpy(&s[len1], s2, len2 + 1);
    return s;
}

/**
 * @brief Returns the first index where seq occurs in base and -1 if it doesn't.
 *
 * @note If there is an equivalent standard C function, this can/should be removed.
 */
static long IOHprofiler_strfind(const char *base, const char *seq) {
    const size_t strlen_seq = strlen(seq);
    const size_t last_first_idx = strlen(base) - strlen(seq);
    size_t i, j;

    if (strlen(base) < strlen(seq))
        return -1;

    for (i = 0; i <= last_first_idx; ++i) {
        if (base[i] == seq[0]) {
            for (j = 0; j < strlen_seq; ++j) {
                if (base[i + j] != seq[j])
                    break;
            }
            if (j == strlen_seq) {
                if (i > 1e9)
                    IOHprofiler_error("IOHprofiler_strfind(): strange values observed i=%lu, j=%lu, strlen(base)=%lu",
                                      (unsigned long)i, (unsigned long)j, (unsigned long)strlen(base));
                return (long)i;
            }
        }
    }
    return -1;
}

/**
 * @brief Splits a string based on the given delimiter.
 *
 * Returns a pointer to the resulting substrings with NULL as the last one.
 * The caller is responsible for freeing the allocated memory using IOHprofiler_free_memory().
 */
static char **IOHprofiler_string_split(const char *string, const char delimiter) {
    char **result;
    char *str_copy, *ptr, *token;
    char str_delimiter[2];
    size_t i;
    size_t count = 1;

    str_copy = IOHprofiler_strdup(string);

    /* Counts the parts between delimiters */
    ptr = str_copy;
    while (*ptr != '\0') {
        if (*ptr == delimiter) {
            count++;
        }
        ptr++;
    }
    /* Makes room for an empty string that will be appended at the end */
    count++;

    result = (char **)IOHprofiler_allocate_memory(count * sizeof(char *));

    /* Iterates through tokens
   * NOTE: strtok() ignores multiple delimiters, therefore the final number of detected substrings might be
   * lower than the count. This is OK. */
    i = 0;
    /* A char* delimiter needs to be used, otherwise strtok() can surprise */
    str_delimiter[0] = delimiter;
    str_delimiter[1] = '\0';
    token = strtok(str_copy, str_delimiter);
    while (token) {
        assert(i < count);
        *(result + i++) = IOHprofiler_strdup(token);
        token = strtok(NULL, str_delimiter);
    }
    *(result + i) = NULL;

    IOHprofiler_free_memory(str_copy);

    return result;
}

/**
 * @brief Creates and returns a string with removed characters between from and to.
 *
 * If you wish to remove characters from the beginning of the string, set from to "".
 * If you wish to remove characters until the end of the string, set to to "".
 *
 * The caller is responsible for freeing the allocated memory using IOHprofiler_free_memory().
 */
static char *IOHprofiler_remove_from_string(const char *string, const char *from, const char *to) {
    char *result, *start, *stop;

    result = IOHprofiler_strdup(string);

    if (strcmp(from, "") == 0) {
        /* Remove from the start */
        start = result;
    } else
        start = strstr(result, from);

    if (strcmp(to, "") == 0) {
        /* Remove until the end */
        stop = result + strlen(result);
    } else
        stop = strstr(result, to);

    if ((start == NULL) || (stop == NULL) || (stop < start)) {
        IOHprofiler_error("IOHprofiler_remove_from_string(): failed to remove characters between %s and %s from string %s",
                          from, to, string);
        return NULL; /* Never reached */
    }

    memmove(start, stop, strlen(stop) + 1);

    return result;
}

/**
 * @brief Returns the numbers defined by the ranges.
 *
 * Reads ranges from a string of positive ranges separated by commas. For example: "-3,5-6,8-". Returns the
 * numbers that are defined by the ranges if min and max are used as their extremes. If the ranges with open
 * beginning/end are not allowed, use 0 as min/max. The returned string has an appended 0 to mark its end.
 * A maximum of max_count values is returned. If there is a problem with one of the ranges, the parsing stops
 * and the current result is returned. The memory of the returned object needs to be freed by the caller.
 */
static size_t *IOHprofiler_string_parse_ranges(const char *string,
                                               const size_t min,
                                               const size_t max,
                                               const char *name,
                                               const size_t max_count) {
    char *ptr, *dash = NULL;
    char **ranges, **numbers;
    size_t i, j, count;
    size_t num[2];

    size_t *result;
    size_t i_result = 0;

    char *str = IOHprofiler_strdup(string);

    /* Check for empty string */
    if ((str == NULL) || (strlen(str) == 0)) {
        IOHprofiler_warning("IOHprofiler_string_parse_ranges(): cannot parse empty ranges");
        IOHprofiler_free_memory(str);
        return NULL;
    }

    ptr = str;
    /* Check for disallowed characters */
    while (*ptr != '\0') {
        if ((*ptr != '-') && (*ptr != ',') && !isdigit((unsigned char)*ptr)) {
            IOHprofiler_warning("IOHprofiler_string_parse_ranges(): problem parsing '%s' - cannot parse ranges with '%c'", str,
                                *ptr);
            IOHprofiler_free_memory(str);
            return NULL;
        } else
            ptr++;
    }

    /* Check for incorrect boundaries */
    if ((max > 0) && (min > max)) {
        IOHprofiler_warning("IOHprofiler_string_parse_ranges(): incorrect boundaries");
        IOHprofiler_free_memory(str);
        return NULL;
    }

    result = IOHprofiler_allocate_vector_size_t(max_count + 1);

    /* Split string to ranges w.r.t commas */
    ranges = IOHprofiler_string_split(str, ',');
    IOHprofiler_free_memory(str);

    if (ranges) {
        /* Go over the current range */
        for (i = 0; *(ranges + i); i++) {
            ptr = *(ranges + i);
            /* Count the number of '-' */
            count = 0;
            while (*ptr != '\0') {
                if (*ptr == '-') {
                    if (count == 0)
                        /* Remember the position of the first '-' */
                        dash = ptr;
                    count++;
                }
                ptr++;
            }
            /* Point again to the start of the range */
            ptr = *(ranges + i);

            /* Check for incorrect number of '-' */
            if (count > 1) {
                IOHprofiler_warning("IOHprofiler_string_parse_ranges(): problem parsing '%s' - too many '-'s", string);
                /* Cleanup */
                for (j = i; *(ranges + j); j++)
                    IOHprofiler_free_memory(*(ranges + j));
                IOHprofiler_free_memory(ranges);
                if (i_result == 0) {
                    IOHprofiler_free_memory(result);
                    return NULL;
                }
                result[i_result] = 0;
                return result;
            } else if (count == 0) {
                /* Range is in the format: n (no range) */
                num[0] = (size_t)strtol(ptr, NULL, 10);
                num[1] = num[0];
            } else {
                /* Range is in one of the following formats: n-m / -n / n- / - */

                /* Split current range to numbers w.r.t '-' */
                numbers = IOHprofiler_string_split(ptr, '-');
                j = 0;
                if (numbers) {
                    /* Read the numbers */
                    for (j = 0; *(numbers + j); j++) {
                        assert(j < 2);
                        num[j] = (size_t)strtol(*(numbers + j), NULL, 10);
                        IOHprofiler_free_memory(*(numbers + j));
                    }
                }
                IOHprofiler_free_memory(numbers);

                if (j == 0) {
                    /* Range is in the format - (open ends) */
                    if ((min == 0) || (max == 0)) {
                        IOHprofiler_warning("IOHprofiler_string_parse_ranges(): '%s' ranges cannot have an open ends; some ranges ignored", name);
                        /* Cleanup */
                        for (j = i; *(ranges + j); j++)
                            IOHprofiler_free_memory(*(ranges + j));
                        IOHprofiler_free_memory(ranges);
                        if (i_result == 0) {
                            IOHprofiler_free_memory(result);
                            return NULL;
                        }
                        result[i_result] = 0;
                        return result;
                    }
                    num[0] = min;
                    num[1] = max;
                } else if (j == 1) {
                    if (dash - *(ranges + i) == 0) {
                        /* Range is in the format -n */
                        if (min == 0) {
                            IOHprofiler_warning("IOHprofiler_string_parse_ranges(): '%s' ranges cannot have an open beginning; some ranges ignored", name);
                            /* Cleanup */
                            for (j = i; *(ranges + j); j++)
                                IOHprofiler_free_memory(*(ranges + j));
                            IOHprofiler_free_memory(ranges);
                            if (i_result == 0) {
                                IOHprofiler_free_memory(result);
                                return NULL;
                            }
                            result[i_result] = 0;
                            return result;
                        }
                        num[1] = num[0];
                        num[0] = min;
                    } else {
                        /* Range is in the format n- */
                        if (max == 0) {
                            IOHprofiler_warning("IOHprofiler_string_parse_ranges(): '%s' ranges cannot have an open end; some ranges ignored", name);
                            /* Cleanup */
                            for (j = i; *(ranges + j); j++)
                                IOHprofiler_free_memory(*(ranges + j));
                            IOHprofiler_free_memory(ranges);
                            if (i_result == 0) {
                                IOHprofiler_free_memory(result);
                                return NULL;
                            }
                            result[i_result] = 0;
                            return result;
                        }
                        num[1] = max;
                    }
                }
                /* if (j == 2), range is in the format n-m and there is nothing to do */
            }

            /* Make sure the boundaries are taken into account */
            if ((min > 0) && (num[0] < min)) {
                num[0] = min;
                IOHprofiler_warning("IOHprofiler_string_parse_ranges(): '%s' ranges adjusted to be >= %lu", name,
                                    (unsigned long)min);
              }
            if ((max > 0) && (num[1] > max)) {
                num[1] = max;
                IOHprofiler_warning("IOHprofiler_string_parse_ranges(): '%s' ranges adjusted to be <= %lu", name, (unsigned long) max);
            }
            if (num[0] > num[1]) {
                IOHprofiler_warning("IOHprofiler_string_parse_ranges(): '%s' ranges not within boundaries; some ranges ignored", name);
                /* Cleanup */
                for (j = i; *(ranges + j); j++)
                    IOHprofiler_free_memory(*(ranges + j));
                IOHprofiler_free_memory(ranges);
                if (i_result == 0) {
                    IOHprofiler_free_memory(result);
                    return NULL;
                }
                result[i_result] = 0;
                return result;
            }

            /* Write in result */
            for (j = num[0]; j <= num[1]; j++) {
                if (i_result > max_count - 1)
                    break;
                result[i_result++] = j;
            }

            IOHprofiler_free_memory(*(ranges + i));
            *(ranges + i) = NULL;
        }
    }

    IOHprofiler_free_memory(ranges);

    if (i_result == 0) {
        IOHprofiler_free_memory(result);
        return NULL;
    }

    result[i_result] = 0;
    return result;
}

/**
 * @brief Trims the given string (removes any leading and trailing spaces).
 *
 * If the string contains any leading spaces, the contents are shifted so that if it was dynamically
 * allocated, it can be still freed on the returned pointer.
 */
static char *IOHprofiler_string_trim(char *string) {
    size_t len = 0;
    char *frontp = string;
    char *endp = NULL;

    if (string == NULL) {
        return NULL;
    }
    if (string[0] == '\0') {
        return string;
    }

    len = strlen(string);
    endp = string + len;

    /* Move the front and back pointers to address the first non-whitespace characters from each end. */
    while (isspace((unsigned char)*frontp)) {
        ++frontp;
    }
    if (endp != frontp) {
        while (isspace((unsigned char)*(--endp)) && endp != frontp) {
        }
    }

    if (string + len - 1 != endp)
        *(endp + 1) = '\0';
    else if (frontp != string && endp == frontp)
        *string = '\0';

    /* Shift the string. Note the reuse of endp to mean the front of the string buffer now. */
    endp = string;
    if (frontp != string) {
        while (*frontp) {
            *endp++ = *frontp++;
        }
        *endp = '\0';
    }

    return string;
}
#line 18 "code-experiments/src/IOHprofiler_utilities.c"

/***********************************************************************************************************/

/**
 * @brief Initializes the logging level to IOHprofiler_INFO.
 */
static IOHprofiler_log_level_type_e IOHprofiler_log_level = IOHprofiler_ERROR;

/**
 * @param log_level Denotes the level of information given to the user through the standard output and
 * error streams. Can take on the values:
 * - "error" (only error messages are output),
 * - "warning" (only error and warning messages are output),
 * - "info" (only error, warning and info messages are output) and
 * - "debug" (all messages are output).
 * - "" does not set a new value
 * The default value is info.
 *
 * @return The previous IOHprofiler_log_level value as an immutable string.
 */
const char *IOHprofiler_set_log_level(const char *log_level) {
    IOHprofiler_log_level_type_e previous_log_level = IOHprofiler_log_level;

    if (strcmp(log_level, "error") == 0)
        IOHprofiler_log_level = IOHprofiler_ERROR;
    else if (strcmp(log_level, "warning") == 0)
        IOHprofiler_log_level = IOHprofiler_WARNING;
    else if (strcmp(log_level, "info") == 0)
        IOHprofiler_log_level = IOHprofiler_INFO;
    else if (strcmp(log_level, "debug") == 0)
        IOHprofiler_log_level = IOHprofiler_DEBUG;
    else if (strcmp(log_level, "") == 0) {
        /* Do nothing */
    } else {
        IOHprofiler_warning("IOHprofiler_set_log_level(): unknown level %s", log_level);
    }

    if (previous_log_level == IOHprofiler_ERROR)
        return "error";
    else if (previous_log_level == IOHprofiler_WARNING)
        return "warning";
    else if (previous_log_level == IOHprofiler_INFO)
        return "info";
    else if (previous_log_level == IOHprofiler_DEBUG)
        return "debug";
    else {
        IOHprofiler_error("IOHprofiler_set_log_level(): unknown previous log level");
        return "";
    }
}

/***********************************************************************************************************/

/**
 * @name Methods regarding file, directory and path manipulations
 */
/**@{*/
/**
 * @brief Creates a platform-dependent path from the given strings.
 *
 * @note The last argument must be NULL.
 * @note The first parameter must be able to accommodate path_max_length characters and the length
 * of the joined path must not exceed path_max_length characters.
 * @note Should work cross-platform.
 *
 * Usage examples:
 * - IOHprofiler_join_path(base_path, 100, folder1, folder2, folder3, NULL) creates base_path/folder1/folder2/folder3
 * - IOHprofiler_join_path(base_path, 100, folder1, file_name, NULL) creates base_path/folder1/file_name
 * @param path The base path; it's also where the joined path is stored to.
 * @param path_max_length The maximum length of the path.
 * @param ... Additional strings, must end with NULL
 */
static void IOHprofiler_join_path(char *path, const size_t path_max_length, ...) {
    const size_t path_separator_length = strlen(IOHprofiler_path_separator);
    va_list args;
    char *path_component;
    size_t path_length = strlen(path);

    va_start(args, path_max_length);
    while (NULL != (path_component = va_arg(args, char *))) {
        size_t component_length = strlen(path_component);
        if (path_length + path_separator_length + component_length >= path_max_length) {
            IOHprofiler_error("IOHprofiler_join_path() failed because the ${path} is too short.");
            return; /* never reached */
        }
        /* Both should be safe because of the above check. */
        if (strlen(path) > 0)
            strncat(path, IOHprofiler_path_separator, path_max_length - strlen(path) - 1);
        strncat(path, path_component, path_max_length - strlen(path) - 1);
    }
    va_end(args);
}

/**
 * @brief Checks if the given directory exists.
 *
 * @note Should work cross-platform.
 *
 * @param path The given path.
 *
 * @return 1 if the path exists and corresponds to a directory and 0 otherwise.
 */
static int IOHprofiler_directory_exists(const char *path) {
    int res;
#if defined(HAVE_GFA)
    DWORD dwAttrib = GetFileAttributesA(path);
    res = (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#elif defined(HAVE_STAT)
    struct stat buf;
    res = (!stat(path, &buf) && S_ISDIR(buf.st_mode));
#else
#error Ooops
#endif
    return res;
}

/**
 * @brief Checks if the given file exists.
 *
 * @note Should work cross-platform.
 *
 * @param path The given path.
 *
 * @return 1 if the path exists and corresponds to a file and 0 otherwise.
 */
static int IOHprofiler_file_exists(const char *path) {
    int res;
#if defined(HAVE_GFA)
    DWORD dwAttrib = GetFileAttributesA(path);
    res = (dwAttrib != INVALID_FILE_ATTRIBUTES) && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
#elif defined(HAVE_STAT)
    struct stat buf;
    res = (!stat(path, &buf) && !S_ISDIR(buf.st_mode));
#else
#error Ooops
#endif
    return res;
}

/**
 * @brief Calls the right mkdir() method (depending on the platform).
 *
 * @param path The directory path.
 *
 * @return 0 on successful completion, and -1 on error.
 */
static int IOHprofiler_mkdir(const char *path) {
#if _MSC_VER
    return _mkdir(path);
#elif defined(__MINGW32__) || defined(__MINGW64__)
    return mkdir(path);
#else
    return mkdir(path, S_IRWXU);
#endif
}

/**
 * @brief Creates a directory with full privileges for the user.
 *
 * @note Should work cross-platform.
 *
 * @param path The directory path.
 */
static void IOHprofiler_create_directory(const char *path) {
    char *tmp = NULL;
    char *p;
    size_t len = strlen(path);
    char path_sep = IOHprofiler_path_separator[0];

    /* Nothing to do if the path exists. */
    if (IOHprofiler_directory_exists(path))
        return;

    tmp = IOHprofiler_strdup(path);
    /* Remove possible trailing slash */
    if (tmp[len - 1] == path_sep)
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++) {
        if (*p == path_sep) {
            *p = 0;
            if (!IOHprofiler_directory_exists(tmp)) {
                if (0 != IOHprofiler_mkdir(tmp))
                    IOHprofiler_error("IOHprofiler_create_path(): failed creating %s", tmp);
            }
            *p = path_sep;
        }
    }
    if (0 != IOHprofiler_mkdir(tmp))
        IOHprofiler_error("IOHprofiler_create_path(): failed creating %s", tmp);
    IOHprofiler_free_memory(tmp);
    return;
}

/* Commented to silence the compiler (unused function warning) */
#if 0

#endif

/**
 * @brief Creates a unique directory from the given path.
 *
 * If the given path does not yet exit, it is left as is, otherwise it is changed(!) by appending a number
 * to it. If path already exists, path-01 will be tried. If this one exists as well, path-02 will be tried,
 * and so on. If path-99 exists as well, the function throws an error.
 */
static void IOHprofiler_create_unique_directory(char **path) {
    int counter = 1;
    char *new_path;

    /* Create the path if it does not yet exist */
    if (!IOHprofiler_directory_exists(*path)) {
        IOHprofiler_create_directory(*path);
        return;
    }

    while (counter < 999) {
        new_path = IOHprofiler_strdupf("%s-%03d", *path, counter);

        if (!IOHprofiler_directory_exists(new_path)) {
            IOHprofiler_free_memory(*path);
            *path = new_path;
            IOHprofiler_create_directory(*path);
            return;
        } else {
            counter++;
            IOHprofiler_free_memory(new_path);
        }
    }

    IOHprofiler_error("IOHprofiler_create_unique_path(): could not create a unique path with name %s", *path);
    return; /* Never reached */
}

/**
 * The method should work across different platforms/compilers.
 *
 * @path The path to the directory
 *
 * @return 0 on successful completion, and -1 on error.
 */
int IOHprofiler_remove_directory(const char *path) {
#if _MSC_VER
    WIN32_FIND_DATA find_data_file;
    HANDLE find_handle = NULL;
    char *buf;
    int r = -1;
    int r2 = -1;

    buf = IOHprofiler_strdupf("%s\\*.*", path);
    /* Nothing to do if the folder does not exist */
    if ((find_handle = FindFirstFile(buf, &find_data_file)) == INVALID_HANDLE_VALUE) {
        IOHprofiler_free_memory(buf);
        return 0;
    }
    IOHprofiler_free_memory(buf);

    do {
        r = 0;

        /* Skip the names "." and ".." as we don't want to recurse on them */
        if (strcmp(find_data_file.cFileName, ".") != 0 && strcmp(find_data_file.cFileName, "..") != 0) {
            /* Build the new path using the argument path the file/folder name we just found */
            buf = IOHprofiler_strdupf("%s\\%s", path, find_data_file.cFileName);

            if (find_data_file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                /* Buf is a directory, recurse on it */
                r2 = IOHprofiler_remove_directory(buf);
            } else {
                /* Buf is a file, delete it */
                /* Careful, DeleteFile returns 0 if it fails and nonzero otherwise! */
                r2 = -(DeleteFile(buf) == 0);
            }

            IOHprofiler_free_memory(buf);
        }

        r = r2;

    } while (FindNextFile(find_handle, &find_data_file)); /* Find the next file */

    FindClose(find_handle);

    if (!r) {
        /* Path is an empty directory, delete it */
        /* Careful, RemoveDirectory returns 0 if it fails and nonzero otherwise! */
        r = -(RemoveDirectory(path) == 0);
    }

    return r;
#else
    DIR *d = opendir(path);
    int r = -1;
    int r2 = -1;
    char *buf;

    /* Nothing to do if the folder does not exist */
    if (!IOHprofiler_directory_exists(path))
        return 0;

    if (d) {
        struct dirent *p;

        r = 0;

        while (!r && (p = readdir(d))) {
            /* Skip the names "." and ".." as we don't want to recurse on them */
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
                continue;
            }

            buf = IOHprofiler_strdupf("%s/%s", path, p->d_name);
            if (buf) {
                if (IOHprofiler_directory_exists(buf)) {
                    /* Buf is a directory, recurse on it */
                    r2 = IOHprofiler_remove_directory(buf);
                } else {
                    /* Buf is a file, delete it */
                    r2 = unlink(buf);
                }
            }
            IOHprofiler_free_memory(buf);

            r = r2;
        }

        closedir(d);
    }

    if (!r) {
        /* Path is an empty directory, delete it */
        r = rmdir(path);
    }

    return r;
#endif
}
/**@}*/

/***********************************************************************************************************/

/**
 * @name Methods regarding memory allocations
 */
/**@{*/
double *IOHprofiler_allocate_vector(const size_t number_of_elements) {
    const size_t block_size = number_of_elements * sizeof(double);
    return (double *)IOHprofiler_allocate_memory(block_size);
}

int *IOHprofiler_allocate_int_vector(const size_t number_of_elements) {
    const size_t block_size = number_of_elements * sizeof(int);
    return (int *)IOHprofiler_allocate_memory(block_size);
}
/**
 * @brief Allocates memory for a vector and sets all its elements to value.
 */
static double *IOHprofiler_allocate_vector_with_value(const size_t number_of_elements, double value) {
    const size_t block_size = number_of_elements * sizeof(double);
    double *vector = (double *)IOHprofiler_allocate_memory(block_size);
    size_t i;

    for (i = 0; i < number_of_elements; i++)
        vector[i] = value;

    return vector;
}

/**
 * @brief Safe memory allocation for a vector with size_t elements that either succeeds or triggers a
 * IOHprofiler_error.
 */
static size_t *IOHprofiler_allocate_vector_size_t(const size_t number_of_elements) {
    const size_t block_size = number_of_elements * sizeof(size_t);
    return (size_t *)IOHprofiler_allocate_memory(block_size);
}

static char *IOHprofiler_allocate_string(const size_t number_of_elements) {
    const size_t block_size = number_of_elements * sizeof(char);
    return (char *)IOHprofiler_allocate_memory(block_size);
}

static double *IOHprofiler_duplicate_vector(const double *src, const size_t number_of_elements) {
    size_t i;
    double *dst;

    assert(src != NULL);
    assert(number_of_elements > 0);

    dst = IOHprofiler_allocate_vector(number_of_elements);
    for (i = 0; i < number_of_elements; ++i) {
        dst[i] = src[i];
    }
    return dst;
}

static int *IOHprofiler_duplicate_int_vector(const int *src, const size_t number_of_elements) {
    size_t i;
    int *dst;

    assert(src != NULL);
    assert(number_of_elements > 0);

    dst = IOHprofiler_allocate_int_vector(number_of_elements);
    for (i = 0; i < number_of_elements; ++i) {
        dst[i] = src[i];
    }
    return dst;
}
/**@}*/

/***********************************************************************************************************/

/**
 * @name Methods regarding string options
 */
/**@{*/

/**
 * @brief Allocates an option keys structure holding the given number of option keys.
 */
static IOHprofiler_option_keys_t *IOHprofiler_option_keys_allocate(const size_t count, const char **keys) {
    size_t i;
    IOHprofiler_option_keys_t *option_keys;

    if ((count == 0) || (keys == NULL))
        return NULL;

    option_keys = (IOHprofiler_option_keys_t *)IOHprofiler_allocate_memory(sizeof(*option_keys));

    option_keys->keys = (char **)IOHprofiler_allocate_memory(count * sizeof(char *));
    for (i = 0; i < count; i++) {
        assert(keys[i]);
        option_keys->keys[i] = IOHprofiler_strdup(keys[i]);
    }
    option_keys->count = count;

    return option_keys;
}

/**
 * @brief Frees the given option keys structure.
 */
static void IOHprofiler_option_keys_free(IOHprofiler_option_keys_t *option_keys) {
    size_t i;

    if (option_keys) {
        for (i = 0; i < option_keys->count; i++) {
            IOHprofiler_free_memory(option_keys->keys[i]);
        }
        IOHprofiler_free_memory(option_keys->keys);
        IOHprofiler_free_memory(option_keys);
    }
}

/**
 * @brief Returns redundant option keys (the ones present in given_option_keys but not in known_option_keys).
 */
static IOHprofiler_option_keys_t *IOHprofiler_option_keys_get_redundant(const IOHprofiler_option_keys_t *known_option_keys,
                                                                        const IOHprofiler_option_keys_t *given_option_keys) {
    size_t i, j, count = 0;
    int found;
    char **redundant_keys;
    IOHprofiler_option_keys_t *redundant_option_keys;

    assert(known_option_keys != NULL);
    assert(given_option_keys != NULL);

    /* Find the redundant keys */
    redundant_keys = (char **)IOHprofiler_allocate_memory(given_option_keys->count * sizeof(char *));
    for (i = 0; i < given_option_keys->count; i++) {
        found = 0;
        for (j = 0; j < known_option_keys->count; j++) {
            if (strcmp(given_option_keys->keys[i], known_option_keys->keys[j]) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            redundant_keys[count++] = IOHprofiler_strdup(given_option_keys->keys[i]);
        }
    }
    redundant_option_keys = IOHprofiler_option_keys_allocate(count, (const char **)redundant_keys);

    /* Free memory */
    for (i = 0; i < count; i++) {
        IOHprofiler_free_memory(redundant_keys[i]);
    }
    IOHprofiler_free_memory(redundant_keys);

    return redundant_option_keys;
}

/**
 * @brief Adds additional option keys to the given basic option keys (changes the basic keys).
 */
static void IOHprofiler_option_keys_add(IOHprofiler_option_keys_t **basic_option_keys,
                                        const IOHprofiler_option_keys_t *additional_option_keys) {
    size_t i, j;
    size_t new_count;
    char **new_keys;
    IOHprofiler_option_keys_t *new_option_keys;

    assert(*basic_option_keys != NULL);
    if (additional_option_keys == NULL)
        return;

    /* Construct the union of both keys */
    new_count = (*basic_option_keys)->count + additional_option_keys->count;
    new_keys = (char **)IOHprofiler_allocate_memory(new_count * sizeof(char *));
    for (i = 0; i < (*basic_option_keys)->count; i++) {
        new_keys[i] = IOHprofiler_strdup((*basic_option_keys)->keys[i]);
    }
    for (j = 0; j < additional_option_keys->count; j++) {
        new_keys[(*basic_option_keys)->count + j] = IOHprofiler_strdup(additional_option_keys->keys[j]);
    }
    new_option_keys = IOHprofiler_option_keys_allocate(new_count, (const char **)new_keys);

    /* Free the old basic keys */
    IOHprofiler_option_keys_free(*basic_option_keys);
    *basic_option_keys = new_option_keys;
    for (i = 0; i < new_count; i++) {
        IOHprofiler_free_memory(new_keys[i]);
    }
    IOHprofiler_free_memory(new_keys);
}

/**
 * @brief Creates an instance of option keys from the given string of options containing keys and values
 * separated by colons.
 *
 * @note Relies heavily on the "key: value" format and might fail if the number of colons doesn't match the
 * number of keys.
 */
static IOHprofiler_option_keys_t *IOHprofiler_option_keys(const char *option_string) {
    size_t i;
    char **keys;
    IOHprofiler_option_keys_t *option_keys = NULL;
    char *string_to_parse, *key;

    /* Check for empty string */
    if ((option_string == NULL) || (strlen(option_string) == 0)) {
        return NULL;
    }

    /* Split the options w.r.t ':' */
    keys = IOHprofiler_string_split(option_string, ':');

    if (keys) {
        /* Keys now contain something like this: "values_of_previous_key this_key" except for the first, which
     * contains only the key and the last, which contains only the previous values */
        for (i = 0; *(keys + i); i++) {
            string_to_parse = IOHprofiler_strdup(*(keys + i));

            /* Remove any leading and trailing spaces */
            string_to_parse = IOHprofiler_string_trim(string_to_parse);

            /* Stop if this is the last substring (contains a value and no key) */
            if ((i > 0) && (*(keys + i + 1) == NULL)) {
                IOHprofiler_free_memory(string_to_parse);
                break;
            }

            /* Disregard everything before the last space */
            key = strrchr(string_to_parse, ' ');
            if ((key == NULL) || (i == 0)) {
                /* No spaces left (or this is the first key), everything is the key */
                key = string_to_parse;
            } else {
                /* Move to the start of the key (one char after the space) */
                key++;
            }

            /* Put the key in keys */
            IOHprofiler_free_memory(*(keys + i));
            *(keys + i) = IOHprofiler_strdup(key);
            IOHprofiler_free_memory(string_to_parse);
        }

        option_keys = IOHprofiler_option_keys_allocate(i, (const char **)keys);

        /* Free the keys */
        for (i = 0; *(keys + i); i++) {
            IOHprofiler_free_memory(*(keys + i));
        }
        IOHprofiler_free_memory(keys);
    }

    return option_keys;
}

/**
 * @brief Creates and returns a string containing the info_string and all keys from option_keys.
 *
 * Can be used to output information about the given option_keys.
 */
static char *IOHprofiler_option_keys_get_output_string(const IOHprofiler_option_keys_t *option_keys,
                                                       const char *info_string) {
    size_t i;
    char *string = NULL, *new_string;

    if ((option_keys != NULL) && (option_keys->count > 0)) {
        string = IOHprofiler_strdup(info_string);
        for (i = 0; i < option_keys->count; i++) {
            new_string = IOHprofiler_strdupf("%s %s\n", string, option_keys->keys[i]);
            IOHprofiler_free_memory(string);
            string = new_string;
        }
    }

    return string;
}

/**
 * @brief Parses options in the form "name1: value1 name2: value2".
 *
 * Formatting requirements:
 * - name and value need to be separated by a colon (spaces are optional)
 * - value needs to be a single string (no spaces allowed)
 *
 * @return The number of successful assignments.
 */
static int IOHprofiler_options_read(const char *options, const char *name, const char *format, void *pointer) {
    long i1, i2;

    if ((!options) || (strlen(options) == 0))
        return 0;

    i1 = IOHprofiler_strfind(options, name);
    if (i1 < 0)
        return 0;
    i2 = i1 + IOHprofiler_strfind(&options[i1], ":") + 1;

    /* Remove trailing whitespaces */
    while (isspace((unsigned char)options[i2]))
        i2++;

    if (i2 <= i1) {
        return 0;
    }

    return sscanf(&options[i2], format, pointer);
}

/**
 * @brief Reads an integer from options using the form "name1: value1 name2: value2".
 *
 * Formatting requirements:
 * - name and value need to be separated by a colon (spaces are optional)
 * - the value corresponding to the given name needs to be an integer
 *
 * @return The number of successful assignments.
 */
static int IOHprofiler_options_read_int(const char *options, const char *name, int *pointer) {
    return IOHprofiler_options_read(options, name, " %i", pointer);
}

/**
 * @brief Reads a size_t from options using the form "name1: value1 name2: value2".
 *
 * Formatting requirements:
 * - name and value need to be separated by a colon (spaces are optional)
 * - the value corresponding to the given name needs to be a size_t
 *
 * @return The number of successful assignments.
 */
static int IOHprofiler_options_read_size_t(const char *options, const char *name, size_t *pointer) {
    return IOHprofiler_options_read(options, name, "%lu", pointer);
}

/**
 * @brief Reads a double value from options using the form "name1: value1 name2: value2".
 *
 * Formatting requirements:
 * - name and value need to be separated by a colon (spaces are optional)
 * - the value corresponding to the given name needs to be a double
 *
 * @return The number of successful assignments.
 */
static int IOHprofiler_options_read_double(const char *options, const char *name, double *pointer) {
    return IOHprofiler_options_read(options, name, "%lf", pointer);
}

/**
 * @brief Reads a string from options using the form "name1: value1 name2: value2".
 *
 * Formatting requirements:
 * - name and value need to be separated by a colon (spaces are optional)
 * - the value corresponding to the given name needs to be a string - either a single word or multiple words
 * in double quotes
 *
 * @return The number of successful assignments.
 */
static int IOHprofiler_options_read_string(const char *options, const char *name, char *pointer) {
    long i1, i2;

    if ((!options) || (strlen(options) == 0))
        return 0;

    i1 = IOHprofiler_strfind(options, name);
    if (i1 < 0)
        return 0;
    i2 = i1 + IOHprofiler_strfind(&options[i1], ":") + 1;

    /* Remove trailing white spaces */
    while (isspace((unsigned char)options[i2]))
        i2++;

    if (i2 <= i1) {
        return 0;
    }

    if (options[i2] == '\"') {
        /* The value starts with a quote: read everything between two quotes into a string */
        return sscanf(&options[i2], "\"%[^\"]\"", pointer);
    } else
        return sscanf(&options[i2], "%s", pointer);
}

/**
 * @brief Reads (possibly delimited) values from options using the form "name1: value1,value2,value3 name2: value4",
 * i.e. reads all characters from the corresponding name up to the next whitespace or end of string.
 *
 * Formatting requirements:
 * - name and value need to be separated by a colon (spaces are optional)
 *
 * @return The number of successful assignments.
 */
static int IOHprofiler_options_read_values(const char *options, const char *name, char *pointer) {
    long i1, i2;
    int i;

    if ((!options) || (strlen(options) == 0))
        return 0;

    i1 = IOHprofiler_strfind(options, name);
    if (i1 < 0)
        return 0;
    i2 = i1 + IOHprofiler_strfind(&options[i1], ":") + 1;

    /* Remove trailing white spaces */
    while (isspace((unsigned char)options[i2]))
        i2++;

    if (i2 <= i1) {
        return 0;
    }

    i = 0;
    while (!isspace((unsigned char)options[i2 + i]) && (options[i2 + i] != '\0')) {
        pointer[i] = options[i2 + i];
        i++;
    }
    pointer[i] = '\0';
    return i;
}
/**@}*/

/***********************************************************************************************************/

/**
 * @name Methods implementing functions on double values not contained in C89 standard
 */
/**@{*/

/**
 * @brief Rounds the given double to the nearest integer.
 */
static double IOHprofiler_double_round(const double number) {
    return floor(number + 0.5);
}

/**
 * @brief Returns the maximum of a and b.
 */
static double IOHprofiler_double_max(const double a, const double b) {
    if (a >= b) {
        return a;
    } else {
        return b;
    }
}

/**
 * @brief Returns the minimum of a and b.
 */
static double IOHprofiler_double_min(const double a, const double b) {
    if (a <= b) {
        return a;
    } else {
        return b;
    }
}

/**
 * @brief Performs a "safer" double to size_t conversion.
 */
static size_t IOHprofiler_double_to_size_t(const double number) {
    return (size_t)IOHprofiler_double_round(number);
}

/**
 * @brief  Returns 1 if |a - b| < precision and 0 otherwise.
 */
static int IOHprofiler_double_almost_equal(const double a, const double b, const double precision) {
    return (fabs(a - b) < precision);
}

/**@}*/

/***********************************************************************************************************/

/**
 * @name Methods handling NAN and INFINITY
 */
/**@{*/

/**
 * @brief Returns 1 if x is NAN and 0 otherwise.
 */
static int IOHprofiler_is_nan(const int x) {
    /*return (isnan(x) || (x != x) || !(x == x) || ((x >= NAN / (1 + 1e-9)) && (x <= NAN * (1 + 1e-9))));
*/
    return (0 || (x != x) || !(x == x) || ((x >= NAN / (1 + 1e-9)) && (x <= NAN * (1 + 1e-9))));
}

/**
 * @brief Returns 1 if the input vector of dimension dim contains any NAN values and 0 otherwise.
 */
static int IOHprofiler_vector_contains_nan(const int *x, const size_t dim) {
    size_t i;
    for (i = 0; i < dim; i++) {
        if (IOHprofiler_is_nan(x[i]))
            return 1;
    }
    return 0;
}

/**
 * @brief Sets all dim values of y to NAN.
 */
static void IOHprofiler_vector_set_to_nan(double *y, const size_t dim) {
    size_t i;
    for (i = 0; i < dim; i++) {
        y[i] = NAN;
    }
}

/**
 * @brief Returns 1 if x is INFINITY and 0 otherwise.
 */
static int IOHprofiler_is_inf(const int x) {
    if (IOHprofiler_is_nan(x))
        return 0;
    /*return (isinf(x) || (x <= -INFINITY) || (x >= INFINITY));*/
    return (0 || (x <= -INFINITY) || (x >= INFINITY));
}

/**@}*/

/***********************************************************************************************************/

/**
 * @name Miscellaneous methods
 */
/**@{*/

/**
 * @brief Returns the current time as a string.
 *
 * The caller is responsible for freeing the allocated memory using IOHprofiler_free_memory().
 */
static char *IOHprofiler_current_time_get_string(void) {
    time_t timer;
    char *time_string = IOHprofiler_allocate_string(30);
    struct tm *tm_info;
    time(&timer);
    tm_info = localtime(&timer);
    assert(tm_info != NULL);
    strftime(time_string, 30, "%d.%m.%y %H:%M:%S", tm_info);
    return time_string;
}

/**
 * @brief Returns the number of positive numbers pointed to by numbers (the count stops when the first
 * 0 is encountered of max_count numbers have been read).
 *
 * If there are more than max_count numbers, a IOHprofiler_error is raised. The name argument is used
 * only to provide more informative output in case of any problems.
 */
static size_t IOHprofiler_count_numbers(const size_t *numbers, const size_t max_count, const char *name) {
    size_t count = 0;
    while ((count < max_count) && (numbers[count] != 0)) {
        count++;
    }
    if (count == max_count) {
        IOHprofiler_error("IOHprofiler_count_numbers(): over %lu numbers in %s", (unsigned long)max_count, name);
        return 0; /* Never reached*/
    }

    return count;
}

/**@}*/

/***********************************************************************************************************/
#line 14 "code-experiments/src/IOHprofiler_suite.c"

/** the source files for additional suites*/
#line 1 "code-experiments/src/suite_PBO.c"
/**
 * @file suite_IOHprofiler.c
 * @brief Implementation of the IOHprofiler suite containing 24 noiseless single-objective functions in 6
 * dimensions.
 */

#line 8 "code-experiments/src/suite_PBO.c"

#line 1 "code-experiments/src/f_binary.c"
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

#line 15 "code-experiments/src/f_binary.c"
#line 1 "code-experiments/src/IOHprofiler_problem.c"
/**
 * @file IOHprofiler_problem.c
 * @brief Definitions of functions regarding IOHprofiler problems.
 */

#include <float.h>
#line 8 "code-experiments/src/IOHprofiler_problem.c"
#line 9 "code-experiments/src/IOHprofiler_problem.c"

#line 11 "code-experiments/src/IOHprofiler_problem.c"

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
    problem->dimension = number_of_variables;
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

    problem->dimension = other->dimension;
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
    /*problem->dimension = number_of_variables;
    */problem->number_of_variables = number_of_variables;
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
    assert(problem->dimension > 0);
    return problem->dimension;
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
#line 16 "code-experiments/src/f_binary.c"
#line 1 "code-experiments/src/suite_PBO_legacy_code.c"
/**
 * @file suite_IOHprofiler_legacy_code.c
 * @brief Legacy code from required to replicate the IOHprofiler functions.
 *
 * All of this code should only be used by the suite_IOHprofiler functions to provide compatibility to the
 * legacy code. New test beds should strive to use the new IOHprofiler facilities for random number generation etc.
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#line 13 "code-experiments/src/suite_PBO_legacy_code.c"

/** @brief Maximal dimension used in IOHprofiler. */
#define SUITE_IOHprofiler_MAX_DIM 5000

/** @brief Computes the minimum of the two values. */
static double IOHprofiler_fmin(double a, double b) {
    return (a < b) ? a : b;
}

/** @brief Computes the maximum of the two values. */
static double IOHprofiler_fmax(double a, double b) {
    return (a > b) ? a : b;
}

/** @brief Rounds the given value. */
static double IOHprofiler_round(double x) {
    return floor(x + 0.5);
}

/**
 * @brief Allocates a n by m matrix structured as an array of pointers to double arrays.
 */
static double **IOHprofiler_allocate_matrix(const size_t n, const size_t m) {
    double **matrix = NULL;
    size_t i;
    matrix = (double **)IOHprofiler_allocate_memory(sizeof(double *) * n);
    for (i = 0; i < n; ++i) {
        matrix[i] = IOHprofiler_allocate_vector(m);
    }
    return matrix;
}

/**
 * @brief Frees the matrix structured as an array of pointers to double arrays.
 */
static void IOHprofiler_free_matrix(double **matrix, const size_t n) {
    size_t i;
    for (i = 0; i < n; ++i) {
        if (matrix[i] != NULL) {
            IOHprofiler_free_memory(matrix[i]);
            matrix[i] = NULL;
        }
    }
    IOHprofiler_free_memory(matrix);
}

/**
 * @brief Generates N uniform random numbers using inseed as the seed and stores them in r.
 */
static void IOHprofiler_unif(double *r, size_t N, long inseed) {
    /* generates N uniform numbers with starting seed */
    long aktseed;
    long tmp;
    long rgrand[32];
    long aktrand;
    long i;

    if (inseed < 0)
        inseed = -inseed;
    if (inseed < 1)
        inseed = 1;
    aktseed = inseed;
    for (i = 39; i >= 0; i--) {
        tmp = (int)floor((double)aktseed / (double)127773);
        aktseed = 16807 * (aktseed - tmp * 127773) - 2836 * tmp;
        if (aktseed < 0)
            aktseed = aktseed + 2147483647;
        if (i < 32)
            rgrand[i] = aktseed;
    }
    aktrand = rgrand[0];
    for (i = 0; i < N; i++) {
        tmp = (int)floor((double)aktseed / (double)127773);
        aktseed = 16807 * (aktseed - tmp * 127773) - 2836 * tmp;
        if (aktseed < 0)
            aktseed = aktseed + 2147483647;
        tmp = (int)floor((double)aktrand / (double)67108865);
        aktrand = rgrand[tmp];
        rgrand[tmp] = aktseed;
        r[i] = (double)aktrand / 2.147483647e9;
        if (r[i] == 0.) {
            r[i] = 1e-99;
        }
    }
    return;
}

/**
 * @brief Converts from packed matrix storage to an array of array of double representation.
 */
static double **IOHprofiler_reshape(double **B, double *vector, const size_t m, const size_t n) {
    size_t i, j;
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            B[i][j] = vector[j * m + i];
        }
    }
    return B;
}

/**
 * @brief Generates N Gaussian random numbers using the given seed and stores them in g.
 */
static void IOHprofiler_gauss(double *g, const size_t N, const long seed) {
    size_t i;
    double uniftmp[6000];
    assert(2 * N < 6000);
    IOHprofiler_unif(uniftmp, 2 * N, seed);

    for (i = 0; i < N; i++) {
        g[i] = sqrt(-2 * log(uniftmp[i])) * cos(2 * IOHprofiler_pi * uniftmp[N + i]);
        if (g[i] == 0.)
            g[i] = 1e-99;
    }
    return;
}

/**
 * @brief Computes a DIM by DIM rotation matrix based on seed and stores it in B.
 */
static void IOHprofiler_compute_rotation(double **B, const long seed, const size_t DIM) {
    /* To ensure temporary data fits into gvec */
    double prod;
    double gvect[2000];
    long i, j, k; /* Loop over pairs of column vectors. */

    assert(DIM * DIM < 2000);

    IOHprofiler_gauss(gvect, DIM * DIM, seed);
    IOHprofiler_reshape(B, gvect, DIM, DIM);
    /*1st coordinate is row, 2nd is column.*/

    for (i = 0; i < DIM; i++) {
        for (j = 0; j < i; j++) {
            prod = 0;
            for (k = 0; k < DIM; k++)
                prod += B[k][i] * B[k][j];
            for (k = 0; k < DIM; k++)
                B[k][i] -= prod * B[k][j];
        }
        prod = 0;
        for (k = 0; k < DIM; k++)
            prod += B[k][i] * B[k][i];
        for (k = 0; k < DIM; k++)
            B[k][i] /= sqrt(prod);
    }
}

static void IOHprofiler_copy_rotation_matrix(double **rot, double *M, double *b, const size_t DIM) {
    size_t row, column;
    double *current_row;

    for (row = 0; row < DIM; ++row) {
        current_row = M + row * DIM;
        for (column = 0; column < DIM; ++column) {
            current_row[column] = rot[row][column];
        }
        b[row] = 0.0;
    }
}

/**
 * @brief Randomly computes the location of the global optimum.
 */
static void IOHprofiler_compute_xopt(int *xopt, const long seed, const size_t DIM) {
    long i;
    double *xopttemp;
    xopttemp = IOHprofiler_allocate_vector(DIM);
    IOHprofiler_unif(xopttemp, DIM, seed);
    for (i = 0; i < DIM; i++) {
        xopt[i] = (int)(2 * floor(1e4 * xopttemp[i]) / 1e4 / 1);
    }
    IOHprofiler_free_memory(xopttemp);
}

/**
 * @brief Randomly computes the location of the global optimum.
 */
static void IOHprofiler_compute_xopt_double(double *xopt, const long seed, const size_t DIM) {
    long i;
    IOHprofiler_unif(xopt, DIM, seed);
    for (i = 0; i < DIM; i++) {
        xopt[i] = floor(1e4 * xopt[i]) / 1e4;
        if (xopt[i] == 0.0)
            xopt[i] = -1e-5;
    }
}

/**
 * @brief Randomly chooses the objective offset for the given function and instance.
 */
static double IOHprofiler_compute_fopt(const size_t function, const size_t instance) {
  long rseed, rrseed;
  double gval, gval2;


  rseed = (long) function;

  rrseed = rseed + (long) (10000 * instance);
  IOHprofiler_gauss(&gval, 1, rrseed);
  IOHprofiler_gauss(&gval2, 1, rrseed + 1);
  return IOHprofiler_fmin(1000., IOHprofiler_fmax(-1000., IOHprofiler_round(100. * 100. * gval / gval2) / 100.));
}
#line 17 "code-experiments/src/f_binary.c"
#line 1 "code-experiments/src/transform_obj_shift.c"
/**
 * @file transform_obj_shift.c
 * @brief Implementation of shifting the objective value by the given offset.
 */

#include <assert.h>

#line 9 "code-experiments/src/transform_obj_shift.c"
#line 10 "code-experiments/src/transform_obj_shift.c"

/**
 * @brief Data type for transform_obj_shift.
 */
typedef struct {
  double offset;
} transform_obj_shift_data_t;

/**
 * @brief Evaluates the transformation.
 */
static void transform_obj_shift_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  transform_obj_shift_data_t *data;
  size_t i;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }

  data = (transform_obj_shift_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  IOHprofiler_evaluate_function(IOHprofiler_problem_transformed_get_inner_problem(problem), x, y);

  for (i = 0; i < problem->number_of_objectives; i++) {
      y[i] += data->offset;
      problem->raw_fitness[i] = IOHprofiler_problem_transformed_get_inner_problem(problem)->raw_fitness[i];
  }

  assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_obj_shift(IOHprofiler_problem_t *inner_problem, const double offset) {
  IOHprofiler_problem_t *problem;
  transform_obj_shift_data_t *data;
  size_t i;
  data = (transform_obj_shift_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = offset;

  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, NULL, "transform_obj_shift");
  problem->evaluate_function = transform_obj_shift_evaluate;
  for (i = 0; i < problem->number_of_objectives; i++) {
      problem->best_value[0] += offset;
  }
  return problem;
}
#line 18 "code-experiments/src/f_binary.c"
#line 1 "code-experiments/src/transform_vars_xor.c"
/**
 * @file transform_vars_xor.c
 * @brief Implementation of xor all decision values by an offset.
 */

#include <assert.h>

#line 9 "code-experiments/src/transform_vars_xor.c"
#line 10 "code-experiments/src/transform_vars_xor.c"

/**
 * @brief Data type for transform_vars_xor.
 */
typedef struct {
  int *offset;
  int *xor_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_xor_data_t;

/**
 * @brief Evaluates the transformation.
 */
static int xor_compute(const int x1, const int x2){
    return (int) x1 != x2;
}

static void transform_vars_xor_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  transform_vars_xor_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }

  data = (transform_vars_xor_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->xor_x[i] = xor_compute(x[i],data->offset[i]);
  }
  inner_problem->number_of_variables = problem->number_of_variables;
  IOHprofiler_evaluate_function(inner_problem, data->xor_x, y);
  problem->raw_fitness[0] = y[0];

  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_xor_free(void *thing) {
  transform_vars_xor_data_t *data = (transform_vars_xor_data_t *) thing;
  IOHprofiler_free_memory(data->xor_x);
  IOHprofiler_free_memory(data->offset);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_xor(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int xor_bounds) {

  transform_vars_xor_data_t *data;
  IOHprofiler_problem_t *problem;
  size_t i;
  if (xor_bounds)
    IOHprofiler_error("xor_bounds not implemented.");

  data = (transform_vars_xor_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, inner_problem->number_of_variables);
  data->xor_x = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);

  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_xor_free, "transform_vars_xor");
  problem->evaluate_function = transform_vars_xor_evaluate;
  /* Compute best parameter */
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = xor_compute(problem->best_parameter[i],data->offset[i]);
  }
  return problem;
}
#line 19 "code-experiments/src/f_binary.c"
#line 1 "code-experiments/src/transform_vars_sigma.c"
/**
 * @file transform_vars_sigma.c
 * @brief Implementation of sigmad all decision values by an offset.
 */

#include <assert.h>

#line 9 "code-experiments/src/transform_vars_sigma.c"
#line 10 "code-experiments/src/transform_vars_sigma.c"

/**
 * @brief Data type for transform_vars_sigma.
 */
typedef struct {
  int *offset;
  int *sigma_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_sigma_data_t;

/**
 * @brief Evaluates the transformation.
 */
static int sigma_compute(const int *x, const int pos){
  return x[pos];
}

static void transform_vars_sigma_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  transform_vars_sigma_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (transform_vars_sigma_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->sigma_x[i] = sigma_compute(x,data->offset[i]);
  }

  IOHprofiler_evaluate_function(inner_problem, data->sigma_x, y);
  problem->raw_fitness[0] = y[0];
  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_sigma_free(void *thing) {
  transform_vars_sigma_data_t *data = (transform_vars_sigma_data_t *) thing;
  IOHprofiler_free_memory(data->sigma_x);
  IOHprofiler_free_memory(data->offset);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_sigma(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int sigma_bounds) {

  transform_vars_sigma_data_t *data;
  IOHprofiler_problem_t *problem;
  int * temp_best;
  size_t i;
  if (sigma_bounds)
    IOHprofiler_error("sigma_bounds not implemented.");

  data = (transform_vars_sigma_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, inner_problem->number_of_variables);
  data->sigma_x = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);
  temp_best = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);

  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_sigma_free, "transform_vars_sigma");
  problem->evaluate_function = transform_vars_sigma_evaluate;
  /* Compute best parameter */
  for (i = 0; i < problem->number_of_variables; i++) {
      temp_best[i] = sigma_compute(problem->best_parameter,data->offset[i]);
  }
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = temp_best[i];
  }
  IOHprofiler_free_memory(temp_best);
  return problem;
}
#line 20 "code-experiments/src/f_binary.c"


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
#line 10 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_jump.c"
/**
 * @Jump function
 */

/**
 * @file f_jump.c
 * @brief Implementation of the jump function and problem.
 * this is for jump function
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>

#line 16 "code-experiments/src/f_jump.c"
#line 17 "code-experiments/src/f_jump.c"
#line 18 "code-experiments/src/f_jump.c"
#line 19 "code-experiments/src/f_jump.c"
#line 20 "code-experiments/src/f_jump.c"
#line 21 "code-experiments/src/f_jump.c"
#line 1 "code-experiments/src/transform_obj_scale.c"
/**
 * @file transform_obj_scale.c
 * @brief Implementation of scaleing the objective value by the given offset.
 */

#include <assert.h>

#line 9 "code-experiments/src/transform_obj_scale.c"
#line 10 "code-experiments/src/transform_obj_scale.c"

/**
 * @brief Data type for transform_obj_scale.
 */
typedef struct {
  double offset;
} transform_obj_scale_data_t;

/**
 * @brief Evaluates the transformation.
 */
static void transform_obj_scale_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  transform_obj_scale_data_t *data;
  size_t i;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (transform_obj_scale_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  IOHprofiler_evaluate_function(IOHprofiler_problem_transformed_get_inner_problem(problem), x, y);

  for (i = 0; i < problem->number_of_objectives; i++) {
      y[i] = y[i] * data->offset;
      problem->raw_fitness[i] = IOHprofiler_problem_transformed_get_inner_problem(problem)->raw_fitness[i];
  }
  assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_obj_scale(IOHprofiler_problem_t *inner_problem, const double offset) {
  IOHprofiler_problem_t *problem;
  transform_obj_scale_data_t *data;
  size_t i;
  data = (transform_obj_scale_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = offset;

  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, NULL, "transform_obj_scale");
  problem->evaluate_function = transform_obj_scale_evaluate;
  for (i = 0; i < problem->number_of_objectives; i++) {
      problem->best_value[0] = problem->best_value[0] * offset;
  }
  return problem;
}
#line 22 "code-experiments/src/f_jump.c"

/**
 * @brief Implements the linear slope function without connections to any IOHprofiler structures.
 */
static double f_jump_raw(const int *x, const size_t number_of_variables) {
    static const size_t l = 1;
    size_t i;
    double result = 0.0;

    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;

    for (i = 0; i < number_of_variables; ++i) {
        result += (double)x[i];
    }

    if ((result >= number_of_variables - l || result <= l) && result != number_of_variables) {
        result = 0.0;
    }

    return result;
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static void f_jump_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_jump_raw(x, problem->number_of_variables);
    problem->raw_fitness[0] = y[0];
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic jump problem.
 */
static IOHprofiler_problem_t *f_jump_allocate(const size_t number_of_variables) {
    /* best_parameter will be overwritten below */
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("jump function",
                                                                               f_jump_evaluate, NULL, number_of_variables, 0, 1, 1);
    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "jump", number_of_variables);

    /* Compute best solution */
    f_jump_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler jump problem.
 */
static IOHprofiler_problem_t *f_jump_IOHprofiler_problem_allocate(const size_t function,
                                                                  const size_t dimension,
                                                                  const size_t instance,
                                                                  const long rseed,
                                                                  const char *problem_id_template,
                                                                  const char *problem_name_template) {

  int *z, *sigma;
  int temp,t;
  size_t i;
  double a;
  double b;
  double *xins;
  IOHprofiler_problem_t *problem;
  z = IOHprofiler_allocate_int_vector(dimension);
  sigma = IOHprofiler_allocate_int_vector(dimension);
  xins = IOHprofiler_allocate_vector(dimension);
  problem = f_jump_allocate(dimension);

  if(instance == 1){
    for(i = 0; i < dimension; i++)
      z[i] = 0;
    a = 0.0;
    problem = transform_vars_xor(problem,z,0);
    problem = transform_obj_shift(problem,a);
  }
  else if(instance > 1 && instance <= 50){
        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_sigma(problem, sigma, 0);
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
  IOHprofiler_free_memory(sigma);
  IOHprofiler_free_memory(xins);
  return problem;
}
#line 11 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_leading_ones.c"
/**
 * @Leading Ones
 */

/**
 * @file f_leading_ones.c
 * @brief Implementation of the leading ones function and problem.
 */

#include <assert.h>
#include <math.h>

#line 14 "code-experiments/src/f_leading_ones.c"
#line 15 "code-experiments/src/f_leading_ones.c"
#line 16 "code-experiments/src/f_leading_ones.c"
#line 17 "code-experiments/src/f_leading_ones.c"
#line 18 "code-experiments/src/f_leading_ones.c"
#line 19 "code-experiments/src/f_leading_ones.c"
#line 20 "code-experiments/src/f_leading_ones.c"


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
  int *z, *sigma;
  int temp,t;
  size_t i;
  double a;
  double b;
  double *xins;
  IOHprofiler_problem_t *problem;
  z = IOHprofiler_allocate_int_vector(dimension);
  sigma = IOHprofiler_allocate_int_vector(dimension);
  xins = IOHprofiler_allocate_vector(dimension);
  problem = f_leading_ones_allocate(dimension);

  if(instance == 1){
    for(i = 0; i < dimension; i++)
        z[i] = 0;
    a = 0.0;
    problem = transform_vars_xor(problem,z,0);
    problem = transform_obj_shift(problem,a);
  }
  else if(instance > 1 && instance <= 50){
        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_sigma(problem, sigma, 0);
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
  IOHprofiler_free_memory(sigma);
  IOHprofiler_free_memory(xins);
  return problem;
}
#line 12 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_linear.c"
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

#line 15 "code-experiments/src/f_linear.c"
#line 16 "code-experiments/src/f_linear.c"
#line 17 "code-experiments/src/f_linear.c"
#line 18 "code-experiments/src/f_linear.c"
#line 19 "code-experiments/src/f_linear.c"
#line 20 "code-experiments/src/f_linear.c"
#line 21 "code-experiments/src/f_linear.c"

/**
 * @brief Implements the linear function without connections to any IOHprofiler structures.
 */

long f_linear_ins;

static double f_linear_raw(const int *x, const size_t number_of_variables) {
    size_t i = 0;
    double result;
    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;

    result = 0.0;
    for (i = 0; i < number_of_variables; ++i) {
        result += (double)x[i] * (double)(i+1);
    }
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

  int *z, *sigma;
  int temp,t;
  size_t i;
  double a;
  double b;
  double *xins;
  IOHprofiler_problem_t *problem;
  z = IOHprofiler_allocate_int_vector(dimension);
  sigma = IOHprofiler_allocate_int_vector(dimension);
  xins = IOHprofiler_allocate_vector(dimension);
  problem =  f_linear_allocate(dimension);
  f_linear_ins = 1;

  if(instance == 1){
    for(i = 0; i < dimension; i++)
      z[i] = 0;
    a = 0.0;
    problem = transform_vars_xor(problem,z,0);
    problem = transform_obj_shift(problem,a);
  }
  else if(instance > 1 && instance <= 50){
        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_sigma(problem, sigma, 0);
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
  IOHprofiler_free_memory(sigma);
  IOHprofiler_free_memory(xins);
  return problem;
}
#line 13 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_one_max.c"
/**
 * @Onemax function
 */

/**
 * @file f_one_max.c
 * @brief Implementation of the one_max function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 14 "code-experiments/src/f_one_max.c"
#line 15 "code-experiments/src/f_one_max.c"
#line 16 "code-experiments/src/f_one_max.c"
#line 17 "code-experiments/src/f_one_max.c"
#line 18 "code-experiments/src/f_one_max.c"
#line 19 "code-experiments/src/f_one_max.c"
#line 20 "code-experiments/src/f_one_max.c"
/**
 * @brief Implements the one_max function without connections to any IOHprofiler structures.
 */
static double f_one_max_raw(const int *x, const size_t number_of_variables) {
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
static void f_one_max_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_one_max_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic one_max problem.
 */
static IOHprofiler_problem_t *f_one_max_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("one_max function",
                                                                               f_one_max_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "one_max", number_of_variables);

    /* Compute best solution */
    f_one_max_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler one_max problem.
 */
static IOHprofiler_problem_t *f_one_max_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {
    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);
    sigma = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);
    problem = f_one_max_allocate(dimension);

    if(instance == 1){
        for(i = 0; i < dimension; i++)
            z[i] = 0;
        a = 0.0;
        problem = transform_vars_xor(problem,z,0);
        problem = transform_obj_shift(problem,a);
    }
    else if(instance > 1 && instance <= 50){
        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_sigma(problem, sigma, 0);
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
    IOHprofiler_free_memory(sigma);
    IOHprofiler_free_memory(xins);
    return problem;
}
#line 14 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_labs.c"
/**
 * @Onemax function
 */

/**
 * @file f_labs.c
 * @brief Implementation of the labs function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 14 "code-experiments/src/f_labs.c"
#line 15 "code-experiments/src/f_labs.c"
#line 16 "code-experiments/src/f_labs.c"
#line 17 "code-experiments/src/f_labs.c"
#line 18 "code-experiments/src/f_labs.c"
#line 19 "code-experiments/src/f_labs.c"
#line 1 "code-experiments/src/transform_vars_shift.c"
/**
 * @file transform_vars_shift.c
 * @brief Implementation of shifting all decision values by an offset.
 */

#include <assert.h>

#line 9 "code-experiments/src/transform_vars_shift.c"
#line 10 "code-experiments/src/transform_vars_shift.c"

/**
 * @brief Data type for transform_vars_shift.
 */
typedef struct {
  int *offset;
  int *shifted_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_shift_data_t;

/**
 * @brief Evaluates the transformation.
 */
static void transform_vars_shift_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  transform_vars_shift_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }

  data = (transform_vars_shift_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);

  for (i = 0; i < problem->number_of_variables; ++i) {
    data->shifted_x[i] = x[i] + data->offset[i];
  }
  inner_problem->number_of_variables = problem->number_of_variables;
  IOHprofiler_evaluate_function(inner_problem, data->shifted_x, y);
  problem->raw_fitness[0] = y[0];

  assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_shift_free(void *thing) {
  transform_vars_shift_data_t *data = (transform_vars_shift_data_t *) thing;
  IOHprofiler_free_memory(data->shifted_x);
  IOHprofiler_free_memory(data->offset);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_shift(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int shift_bounds) {
  transform_vars_shift_data_t *data;
  IOHprofiler_problem_t *problem;
  size_t i;
  if (shift_bounds)
    IOHprofiler_error("shift_bounds not implemented.");


  data = (transform_vars_shift_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, inner_problem->number_of_variables);
  data->shifted_x = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);


  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_shift_free, "transform_vars_shift");
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);
  problem->evaluate_function = transform_vars_shift_evaluate;
  /* Compute best parameter */
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] += data->offset[i];
  }
  return problem;
}
#line 20 "code-experiments/src/f_labs.c"
#line 21 "code-experiments/src/f_labs.c"

/**
 * @brief Implements the labs function without connections to any IOHprofiler structures.
 */
static double correlation(const int * x, const size_t number_of_variables, size_t k)
{
    size_t i;
    int x1,x2;
    double result;
    result = 0.0;
    for(i = 0 ; i < number_of_variables - k; ++i){
        if(x[i] == 0){
            x1 = -1;
        }
        else{
            x1 = 1;
        }
        if(x[i + k] == 0){
            x2 = -1;
        }
        else{
            x2 = 1;
        }
        result += x1 * x2;
    }
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


    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);
    sigma = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);
    problem = f_labs_allocate(dimension);
    if(instance == 1){
        for(i = 0; i < dimension; i++)
            z[i] = 0;
        a = 0.0;
        problem = transform_vars_xor(problem,z,0);
        problem = transform_obj_shift(problem,a);
    }
    else if(instance > 1 && instance <= 50){
        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_sigma(problem, sigma, 0);
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
    IOHprofiler_free_memory(sigma);
    IOHprofiler_free_memory(xins);
    return problem;
}
#line 15 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_one_max_dummy1.c"
/**
 * @Onemax function
 */

/**
 * @file f_one_max_dummy1.c
 * @brief Implementation of the one_max_dummy1 function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 14 "code-experiments/src/f_one_max_dummy1.c"
#line 15 "code-experiments/src/f_one_max_dummy1.c"
#line 16 "code-experiments/src/f_one_max_dummy1.c"
#line 17 "code-experiments/src/f_one_max_dummy1.c"
#line 18 "code-experiments/src/f_one_max_dummy1.c"
#line 19 "code-experiments/src/f_one_max_dummy1.c"
#line 1 "code-experiments/src/transform_vars_dummy.c"
/**
 * @file transform_vars_dummy.c
 * @brief Implementation of dummyd all decision values by an offset.
 */

#include <assert.h>

#line 9 "code-experiments/src/transform_vars_dummy.c"
#line 10 "code-experiments/src/transform_vars_dummy.c"

/**
 * @brief Data type for transform_vars_dummy.
 */
typedef struct {
  int *offset;
  int *reduncted_x;
  int *postion_match;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_dummy_data_t;

static void compute_dummy_match(int * postion_match, const size_t old_dimension, const size_t new_dimension){
  size_t i,j;
  int temp;
  double *randN = IOHprofiler_allocate_vector(1);
  int flag;
  int dummyins = 10000;
  for(i = 0; i < new_dimension;){
    IOHprofiler_unif(randN, 1, dummyins++);
    temp = (int)(randN[0] * (double)old_dimension);
    flag = 1;
    for(j = 0; j < i; ++j){
      if(postion_match[j] == temp){
        flag = 0;
        break;
      }
    }
    if(flag == 1){
      postion_match[i] = temp;
      ++i;
    }
    else{
      continue;
    }
  }
  IOHprofiler_free_memory(randN);
}

static void transform_vars_dummy_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  transform_vars_dummy_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (transform_vars_dummy_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->reduncted_x[i] = x[data->postion_match[i]];
  }
  IOHprofiler_evaluate_function(inner_problem, data->reduncted_x, y);
  problem->raw_fitness[0] = y[0];

  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_dummy_free(void *thing) {
  transform_vars_dummy_data_t *data = (transform_vars_dummy_data_t *) thing;
  IOHprofiler_free_memory(data->reduncted_x);
  IOHprofiler_free_memory(data->postion_match);
  IOHprofiler_free_memory(data->offset);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_dummy(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int dummy_bounds) {

  transform_vars_dummy_data_t *data;
  IOHprofiler_problem_t *problem;
  size_t i;
  size_t new_dimension;
  if (dummy_bounds)
    IOHprofiler_error("dummy_bounds not implemented.");

  new_dimension = (size_t)offset[0];
  data = (transform_vars_dummy_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, 1);
  data->reduncted_x = IOHprofiler_allocate_int_vector(new_dimension);
  data->postion_match = IOHprofiler_allocate_int_vector(new_dimension);
  compute_dummy_match(data->postion_match,inner_problem->number_of_variables,new_dimension);

  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_dummy_free, "transform_vars_dummy");
  problem->number_of_variables = new_dimension;
  inner_problem->number_of_variables = problem->number_of_variables;

  while(inner_problem->data != NULL){
    inner_problem = IOHprofiler_problem_transformed_get_inner_problem(inner_problem);
    inner_problem->number_of_variables = problem->number_of_variables;
  }

  problem->evaluate_function = transform_vars_dummy_evaluate;
  /* Compute best parameter */
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = problem->best_parameter[data->postion_match[i]];
  }
  IOHprofiler_evaluate_function(inner_problem, problem->best_parameter, problem->best_value);
  return problem;
}
#line 20 "code-experiments/src/f_one_max_dummy1.c"
#line 1 "code-experiments/src/transform_vars_dummy_sigma.c"
/**
 * @file transform_vars_dummy.c
 * @brief Implementation of dummyd all decision values by an offset.
 */

#include <assert.h>

#line 9 "code-experiments/src/transform_vars_dummy_sigma.c"
#line 10 "code-experiments/src/transform_vars_dummy_sigma.c"

/**
 * @brief Data type for transform_vars_dummy.
 */
typedef struct {
  int *offset;
  int *reduncted_x;
  int *postion_match;
  int *sigma_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_dummy_sigma_data_t;

static void compute_dummy_sigma_match(int * postion_match, const size_t old_dimension, const size_t new_dimension){
  size_t i,j;
  int temp;
  double *randN = IOHprofiler_allocate_vector(1);
  int flag;
  int dummyins = 10000;
  for(i = 0; i < new_dimension;){
    IOHprofiler_unif(randN, 1, dummyins++);
    temp = (int)(randN[0] * (double)old_dimension);
    flag = 1;
    for(j = 0; j < i; ++j){
      if(postion_match[j] == temp){
        flag = 0;
        break;
      }
    }
    if(flag == 1){
      postion_match[i] = temp;
      ++i;
    }
    else{
      continue;
    }
  }
  IOHprofiler_free_memory(randN);
}

static int dummy_sigma_compute(const int *x, const int pos){
  return x[pos];
}


static void transform_vars_dummy_sigma_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  transform_vars_dummy_sigma_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (transform_vars_dummy_sigma_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->reduncted_x[i] = x[data->postion_match[i]];
  }
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->sigma_x[i] = dummy_sigma_compute(data->reduncted_x,data->offset[i]);
  }
  IOHprofiler_evaluate_function(inner_problem, data->sigma_x, y);
  problem->raw_fitness[0] = y[0];

  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_dummy_sigma_free(void *thing) {
  transform_vars_dummy_sigma_data_t *data = (transform_vars_dummy_sigma_data_t *) thing;
  IOHprofiler_free_memory(data->reduncted_x);
  IOHprofiler_free_memory(data->postion_match);
  IOHprofiler_free_memory(data->sigma_x);
  IOHprofiler_free_memory(data->offset);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_dummy_sigma(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int *offset1,
                                            const int dummy_bounds) {

  transform_vars_dummy_sigma_data_t *data;
  IOHprofiler_problem_t *problem;
  int * temp_best;
  size_t i;
  size_t new_dimension;
  if (dummy_bounds)
    IOHprofiler_error("dummy_bounds not implemented.");

  new_dimension = (size_t)offset[0];
  temp_best = IOHprofiler_allocate_int_vector(new_dimension);
  data = (transform_vars_dummy_sigma_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset1, new_dimension);
  data->reduncted_x = IOHprofiler_allocate_int_vector(new_dimension);
  data->postion_match = IOHprofiler_allocate_int_vector(new_dimension);
  data->sigma_x = IOHprofiler_allocate_int_vector(new_dimension);
  compute_dummy_sigma_match(data->postion_match,inner_problem->number_of_variables,new_dimension);

  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_dummy_sigma_free, "transform_vars_dummy_sigma");
  problem->number_of_variables = new_dimension;
  inner_problem->number_of_variables = problem->number_of_variables;

  while(inner_problem->data != NULL){
    inner_problem = IOHprofiler_problem_transformed_get_inner_problem(inner_problem);
    inner_problem->number_of_variables = problem->number_of_variables;
  }

  problem->evaluate_function = transform_vars_dummy_sigma_evaluate;
  /* Compute best parameter */
  for (i = 0; i < problem->number_of_variables; i++) {
      temp_best[i] = problem->best_parameter[data->postion_match[i]];
  }
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = temp_best[i];
  }
  for (i = 0; i < problem->number_of_variables; i++) {
      temp_best[i] = dummy_sigma_compute(problem->best_parameter,data->offset[i]);
  }
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = temp_best[i];
  }
  problem->evaluate_function(problem, problem->best_parameter, problem->best_value);
  IOHprofiler_free_memory(temp_best);
  return problem;
}
#line 21 "code-experiments/src/f_one_max_dummy1.c"
#line 1 "code-experiments/src/transform_vars_dummy_xor.c"
/**
 * @file transform_vars_dummy.c
 * @brief Implementation of dummyd all decision values by an offset.
 */

#include <assert.h>

#line 9 "code-experiments/src/transform_vars_dummy_xor.c"
#line 10 "code-experiments/src/transform_vars_dummy_xor.c"

/**
 * @brief Data type for transform_vars_dummy.
 */
typedef struct {
  int *offset;
  int *reduncted_x;
  int *postion_match;
  int *xor_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_dummy_xor_data_t;

static void compute_dummy_xor_match(int * postion_match, const size_t old_dimension, const size_t new_dimension){
  size_t i,j;
  int temp;
  double *randN = IOHprofiler_allocate_vector(1);
  int flag;
  int dummyins = 10000;
  for(i = 0; i < new_dimension;){
    IOHprofiler_unif(randN, 1, dummyins++);
    temp = (int)(randN[0] * (double)old_dimension);
    flag = 1;
    for(j = 0; j < i; ++j){
      if(postion_match[j] == temp){
        flag = 0;
        break;
      }
    }
    if(flag == 1){
      postion_match[i] = temp;
      ++i;
    }
    else{
      continue;
    }
  }
  IOHprofiler_free_memory(randN);
}

static int dummy_xor_compute(const int x1, const int x2){
    return (int) x1 != x2;
}


static void transform_vars_dummy_xor_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  transform_vars_dummy_xor_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (transform_vars_dummy_xor_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->reduncted_x[i] = x[data->postion_match[i]];
  }
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->xor_x[i] = dummy_xor_compute(data->reduncted_x[i],data->offset[i]);
  }
  IOHprofiler_evaluate_function(inner_problem, data->xor_x, y);
  problem->raw_fitness[0] = y[0];

  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_dummy_xor_free(void *thing) {
  transform_vars_dummy_xor_data_t *data = (transform_vars_dummy_xor_data_t *) thing;
  IOHprofiler_free_memory(data->reduncted_x);
  IOHprofiler_free_memory(data->postion_match);
  IOHprofiler_free_memory(data->xor_x);
  IOHprofiler_free_memory(data->offset);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_dummy_xor(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int *offset1,
                                            const int dummy_bounds) {

  transform_vars_dummy_xor_data_t *data;
  IOHprofiler_problem_t *problem;
  size_t i;
  size_t new_dimension;
  if (dummy_bounds)
    IOHprofiler_error("dummy_bounds not implemented.");

  new_dimension = (size_t)offset[0];
  data = (transform_vars_dummy_xor_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset1, new_dimension);
  data->reduncted_x = IOHprofiler_allocate_int_vector(new_dimension);
  data->postion_match = IOHprofiler_allocate_int_vector(new_dimension);
  data->xor_x = IOHprofiler_allocate_int_vector(new_dimension);
  compute_dummy_xor_match(data->postion_match,inner_problem->number_of_variables,new_dimension);

  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_dummy_xor_free, "transform_vars_dummy_xor");
  problem->number_of_variables = new_dimension;
  inner_problem->number_of_variables = problem->number_of_variables;

  while(inner_problem->data != NULL){
    inner_problem = IOHprofiler_problem_transformed_get_inner_problem(inner_problem);
    inner_problem->number_of_variables = problem->number_of_variables;
  }

  problem->evaluate_function = transform_vars_dummy_xor_evaluate;
  /* Compute best parameter */
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = problem->best_parameter[data->postion_match[i]];
  }
  IOHprofiler_evaluate_function(inner_problem, problem->best_parameter, problem->best_value);
  return problem;
}
#line 22 "code-experiments/src/f_one_max_dummy1.c"
#line 23 "code-experiments/src/f_one_max_dummy1.c"


/**
 * @brief Implements the one_max_dummy1 function without connections to any IOHprofiler structures.
 */
static double f_one_max_dummy1_raw(const int *x, const size_t number_of_variables) {
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
static void f_one_max_dummy1_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_one_max_dummy1_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic one_max_dummy1 problem.
 */
static IOHprofiler_problem_t *f_one_max_dummy1_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("one_max_dummy1 function",
                                                                               f_one_max_dummy1_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "one_max_dummy1", number_of_variables);

    /* Compute best solution */
    f_one_max_dummy1_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler one_max_dummy1 problem.
 */
static IOHprofiler_problem_t *f_one_max_dummy1_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {

    int * dummy;
    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;





    dummy = IOHprofiler_allocate_int_vector(1);
    problem = f_one_max_dummy1_allocate(dimension);

    if(instance == 1){
        dummy[0] = (int)((double)dimension * 0.5);
        problem = transform_vars_dummy(problem, dummy, 0);
    }
    else if(instance > 1 && instance <= 50){
        dummy[0] = (int)((double)dimension * 0.5);

        z = IOHprofiler_allocate_int_vector((size_t)dummy[0]);
        IOHprofiler_compute_xopt(z,rseed,(size_t)dummy[0]);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_dummy_xor(problem,dummy,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);

        IOHprofiler_free_memory(z);
    }
    else if(instance > 50 && instance <= 100)
    {
        dummy[0] = (int)((double)dimension * 0.5);
        sigma = IOHprofiler_allocate_int_vector((size_t)dummy[0]);
        xins = IOHprofiler_allocate_vector((size_t)dummy[0]);
        IOHprofiler_compute_xopt_double(xins,rseed,(size_t)dummy[0]);
        for(i = 0; i < dummy[0]; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dummy[0]; i++){
            t = (int)(xins[i] * (double)dummy[0]);
            assert(t >= 0 && t < dummy[0]);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_dummy_sigma(problem, dummy, sigma, 0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);

        IOHprofiler_free_memory(sigma);
        IOHprofiler_free_memory(xins);
    } else {
        dummy[0] = (int)((double)dimension * 0.5);
        problem = transform_vars_dummy(problem, dummy, 0);
    }
    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");

    IOHprofiler_free_memory(dummy);
    return problem;
}
#line 16 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_one_max_dummy2.c"
/**
 * @Onemax function
 */

/**
 * @file f_one_max_dummy2.c
 * @brief Implementation of the one_max_dummy2 function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 14 "code-experiments/src/f_one_max_dummy2.c"
#line 15 "code-experiments/src/f_one_max_dummy2.c"
#line 16 "code-experiments/src/f_one_max_dummy2.c"
#line 17 "code-experiments/src/f_one_max_dummy2.c"
#line 18 "code-experiments/src/f_one_max_dummy2.c"
#line 19 "code-experiments/src/f_one_max_dummy2.c"
#line 20 "code-experiments/src/f_one_max_dummy2.c"
#line 21 "code-experiments/src/f_one_max_dummy2.c"

/**
 * @brief Implements the one_max_dummy2 function without connections to any IOHprofiler structures.
 */
static double f_one_max_dummy2_raw(const int *x, const size_t number_of_variables) {
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
static void f_one_max_dummy2_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_one_max_dummy2_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic one_max_dummy2 problem.
 */
static IOHprofiler_problem_t *f_one_max_dummy2_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("one_max_dummy2 function",
                                                                               f_one_max_dummy2_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "one_max_dummy2", number_of_variables);

    /* Compute best solution */
    f_one_max_dummy2_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler one_max_dummy2 problem.
 */
static IOHprofiler_problem_t *f_one_max_dummy2_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {

    int * dummy;
    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);


    dummy = IOHprofiler_allocate_int_vector(1);
    problem = f_one_max_dummy2_allocate(dimension);
    if(instance == 1){
        dummy[0] = (int)((double)dimension * 0.9);
        problem = transform_vars_dummy(problem, dummy, 0);
    }
    else if(instance > 1 && instance <= 50){
        dummy[0] = (int)((double)dimension * 0.9);
        z = IOHprofiler_allocate_int_vector((size_t)dummy[0]);
        IOHprofiler_compute_xopt(z,rseed,(size_t)dummy[0]);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_dummy_xor(problem,dummy,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);

        IOHprofiler_free_memory(z);
    }
    else if(instance > 50 && instance <= 100)
    {

        dummy[0] = (int)((double)dimension * 0.9);
        sigma = IOHprofiler_allocate_int_vector((size_t)dummy[0]);
        xins = IOHprofiler_allocate_vector((size_t)dummy[0]);
        IOHprofiler_compute_xopt_double(xins,rseed,(size_t)dummy[0]);
        for(i = 0; i < dummy[0]; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dummy[0]; i++){
            t = (int)(xins[i] * (double)dummy[0]);
            assert(t >= 0 && t < dummy[0]);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_dummy_sigma(problem, dummy, sigma, 0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
        IOHprofiler_free_memory(sigma);
        IOHprofiler_free_memory(xins);
    } else {
        dummy[0] = (int)((double)dimension * 0.9);
        problem = transform_vars_dummy(problem, dummy, 0);
    }
    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");

    IOHprofiler_free_memory(dummy);

    return problem;
}
#line 17 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_one_max_epistasis.c"
/**
 * @Onemax function
 */

/**
 * @file f_one_max_epistasis.c
 * @brief Implementation of the one_max_epistasis function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 14 "code-experiments/src/f_one_max_epistasis.c"
#line 15 "code-experiments/src/f_one_max_epistasis.c"
#line 16 "code-experiments/src/f_one_max_epistasis.c"
#line 17 "code-experiments/src/f_one_max_epistasis.c"
#line 1 "code-experiments/src/transform_vars_epistasis.c"
/**
 * @file transform_vars_epistasis.c
 * @brief Implementation of epistasis all decision values by an offset.
 */

#include <assert.h>

#line 9 "code-experiments/src/transform_vars_epistasis.c"
#line 10 "code-experiments/src/transform_vars_epistasis.c"

/**
 * @brief Data type for transform_vars_epistasis.
 */
typedef struct {
  int *offset;
  int *epistasis_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_epistasis_data_t;


static void epistasis_compute(const int *x, int * epistasis_x,  int block_size, const size_t dimension){
  int i,j,h,epistasis_result;

  h = 0;
  while(h + block_size - 1 < dimension){
    i = 0;
    while(i < block_size){
      epistasis_result = -1;
      for(j = 0; j < block_size; ++j){
        if( (block_size - j - 1) != ((block_size - i - 1) - 1) % 4){
          if(epistasis_result == -1){
            epistasis_result = x[h+j];
          }
          else{
            epistasis_result = (epistasis_result != x[h+j]);
          }
        }
      }
      epistasis_x[h+i] = epistasis_result;
      ++i;
    }
    h += block_size;
  }
  if((int)dimension - h > 0){
    block_size = (int)dimension - h;
    i = 0;
    while(i < block_size){
      epistasis_result = -1;
      for(j = 0; j < block_size; ++j){
        if( (block_size - j - 1) != ((block_size - i - 1) - 1) % 4){
          if(epistasis_result == -1){
            epistasis_result = x[h+j];
          }
          else{
            epistasis_result = (epistasis_result != x[h+j]);
          }
        }
      }
      epistasis_x[h+i] = epistasis_result;
      ++i;
    }
  }
}

static void transform_vars_epistasis_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {

  transform_vars_epistasis_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (transform_vars_epistasis_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);

  epistasis_compute(x, data->epistasis_x, data->offset[0], problem->number_of_variables);

  IOHprofiler_evaluate_function(inner_problem, data->epistasis_x, y);
  problem->raw_fitness[0] = y[0];

  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_epistasis_free(void *thing) {
  transform_vars_epistasis_data_t *data = (transform_vars_epistasis_data_t *) thing;
  IOHprofiler_free_memory(data->epistasis_x);
  IOHprofiler_free_memory(data->offset);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_epistasis(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int epistasis_bounds) {

  transform_vars_epistasis_data_t *data;
  IOHprofiler_problem_t *problem;

  if (epistasis_bounds)
    IOHprofiler_error("epistasis_bounds not implemented.");


  data = (transform_vars_epistasis_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, 1);
  data->epistasis_x = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);


  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_epistasis_free, "transform_vars_epistasis");
  problem->evaluate_function = transform_vars_epistasis_evaluate;
  /* Compute best parameter */
  /*Best parameter will not change with this transformation*/

  return problem;
}
#line 18 "code-experiments/src/f_one_max_epistasis.c"
#line 19 "code-experiments/src/f_one_max_epistasis.c"
#line 1 "code-experiments/src/transform_vars_epistasis_xor.c"
/**
 * @file transform_vars_epistasis_xor.c
 * @brief Implementation of epistasis_xor all decision values by an offset.
 */

#include <assert.h>

#line 9 "code-experiments/src/transform_vars_epistasis_xor.c"
#line 10 "code-experiments/src/transform_vars_epistasis_xor.c"

/**
 * @brief Data type for transform_vars_epistasis_xor.
 */
typedef struct {
  int *offset;
  int *offset1;
  int *epistasis_x;
  int *epistasis_xor_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_epistasis_xor_data_t;


static void epistasis_compute_beforexor(const int *x, int * epistasis_x,  int block_size, const size_t dimension){
  int i,j,h,epistasis_result;

  h = 0;
  while(h + block_size - 1 < dimension){
    i = 0;
    while(i < block_size){
      epistasis_result = -1;
      for(j = 0; j < block_size; ++j){
        if( (block_size - j - 1) != ((block_size - i - 1) - 1) % 4){
          if(epistasis_result == -1){
            epistasis_result = x[h+j];
          }
          else{
            epistasis_result = (epistasis_result != x[h+j]);
          }
        }
      }
      epistasis_x[h+i] = epistasis_result;
      ++i;
    }
    h += block_size;
  }
  if((int)dimension - h > 0){
    block_size = (int)dimension - h;
    i = 0;
    while(i < block_size){
      epistasis_result = -1;
      for(j = 0; j < block_size; ++j){
        if( (block_size - j - 1) != ((block_size - i - 1) - 1) % 4){
          if(epistasis_result == -1){
            epistasis_result = x[h+j];
          }
          else{
            epistasis_result = (epistasis_result != x[h+j]);
          }
        }
      }
      epistasis_x[h+i] = epistasis_result;
      ++i;
    }
  }
}

static int epistasis_xor_compute(const int x1, const int x2){
    return (int) x1 != x2;
}


static void transform_vars_epistasis_xor_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  transform_vars_epistasis_xor_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (transform_vars_epistasis_xor_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);

  epistasis_compute_beforexor(x, data->epistasis_x, data->offset[0], problem->number_of_variables);
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->epistasis_xor_x[i] = epistasis_xor_compute(data->epistasis_x[i],data->offset1[i]);
  }

  IOHprofiler_evaluate_function(inner_problem, data->epistasis_xor_x, y);
  problem->raw_fitness[0] = y[0];

  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_epistasis_xor_free(void *thing) {
  transform_vars_epistasis_xor_data_t *data = (transform_vars_epistasis_xor_data_t *) thing;
  IOHprofiler_free_memory(data->epistasis_xor_x);
  IOHprofiler_free_memory(data->offset);
  IOHprofiler_free_memory(data->offset1);
  IOHprofiler_free_memory(data->epistasis_x);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_epistasis_xor(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int *offset1,
                                            const int epistasis_xor_bounds) {

  transform_vars_epistasis_xor_data_t *data;
  IOHprofiler_problem_t *problem;

  if (epistasis_xor_bounds)
    IOHprofiler_error("epistasis_xor_bounds not implemented.");


  data = (transform_vars_epistasis_xor_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, 1);
  data->offset1 = IOHprofiler_duplicate_int_vector(offset1, inner_problem->number_of_variables);
  data->epistasis_x = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);
  data->epistasis_xor_x = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);



  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_epistasis_xor_free, "transform_vars_epistasis_xor");
  problem->evaluate_function = transform_vars_epistasis_xor_evaluate;
  /* Compute best parameter */
  /*Best parameter will not change with this transformation*/

  return problem;
}
#line 20 "code-experiments/src/f_one_max_epistasis.c"
#line 1 "code-experiments/src/transform_vars_epistasis_sigma.c"
/**
 * @file transform_vars_epistasis_sigma.c
 * @brief Implementation of epistasis_sigma all decision values by an offset.
 */

#include <assert.h>

#line 9 "code-experiments/src/transform_vars_epistasis_sigma.c"
#line 10 "code-experiments/src/transform_vars_epistasis_sigma.c"

/**
 * @brief Data type for transform_vars_epistasis_sigma.
 */
typedef struct {
  int *offset;
  int *offset1;
  int *epistasis_x;
  int *epistasis_sigma_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_epistasis_sigma_data_t;


static void epistasis_compute_beforesigma(const int *x, int * epistasis_x,  int block_size, const size_t dimension){
  int i,j,h,epistasis_result;

  h = 0;
  while(h + block_size - 1 < dimension){
    i = 0;
    while(i < block_size){
      epistasis_result = -1;
      for(j = 0; j < block_size; ++j){
        if( (block_size - j - 1) != ((block_size - i - 1) - 1) % 4){
          if(epistasis_result == -1){
            epistasis_result = x[h+j];
          }
          else{
            epistasis_result = (epistasis_result != x[h+j]);
          }
        }
      }
      epistasis_x[h+i] = epistasis_result;
      ++i;
    }
    h += block_size;
  }
  if((int)dimension - h > 0){
    block_size = (int)dimension - h;
    i = 0;
    while(i < block_size){
      epistasis_result = -1;
      for(j = 0; j < block_size; ++j){
        if( (block_size - j - 1) != ((block_size - i - 1) - 1) % 4){
          if(epistasis_result == -1){
            epistasis_result = x[h+j];
          }
          else{
            epistasis_result = (epistasis_result != x[h+j]);
          }
        }
      }
      epistasis_x[h+i] = epistasis_result;
      ++i;
    }
  }
}

static int epistasis_sigma_compute(const int *x, const int pos){
  return x[pos];
}


static void transform_vars_epistasis_sigma_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;
  transform_vars_epistasis_sigma_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (transform_vars_epistasis_sigma_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);

  epistasis_compute_beforesigma(x, data->epistasis_x, data->offset[0], problem->number_of_variables);
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->epistasis_sigma_x[i] = epistasis_sigma_compute(data->epistasis_x,data->offset1[i]);
  }

  IOHprofiler_evaluate_function(inner_problem, data->epistasis_sigma_x, y);
  problem->raw_fitness[0] = y[0];

  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_epistasis_sigma_free(void *thing) {
  transform_vars_epistasis_sigma_data_t *data = (transform_vars_epistasis_sigma_data_t *) thing;
  IOHprofiler_free_memory(data->epistasis_sigma_x);
  IOHprofiler_free_memory(data->offset);
  IOHprofiler_free_memory(data->offset1);
  IOHprofiler_free_memory(data->epistasis_x);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_epistasis_sigma(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int *offset1,
                                            const int epistasis_sigma_bounds) {

  transform_vars_epistasis_sigma_data_t *data;
  IOHprofiler_problem_t *problem;

  if (epistasis_sigma_bounds)
    IOHprofiler_error("epistasis_sigma_bounds not implemented.");


  data = (transform_vars_epistasis_sigma_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, 1);
  data->offset1 = IOHprofiler_duplicate_int_vector(offset1, inner_problem->number_of_variables);
  data->epistasis_x = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);
  data->epistasis_sigma_x = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);



  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_epistasis_sigma_free, "transform_vars_epistasis_sigma");
  problem->evaluate_function = transform_vars_epistasis_sigma_evaluate;
  /* Compute best parameter */
  /*Best parameter will not change with this transformation*/

  return problem;
}
#line 21 "code-experiments/src/f_one_max_epistasis.c"
/**
 * @brief Implements the one_max_epistasis function without connections to any IOHprofiler structures.
 */
static double f_one_max_epistasis_raw(const int *x, const size_t number_of_variables) {
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
static void f_one_max_epistasis_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_one_max_epistasis_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic one_max_epistasis problem.
 */
static IOHprofiler_problem_t *f_one_max_epistasis_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("one_max_epistasis function",
                                                                               f_one_max_epistasis_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "one_max_epistasis", number_of_variables);

    /* Compute best solution */
    f_one_max_epistasis_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler one_max_epistasis problem.
 */
static IOHprofiler_problem_t *f_one_max_epistasis_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {

    int * epistasis;
    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);
    sigma = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);

    epistasis = IOHprofiler_allocate_int_vector(1);
    problem = f_one_max_epistasis_allocate(dimension);
    if(instance == 1){
        epistasis[0] = 4;
        problem = transform_vars_epistasis(problem, epistasis,0);
    }
    else if(instance > 1 && instance <= 50){
        epistasis[0] = 4;


        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_vars_epistasis_xor(problem,epistasis, z, 0);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        epistasis[0] = 4;

        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_epistasis_sigma(problem,epistasis, sigma, 0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    } else {
        epistasis[0] = 4;
        problem = transform_vars_epistasis(problem, epistasis,0);
    }
    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");

    IOHprofiler_free_memory(epistasis);
    IOHprofiler_free_memory(z);
    IOHprofiler_free_memory(sigma);
    IOHprofiler_free_memory(xins);
    return problem;
}
#line 18 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_one_max_neutrality.c"
/**
 * @Onemax function
 */

/**
 * @file f_one_max_neutrality.c
 * @brief Implementation of the one_max_neutrality function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 14 "code-experiments/src/f_one_max_neutrality.c"
#line 15 "code-experiments/src/f_one_max_neutrality.c"
#line 16 "code-experiments/src/f_one_max_neutrality.c"
#line 17 "code-experiments/src/f_one_max_neutrality.c"
#line 1 "code-experiments/src/transform_vars_neutrality.c"
/**
 * @file transform_vars_neutrality.c
 * @brief Implementation of neutrality all decision values by an offset.
 */

#include <assert.h>

#line 9 "code-experiments/src/transform_vars_neutrality.c"
#line 10 "code-experiments/src/transform_vars_neutrality.c"

/**
 * @brief Data type for transform_vars_neutrality.
 */
typedef struct {
  int *offset;
  int *neutrality_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_neutrality_data_t;

static int neutrality_compute(const int *x, const int index, const int block_size){
  int number_of_one, number_of_zero, i;
  number_of_zero = 0;
  number_of_one = 0;
  i = 0;
  while(i < block_size){
    if(x[index + i] == 0){
      number_of_zero++;
    }
    if(x[index + i] == 1){
      number_of_one++;
    }
    ++i;
  }
  return (number_of_zero >= number_of_one ? 0 : 1);
}

static void transform_vars_neutrality_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  int i;
  transform_vars_neutrality_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (transform_vars_neutrality_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);

  for (i = 0; i < problem->number_of_variables; ++i) {
    data->neutrality_x[i] = neutrality_compute(x, i*data->offset[0], data->offset[0]);
  }
  IOHprofiler_evaluate_function(inner_problem, data->neutrality_x, y);
  problem->raw_fitness[0] = y[0];

  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_neutrality_free(void *thing) {
  transform_vars_neutrality_data_t *data = (transform_vars_neutrality_data_t *) thing;
  IOHprofiler_free_memory(data->neutrality_x);
  IOHprofiler_free_memory(data->offset);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_neutrality(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int neutrality_bounds) {

  transform_vars_neutrality_data_t *data;
  IOHprofiler_problem_t *problem;
  int i;
  size_t new_dimension;
  if (neutrality_bounds)
    IOHprofiler_error("neutrality_bounds not implemented.");

  if(inner_problem->number_of_variables >= offset[0])
  {
    new_dimension = inner_problem->number_of_variables / (size_t)offset[0];
  }
  else{
    return inner_problem;
  }

  assert(new_dimension > 0);
  data = (transform_vars_neutrality_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, inner_problem->number_of_variables);
  data->neutrality_x = IOHprofiler_allocate_int_vector(new_dimension);


  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_neutrality_free, "transform_vars_neutrality");
  problem->number_of_variables = new_dimension;
  inner_problem->number_of_variables = problem->number_of_variables;
  problem->evaluate_function = transform_vars_neutrality_evaluate;
  while(inner_problem->data != NULL){
    inner_problem = IOHprofiler_problem_transformed_get_inner_problem(inner_problem);
    inner_problem->number_of_variables = problem->number_of_variables;
  }
  /* Compute best parameter */
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = neutrality_compute(inner_problem->best_parameter, i*data->offset[0], data->offset[0]);
  }
  problem->evaluate_function(problem, problem->best_parameter, problem->best_value);
  return problem;
}
#line 18 "code-experiments/src/f_one_max_neutrality.c"
#line 19 "code-experiments/src/f_one_max_neutrality.c"
#line 1 "code-experiments/src/transform_vars_neutrality_sigma.c"
/**
 * @file transform_vars_neutrality_sigma_sigma.c
 * @brief Implementation of neutrality_sigma all decision values by an offset.
 */

#include <assert.h>

#line 9 "code-experiments/src/transform_vars_neutrality_sigma.c"
#line 10 "code-experiments/src/transform_vars_neutrality_sigma.c"

/**
 * @brief Data type for transform_vars_neutrality_sigma.
 */
typedef struct {
  int *offset;
  int *offset1;
  int *neutrality_x;
  int *sigma_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_neutrality_sigma_data_t;

static int neutrality_compute_before_sigma(const int *x, const int index, const int block_size){
  int number_of_one, number_of_zero, i;
  number_of_zero = 0;
  number_of_one = 0;
  i = 0;
  while(i < block_size){
    if(x[index + i] == 0){
      number_of_zero++;
    }
    if(x[index + i] == 1){
      number_of_one++;
    }
    ++i;
  }
  return (number_of_zero >= number_of_one ? 0 : 1);
}

static int neutrality_sigma_compute(const int *x, const int pos){
  return x[pos];
}

static void transform_vars_neutrality_sigma_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  int i;
  transform_vars_neutrality_sigma_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (transform_vars_neutrality_sigma_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);

  for (i = 0; i < problem->number_of_variables; ++i) {
    data->neutrality_x[i] = neutrality_compute_before_sigma(x, i*data->offset[0], data->offset[0]);
  }
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->sigma_x[i] = neutrality_sigma_compute(data->neutrality_x,data->offset1[i]);
  }
  IOHprofiler_evaluate_function(inner_problem, data->sigma_x, y);
  problem->raw_fitness[0] = y[0];

  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_neutrality_sigma_free(void *thing) {
  transform_vars_neutrality_sigma_data_t *data = (transform_vars_neutrality_sigma_data_t *) thing;
  IOHprofiler_free_memory(data->neutrality_x);
  IOHprofiler_free_memory(data->offset);
  IOHprofiler_free_memory(data->offset1);
  IOHprofiler_free_memory(data->sigma_x);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_neutrality_sigma(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int *offset1,
                                            const int neutrality_sigma_bounds) {

  transform_vars_neutrality_sigma_data_t *data;
  IOHprofiler_problem_t *problem;
  int * temp_best;
  int i;
  size_t new_dimension;
  if (neutrality_sigma_bounds)
    IOHprofiler_error("neutrality_sigma_bounds not implemented.");

  if(inner_problem->number_of_variables >= offset[0])
  {
    new_dimension = inner_problem->number_of_variables / (size_t)offset[0];
  }
  else{
    return inner_problem;
  }

  assert(new_dimension > 0);
  data = (transform_vars_neutrality_sigma_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, 1);
  data->offset1 = IOHprofiler_duplicate_int_vector(offset1,new_dimension);
  data->neutrality_x = IOHprofiler_allocate_int_vector(new_dimension);
  data->sigma_x = IOHprofiler_allocate_int_vector(new_dimension);
  temp_best = IOHprofiler_allocate_int_vector(new_dimension);


  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_neutrality_sigma_free, "transform_vars_neutrality_sigma");
  problem->number_of_variables = new_dimension;
  inner_problem->number_of_variables = problem->number_of_variables;
  problem->evaluate_function = transform_vars_neutrality_sigma_evaluate;
  while(inner_problem->data != NULL){
    inner_problem = IOHprofiler_problem_transformed_get_inner_problem(inner_problem);
    inner_problem->number_of_variables = problem->number_of_variables;
  }
  /* Compute best parameter */
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = neutrality_compute_before_sigma(inner_problem->best_parameter, i*data->offset[0], data->offset[0]);
  }
  for (i = 0; i < problem->number_of_variables; i++) {
      temp_best[i] = neutrality_sigma_compute(problem->best_parameter,data->offset1[i]);
  }
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = temp_best[i];
  }
  IOHprofiler_evaluate_function(inner_problem, problem->best_parameter, problem->best_value);
  IOHprofiler_free_memory(temp_best);
  return problem;
}
#line 20 "code-experiments/src/f_one_max_neutrality.c"
#line 1 "code-experiments/src/transform_vars_neutrality_xor.c"
/**
 * @file transform_vars_neutrality_xor.c
 * @brief Implementation of neutrality_xor all decision values by an offset.
 */

#include <assert.h>

#line 9 "code-experiments/src/transform_vars_neutrality_xor.c"
#line 10 "code-experiments/src/transform_vars_neutrality_xor.c"

/**
 * @brief Data type for transform_vars_neutrality_xor.
 */
typedef struct {
  int *offset;
  int *offset1;
  int *neutrality_x;
  int *xor_x;
  IOHprofiler_problem_free_function_t old_free_problem;
} transform_vars_neutrality_xor_data_t;

static int neutrality_compute_before_xor(const int *x, const int index, const int block_size){
  int number_of_one, number_of_zero, i;
  number_of_zero = 0;
  number_of_one = 0;
  i = 0;
  while(i < block_size){
    if(x[index + i] == 0){
      number_of_zero++;
    }
    if(x[index + i] == 1){
      number_of_one++;
    }
    ++i;
  }
  return (number_of_zero >= number_of_one ? 0 : 1);
}

static int neutrality_xor_compute(const int x1, const int x2){
    return (int) x1 != x2;
}

static void transform_vars_neutrality_xor_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  int i;
  transform_vars_neutrality_xor_data_t *data;
  IOHprofiler_problem_t *inner_problem;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }
  data = (transform_vars_neutrality_xor_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);

  for (i = 0; i < problem->number_of_variables; ++i) {
    data->neutrality_x[i] = neutrality_compute_before_xor(x, i*data->offset[0], data->offset[0]);
  }
  for (i = 0; i < problem->number_of_variables; ++i) {
    data->xor_x[i] = neutrality_xor_compute(data->neutrality_x[i],data->offset1[i]);
  }
  IOHprofiler_evaluate_function(inner_problem, data->xor_x, y);
  problem->raw_fitness[0] = y[0];
  assert(y[0]  <= problem->best_value[0]);
}

/**
 * @brief Frees the data object.
 */
static void transform_vars_neutrality_xor_free(void *thing) {
  transform_vars_neutrality_xor_data_t *data = (transform_vars_neutrality_xor_data_t *) thing;
  IOHprofiler_free_memory(data->neutrality_x);
  IOHprofiler_free_memory(data->offset);
  IOHprofiler_free_memory(data->offset1);
  IOHprofiler_free_memory(data->xor_x);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_vars_neutrality_xor(IOHprofiler_problem_t *inner_problem,
                                            const int *offset,
                                            const int *offset1,
                                            const int neutrality_xor_bounds) {

  transform_vars_neutrality_xor_data_t *data;
  IOHprofiler_problem_t *problem;
  int i;
  size_t new_dimension;
  if (neutrality_xor_bounds)
    IOHprofiler_error("neutrality_xor_bounds not implemented.");

  if(inner_problem->number_of_variables >= offset[0])
  {
    new_dimension = inner_problem->number_of_variables / (size_t)offset[0];
  }
  else{
    return inner_problem;
  }

  assert(new_dimension > 0);
  data = (transform_vars_neutrality_xor_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->offset = IOHprofiler_duplicate_int_vector(offset, 1);
  data->offset1 = IOHprofiler_duplicate_int_vector(offset1,new_dimension);
  data->neutrality_x = IOHprofiler_allocate_int_vector(new_dimension);
  data->xor_x = IOHprofiler_allocate_int_vector(new_dimension);


  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, transform_vars_neutrality_xor_free, "transform_vars_neutrality_xor");
  problem->number_of_variables = new_dimension;
  inner_problem->number_of_variables = problem->number_of_variables;
  problem->evaluate_function = transform_vars_neutrality_xor_evaluate;
  while(inner_problem->data != NULL){
    inner_problem = IOHprofiler_problem_transformed_get_inner_problem(inner_problem);
    inner_problem->number_of_variables = problem->number_of_variables;
  }
  /* Compute best parameter */
  for (i = 0; i < problem->number_of_variables; i++) {
      problem->best_parameter[i] = neutrality_compute_before_xor(inner_problem->best_parameter, i*data->offset[0], data->offset[0]);
  }

  IOHprofiler_evaluate_function(inner_problem, problem->best_parameter, problem->best_value);
  return problem;
}
#line 21 "code-experiments/src/f_one_max_neutrality.c"
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
#line 19 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_one_max_ruggedness1.c"
/**
 * @Onemax function
 */

/**
 * @file f_one_max_ruggedness1.c
 * @brief Implementation of the one_max_ruggedness1 function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 14 "code-experiments/src/f_one_max_ruggedness1.c"
#line 15 "code-experiments/src/f_one_max_ruggedness1.c"
#line 16 "code-experiments/src/f_one_max_ruggedness1.c"
#line 17 "code-experiments/src/f_one_max_ruggedness1.c"
#line 18 "code-experiments/src/f_one_max_ruggedness1.c"
#line 1 "code-experiments/src/transform_obj_ruggedness1.c"
/**
 * @file transform_obj_ruggedness1.c
 * @brief Implementation of ruggedness1ing the objective value by the given offset.
 */

#include <assert.h>

#line 9 "code-experiments/src/transform_obj_ruggedness1.c"
#line 10 "code-experiments/src/transform_obj_ruggedness1.c"

/**
 * @brief Data type for transform_obj_ruggedness1.
 */
typedef struct {
  double offset;
} transform_obj_ruggedness1_data_t;

double compute_ruggedness(double y, size_t dimension){
  double ruggedness_y, s;
  s = (double)dimension;
  if(y == s){
    ruggedness_y = ceil(y/2.0) + 1.0;
  }
  else if(y < s && dimension % 2 == 0){
    ruggedness_y = floor(y / 2.0) + 1.0;
  }
  else if(y < s && dimension % 2 != 0){
    ruggedness_y = ceil(y / 2.0) + 1.0;
  }
  else{
    ruggedness_y = y;
    assert(y <= s);
  }

  return ruggedness_y;
}

/**
 * @brief Evaluates the transformation.
 */
static void transform_obj_ruggedness1_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }

  IOHprofiler_evaluate_function(IOHprofiler_problem_transformed_get_inner_problem(problem), x, y);
  for (i = 0; i < problem->number_of_objectives; i++) {
      y[i] = compute_ruggedness(y[i],problem->number_of_variables);
      problem->raw_fitness[i] = y[i];

  }
  assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_obj_ruggedness1(IOHprofiler_problem_t *inner_problem) {
  IOHprofiler_problem_t *problem;
  transform_obj_ruggedness1_data_t *data;
  size_t i;
  data = (transform_obj_ruggedness1_data_t *) IOHprofiler_allocate_memory(sizeof(*data));

  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, NULL, "transform_obj_ruggedness1");
  problem->evaluate_function = transform_obj_ruggedness1_evaluate;
  for (i = 0; i < problem->number_of_objectives; i++) {
      problem->best_value[i] = compute_ruggedness(problem->best_value[i],problem->number_of_variables);
  }
  return problem;
}
#line 19 "code-experiments/src/f_one_max_ruggedness1.c"
/**
 * @brief Implements the one_max_ruggedness1 function without connections to any IOHprofiler structures.
 */
static double f_one_max_ruggedness1_raw(const int *x, const size_t number_of_variables) {
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
static void f_one_max_ruggedness1_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_one_max_ruggedness1_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic one_max_ruggedness1 problem.
 */
static IOHprofiler_problem_t *f_one_max_ruggedness1_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("one_max_ruggedness1 function",
                                                                               f_one_max_ruggedness1_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "one_max_ruggedness1", number_of_variables);

    /* Compute best solution */
    f_one_max_ruggedness1_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler one_max_ruggedness1 problem.
 */
static IOHprofiler_problem_t *f_one_max_ruggedness1_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {
    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);
    sigma = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);

    problem = f_one_max_ruggedness1_allocate(dimension);
    if(instance == 1){
       problem = transform_obj_ruggedness1(problem);
    }
    else if(instance > 1 && instance <= 50){

        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_ruggedness1(problem);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {

        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_sigma(problem, sigma, 0);
        assert(a <= 5.0 && a >= 0.2);

        problem = transform_obj_ruggedness1(problem);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    } else {
        problem = transform_obj_ruggedness1(problem);
    }
    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");

    IOHprofiler_free_memory(z);
    IOHprofiler_free_memory(sigma);
    IOHprofiler_free_memory(xins);
    return problem;
}
#line 20 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_one_max_ruggedness2.c"
/**
 * @Onemax function
 */

/**
 * @file f_one_max_ruggedness2.c
 * @brief Implementation of the one_max_ruggedness2 function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 14 "code-experiments/src/f_one_max_ruggedness2.c"
#line 15 "code-experiments/src/f_one_max_ruggedness2.c"
#line 16 "code-experiments/src/f_one_max_ruggedness2.c"
#line 17 "code-experiments/src/f_one_max_ruggedness2.c"
#line 18 "code-experiments/src/f_one_max_ruggedness2.c"
#line 1 "code-experiments/src/transform_obj_ruggedness2.c"
/**
 * @file transform_obj_ruggedness2.c
 * @brief Implementation of ruggedness2ing the objective value by the given offset.
 */

#include <assert.h>

#line 9 "code-experiments/src/transform_obj_ruggedness2.c"
#line 10 "code-experiments/src/transform_obj_ruggedness2.c"

/**
 * @brief Data type for transform_obj_ruggedness2.
 */
typedef struct {
  double offset;
} transform_obj_ruggedness2_data_t;

double compute_ruggedness2(double y, size_t dimension){
  double ruggedness_y;
  int tempy=(int)(y+0.5);
  if(tempy == dimension){
    ruggedness_y = y;
  }
  else if(tempy < dimension && tempy % 2 == 0 && dimension % 2 == 0){
    ruggedness_y = y + 1.0;
  }
  else if(tempy < dimension && tempy % 2 == 0 && dimension % 2 != 0){
    ruggedness_y = (y - 1.0) > 0 ? (y - 1.0) : 0;
  }
  else if(tempy < dimension && tempy % 2 != 0 && dimension % 2 == 0){
    ruggedness_y = (y - 1.0) > 0 ? (y - 1.0) : 0;
  }
  else if(tempy < dimension && tempy % 2 != 0 && dimension % 2 != 0){
    ruggedness_y = y + 1.0;
  }
  else{
    ruggedness_y = y;
    assert(tempy <= dimension);
  }

  return ruggedness_y;
}

/**
 * @brief Evaluates the transformation.
 */
static void transform_obj_ruggedness2_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  size_t i;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }

  IOHprofiler_evaluate_function(IOHprofiler_problem_transformed_get_inner_problem(problem), x, y);
  for (i = 0; i < problem->number_of_objectives; i++) {
      y[i] = compute_ruggedness2(y[i],problem->number_of_variables);
      problem->raw_fitness[i] = y[i];

  }
  assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_obj_ruggedness2(IOHprofiler_problem_t *inner_problem) {
  IOHprofiler_problem_t *problem;
  transform_obj_ruggedness2_data_t *data;
  size_t i;
  data = (transform_obj_ruggedness2_data_t *) IOHprofiler_allocate_memory(sizeof(*data));

  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, NULL, "transform_obj_ruggedness2");
  problem->evaluate_function = transform_obj_ruggedness2_evaluate;
  for (i = 0; i < problem->number_of_objectives; i++) {
      problem->best_value[i] = compute_ruggedness2(problem->best_value[i],problem->number_of_variables);
  }
  return problem;
}
#line 19 "code-experiments/src/f_one_max_ruggedness2.c"
/**
 * @brief Implements the one_max_ruggedness2 function without connections to any IOHprofiler structures.
 */
static double f_one_max_ruggedness2_raw(const int *x, const size_t number_of_variables) {
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
static void f_one_max_ruggedness2_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_one_max_ruggedness2_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic one_max_ruggedness2 problem.
 */
static IOHprofiler_problem_t *f_one_max_ruggedness2_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("one_max_ruggedness2 function",
                                                                               f_one_max_ruggedness2_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "one_max_ruggedness2", number_of_variables);

    /* Compute best solution */
    f_one_max_ruggedness2_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler one_max_ruggedness2 problem.
 */
static IOHprofiler_problem_t *f_one_max_ruggedness2_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {
    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);
    sigma = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);

    problem = f_one_max_ruggedness2_allocate(dimension);
    if(instance == 1){
       problem = transform_obj_ruggedness2(problem);
    }
    else if(instance > 1 && instance <= 50){
        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);

        problem = transform_obj_ruggedness2(problem);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_sigma(problem, sigma, 0);
        assert(a <= 5.0 && a >= 0.2);

        problem = transform_obj_ruggedness2(problem);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    } else {
        problem = transform_obj_ruggedness2(problem);
    }
    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");

    IOHprofiler_free_memory(z);
    IOHprofiler_free_memory(sigma);
    IOHprofiler_free_memory(xins);
    return problem;
}
#line 21 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_one_max_ruggedness3.c"
/**
 * @Onemax function
 */

/**
 * @file f_one_max_ruggedness3.c
 * @brief Implementation of the one_max_ruggedness3 function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 14 "code-experiments/src/f_one_max_ruggedness3.c"
#line 15 "code-experiments/src/f_one_max_ruggedness3.c"
#line 16 "code-experiments/src/f_one_max_ruggedness3.c"
#line 17 "code-experiments/src/f_one_max_ruggedness3.c"
#line 18 "code-experiments/src/f_one_max_ruggedness3.c"
#line 1 "code-experiments/src/transform_obj_ruggedness3.c"
/**
 * @file transform_obj_ruggedness3.c
 * @brief Implementation of ruggedness3ing the objective value by the given offset.
 */

#include <assert.h>

#line 9 "code-experiments/src/transform_obj_ruggedness3.c"
#line 10 "code-experiments/src/transform_obj_ruggedness3.c"

/**
 * @brief Data type for transform_obj_ruggedness3.
 */
typedef struct {
  double offset;
  double *ruggedness_mapping;
} transform_obj_ruggedness3_data_t;


static void compute_ruggedness3(double * y, size_t dimension){
  size_t j,k;
  for(j = 1; j <= dimension/5; ++j){
    for(k = 0; k < 5; ++k){
      y[dimension-5*j+k] = (double)(dimension - 5 * j + (4-k));
    }
  }
  for(k = 0; k < dimension - dimension / 5 * 5; ++k){
    y[k] = (double)(dimension - dimension / 5 * 5 - 1 - k);
  }
  y[dimension] = (double)dimension;
}


/**
 * @brief Evaluates the transformation.
 */
static void transform_obj_ruggedness3_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
   transform_obj_ruggedness3_data_t *data;
  size_t i;

  if (IOHprofiler_vector_contains_nan(x, IOHprofiler_problem_get_dimension(problem))) {
    IOHprofiler_vector_set_to_nan(y, IOHprofiler_problem_get_number_of_objectives(problem));
    return;
  }

  data = (transform_obj_ruggedness3_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  IOHprofiler_evaluate_function(IOHprofiler_problem_transformed_get_inner_problem(problem), x, y);
  for (i = 0; i < problem->number_of_objectives; i++) {
      y[i] = data->ruggedness_mapping[(int)(y[i]+0.5)];
      problem->raw_fitness[i] = y[i];

  }
  assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Creates the transformation.
 */
static IOHprofiler_problem_t *transform_obj_ruggedness3(IOHprofiler_problem_t *inner_problem) {
   IOHprofiler_problem_t *problem;
  transform_obj_ruggedness3_data_t *data;

  size_t i;
  data = (transform_obj_ruggedness3_data_t *) IOHprofiler_allocate_memory(sizeof(*data));
  data->ruggedness_mapping = IOHprofiler_allocate_vector(inner_problem->number_of_variables+1);
  compute_ruggedness3(data->ruggedness_mapping,inner_problem->number_of_variables);


  problem = IOHprofiler_problem_transformed_allocate(inner_problem, data, NULL, "transform_obj_ruggedness3");
  problem->evaluate_function = transform_obj_ruggedness3_evaluate;

  for (i = 0; i < problem->number_of_objectives; i++) {
      problem->best_value[i] = (double)problem->number_of_variables;
  }

  return problem;
}
#line 19 "code-experiments/src/f_one_max_ruggedness3.c"


/**
 * @brief Implements the one_max_ruggedness3 function without connections to any IOHprofiler structures.
 */
static double f_one_max_ruggedness3_raw(const int *x, const size_t number_of_variables) {
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
static void f_one_max_ruggedness3_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_one_max_ruggedness3_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic one_max_ruggedness3 problem.
 */
static IOHprofiler_problem_t *f_one_max_ruggedness3_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("one_max_ruggedness3 function",
                                                                               f_one_max_ruggedness3_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "one_max_ruggedness3", number_of_variables);

    /* Compute best solution */
    f_one_max_ruggedness3_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler one_max_ruggedness3 problem.
 */
static IOHprofiler_problem_t *f_one_max_ruggedness3_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {
    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);
    sigma = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);

    problem = f_one_max_ruggedness3_allocate(dimension);
    if(instance == 1){
       problem = transform_obj_ruggedness3(problem);
    }
    else if(instance > 1 && instance <= 50){
        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_ruggedness3(problem);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_sigma(problem, sigma, 0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_ruggedness3(problem);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    } else {
        problem = transform_obj_ruggedness3(problem);
    }
    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");

    IOHprofiler_free_memory(z);
    IOHprofiler_free_memory(sigma);
    IOHprofiler_free_memory(xins);
    return problem;
}
#line 22 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_leading_ones_dummy1.c"
/**
 * @Onemax function
 */

/**
 * @file f_leading_ones_dummy1.c
 * @brief Implementation of the leading_ones_dummy1 function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 14 "code-experiments/src/f_leading_ones_dummy1.c"
#line 15 "code-experiments/src/f_leading_ones_dummy1.c"
#line 16 "code-experiments/src/f_leading_ones_dummy1.c"
#line 17 "code-experiments/src/f_leading_ones_dummy1.c"
#line 18 "code-experiments/src/f_leading_ones_dummy1.c"
#line 19 "code-experiments/src/f_leading_ones_dummy1.c"
#line 20 "code-experiments/src/f_leading_ones_dummy1.c"
#line 21 "code-experiments/src/f_leading_ones_dummy1.c"

/**
 * @brief Implements the leading_ones_dummy1 function without connections to any IOHprofiler structures.
 */
static double f_leading_ones_dummy1_raw(const int *x, const size_t number_of_variables) {
    size_t i = 0;
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
static void f_leading_ones_dummy1_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_leading_ones_dummy1_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic leading_ones_dummy1 problem.
 */
static IOHprofiler_problem_t *f_leading_ones_dummy1_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("leading_ones_dummy1 function",
                                                                               f_leading_ones_dummy1_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "leading_ones_dummy1", number_of_variables);

    /* Compute best solution */
    f_leading_ones_dummy1_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler leading_ones_dummy1 problem.
 */
static IOHprofiler_problem_t *f_leading_ones_dummy1_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {

    int *z, *sigma,*dummy;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;


    dummy = IOHprofiler_allocate_int_vector(1);
    problem = f_leading_ones_dummy1_allocate(dimension);

    if(instance == 1){
        dummy[0] = (int)((double)dimension * 0.5);
        problem = transform_vars_dummy(problem, dummy, 0);
    }
    else if(instance > 1 && instance <= 50){
        dummy[0] = (int)((double)dimension * 0.5);

        z = IOHprofiler_allocate_int_vector((size_t)dummy[0]);
        IOHprofiler_compute_xopt(z,rseed,(size_t)dummy[0]);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_dummy_xor(problem,dummy,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
        IOHprofiler_free_memory(z);
    }
    else if(instance > 50 && instance <= 100)
    {

        dummy[0] = (int)((double)dimension * 0.5);
        sigma = IOHprofiler_allocate_int_vector((size_t)dummy[0]);
        xins = IOHprofiler_allocate_vector((size_t)dummy[0]);
        IOHprofiler_compute_xopt_double(xins,rseed,(size_t)dummy[0]);
        for(i = 0; i < dummy[0]; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dummy[0]; i++){
            t = (int)(xins[i] * (double)dummy[0]);
            assert(t >= 0 && t < dummy[0]);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_dummy_sigma(problem, dummy, sigma, 0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
        IOHprofiler_free_memory(sigma);
        IOHprofiler_free_memory(xins);
    } else {
        dummy[0] = (int)((double)dimension * 0.5);
        problem = transform_vars_dummy(problem, dummy, 0);
    }
    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");

    IOHprofiler_free_memory(dummy);

    return problem;
}
#line 23 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_leading_ones_dummy2.c"
/**
 * @Onemax function
 */

/**
 * @file f_leading_ones_dummy2.c
 * @brief Implementation of the leading_ones_dummy2 function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 14 "code-experiments/src/f_leading_ones_dummy2.c"
#line 15 "code-experiments/src/f_leading_ones_dummy2.c"
#line 16 "code-experiments/src/f_leading_ones_dummy2.c"
#line 17 "code-experiments/src/f_leading_ones_dummy2.c"
#line 18 "code-experiments/src/f_leading_ones_dummy2.c"
#line 19 "code-experiments/src/f_leading_ones_dummy2.c"
#line 20 "code-experiments/src/f_leading_ones_dummy2.c"
#line 21 "code-experiments/src/f_leading_ones_dummy2.c"

/**
 * @brief Implements the leading_ones_dummy2 function without connections to any IOHprofiler structures.
 */
static double f_leading_ones_dummy2_raw(const int *x, const size_t number_of_variables) {
    size_t i = 0;
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
static void f_leading_ones_dummy2_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_leading_ones_dummy2_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic leading_ones_dummy2 problem.
 */
static IOHprofiler_problem_t *f_leading_ones_dummy2_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("leading_ones_dummy2 function",
                                                                               f_leading_ones_dummy2_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "leading_ones_dummy2", number_of_variables);

    /* Compute best solution */
    f_leading_ones_dummy2_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler leading_ones_dummy2 problem.
 */
static IOHprofiler_problem_t *f_leading_ones_dummy2_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {

    int *z, *sigma,*dummy;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;

    dummy = IOHprofiler_allocate_int_vector(1);
    problem = f_leading_ones_dummy2_allocate(dimension);
    if(instance == 1){
        dummy[0] = (int)((double)dimension * 0.9);
        problem = transform_vars_dummy(problem, dummy, 0);
    }
    else if(instance > 1 && instance <= 50){
        dummy[0] = (int)((double)dimension * 0.9);

        z = IOHprofiler_allocate_int_vector((size_t)dummy[0]);
        IOHprofiler_compute_xopt(z,rseed,(size_t)dummy[0]);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_dummy_xor(problem,dummy,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
        IOHprofiler_free_memory(z);
    }
    else if(instance > 50 && instance <= 100)
    {

        dummy[0] = (int)((double)dimension * 0.9);
        sigma = IOHprofiler_allocate_int_vector((size_t)dummy[0]);
        xins = IOHprofiler_allocate_vector((size_t)dummy[0]);
        IOHprofiler_compute_xopt_double(xins,rseed,(size_t)dummy[0]);
        for(i = 0; i < dummy[0]; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dummy[0]; i++){
            t = (int)(xins[i] * (double)dummy[0]);
            assert(t >= 0 && t < dummy[0]);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_dummy_sigma(problem, dummy, sigma, 0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
        IOHprofiler_free_memory(sigma);
        IOHprofiler_free_memory(xins);
    } else {
        dummy[0] = (int)((double)dimension * 0.9);
        problem = transform_vars_dummy(problem, dummy, 0);
    }
    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");

    IOHprofiler_free_memory(dummy);

    return problem;
}
#line 24 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_leading_ones_epistasis.c"
/**
 * @Onemax function
 */

/**
 * @file f_leading_ones_epistasis.c
 * @brief Implementation of the leading_ones_epistasis function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 14 "code-experiments/src/f_leading_ones_epistasis.c"
#line 15 "code-experiments/src/f_leading_ones_epistasis.c"
#line 16 "code-experiments/src/f_leading_ones_epistasis.c"
#line 17 "code-experiments/src/f_leading_ones_epistasis.c"
#line 18 "code-experiments/src/f_leading_ones_epistasis.c"
#line 19 "code-experiments/src/f_leading_ones_epistasis.c"
/**
 * @brief Implements the leading_ones_epistasis function without connections to any IOHprofiler structures.
 */
static double f_leading_ones_epistasis_raw(const int *x, const size_t number_of_variables) {
    size_t i = 0;
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
static void f_leading_ones_epistasis_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_leading_ones_epistasis_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic leading_ones_epistasis problem.
 */
static IOHprofiler_problem_t *f_leading_ones_epistasis_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("leading_ones_epistasis function",
                                                                               f_leading_ones_epistasis_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "leading_ones_epistasis", number_of_variables);

    /* Compute best solution */
    f_leading_ones_epistasis_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler leading_ones_epistasis problem.
 */
static IOHprofiler_problem_t *f_leading_ones_epistasis_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {

    int * epistasis;
    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);
    sigma = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);

    epistasis = IOHprofiler_allocate_int_vector(1);
    problem = f_leading_ones_epistasis_allocate(dimension);
    if(instance == 1){
        epistasis[0] = 4;
        problem = transform_vars_epistasis(problem, epistasis,0);
    }
    else if(instance > 1 && instance <= 50){
        epistasis[0] = 4;


        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_vars_epistasis_xor(problem,epistasis, z, 0);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        epistasis[0] = 4;

        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_epistasis_sigma(problem,epistasis, sigma, 0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    } else {
        epistasis[0] = 4;
        problem = transform_vars_epistasis(problem, epistasis,0);
    }
    IOHprofiler_problem_set_id(problem, problem_id_template, function, instance, dimension);
    IOHprofiler_problem_set_name(problem, problem_name_template, function, instance, dimension);
    IOHprofiler_problem_set_type(problem, "pseudo-Boolean");

    IOHprofiler_free_memory(epistasis);
    IOHprofiler_free_memory(z);
    IOHprofiler_free_memory(sigma);
    IOHprofiler_free_memory(xins);
    return problem;
}
#line 25 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_leading_ones_neutrality.c"
/**
 * @Onemax function
 */

/**
 * @file f_leading_ones_neutrality.c
 * @brief Implementation of the leading_ones_neutrality function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 14 "code-experiments/src/f_leading_ones_neutrality.c"
#line 15 "code-experiments/src/f_leading_ones_neutrality.c"
#line 16 "code-experiments/src/f_leading_ones_neutrality.c"
#line 17 "code-experiments/src/f_leading_ones_neutrality.c"
#line 18 "code-experiments/src/f_leading_ones_neutrality.c"
#line 19 "code-experiments/src/f_leading_ones_neutrality.c"
/**
 * @brief Implements the leading_ones_neutrality function without connections to any IOHprofiler structures.
 */
static double f_leading_ones_neutrality_raw(const int *x, const size_t number_of_variables) {
    size_t i = 0;
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
static void f_leading_ones_neutrality_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_leading_ones_neutrality_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic leading_ones_neutrality problem.
 */
static IOHprofiler_problem_t *f_leading_ones_neutrality_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("leading_ones_neutrality function",
                                                                               f_leading_ones_neutrality_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "leading_ones_neutrality", number_of_variables);

    /* Compute best solution */
    f_leading_ones_neutrality_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler leading_ones_neutrality problem.
 */
static IOHprofiler_problem_t *f_leading_ones_neutrality_IOHprofiler_problem_allocate(const size_t function,
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
    problem = f_leading_ones_neutrality_allocate(dimension);
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
#line 26 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_leading_ones_ruggedness1.c"
/**
 * @Onemax function
 */

/**
 * @file f_leading_ones_ruggedness1.c
 * @brief Implementation of the leading_ones_ruggedness1 function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 14 "code-experiments/src/f_leading_ones_ruggedness1.c"
#line 15 "code-experiments/src/f_leading_ones_ruggedness1.c"
#line 16 "code-experiments/src/f_leading_ones_ruggedness1.c"
#line 17 "code-experiments/src/f_leading_ones_ruggedness1.c"
#line 18 "code-experiments/src/f_leading_ones_ruggedness1.c"
#line 19 "code-experiments/src/f_leading_ones_ruggedness1.c"
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
    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);
    sigma = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);

    problem = f_leading_ones_ruggedness1_allocate(dimension);
    if(instance == 1){
       problem = transform_obj_ruggedness1(problem);
    }
    else if(instance > 1 && instance <= 50){
        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_ruggedness1(problem);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_sigma(problem, sigma, 0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_ruggedness1(problem);
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
    IOHprofiler_free_memory(sigma);
    IOHprofiler_free_memory(xins);
    return problem;
}
#line 27 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_leading_ones_ruggedness2.c"
/**
 * @Onemax function
 */

/**
 * @file f_leading_ones_ruggedness2.c
 * @brief Implementation of the leading_ones_ruggedness2 function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 14 "code-experiments/src/f_leading_ones_ruggedness2.c"
#line 15 "code-experiments/src/f_leading_ones_ruggedness2.c"
#line 16 "code-experiments/src/f_leading_ones_ruggedness2.c"
#line 17 "code-experiments/src/f_leading_ones_ruggedness2.c"
#line 18 "code-experiments/src/f_leading_ones_ruggedness2.c"
#line 19 "code-experiments/src/f_leading_ones_ruggedness2.c"
/**
 * @brief Implements the leading_ones_ruggedness2 function without connections to any IOHprofiler structures.
 */
static double f_leading_ones_ruggedness2_raw(const int *x, const size_t number_of_variables) {
    size_t i = 0;
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
static void f_leading_ones_ruggedness2_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_leading_ones_ruggedness2_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic leading_ones_ruggedness2 problem.
 */
static IOHprofiler_problem_t *f_leading_ones_ruggedness2_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("leading_ones_ruggedness2 function",
                                                                               f_leading_ones_ruggedness2_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "leading_ones_ruggedness2", number_of_variables);

    /* Compute best solution */
    f_leading_ones_ruggedness2_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler leading_ones_ruggedness2 problem.
 */
static IOHprofiler_problem_t *f_leading_ones_ruggedness2_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {
    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);
    sigma = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);

    problem = f_leading_ones_ruggedness2_allocate(dimension);
    if(instance == 1){
       problem = transform_obj_ruggedness2(problem);
    }
    else if(instance > 1 && instance <= 50){
        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_ruggedness2(problem);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_sigma(problem, sigma, 0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_ruggedness2(problem);
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
    IOHprofiler_free_memory(sigma);
    IOHprofiler_free_memory(xins);
    return problem;
}
#line 28 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_leading_ones_ruggedness3.c"
/**
 * @Onemax function
 */

/**
 * @file f_leading_ones_ruggedness3.c
 * @brief Implementation of the leading_ones_ruggedness3 function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 14 "code-experiments/src/f_leading_ones_ruggedness3.c"
#line 15 "code-experiments/src/f_leading_ones_ruggedness3.c"
#line 16 "code-experiments/src/f_leading_ones_ruggedness3.c"
#line 17 "code-experiments/src/f_leading_ones_ruggedness3.c"
#line 18 "code-experiments/src/f_leading_ones_ruggedness3.c"
#line 19 "code-experiments/src/f_leading_ones_ruggedness3.c"
/**
 * @brief Implements the leading_ones_ruggedness3 function without connections to any IOHprofiler structures.
 */
static double f_leading_ones_ruggedness3_raw(const int *x, const size_t number_of_variables) {
    size_t i = 0;
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
static void f_leading_ones_ruggedness3_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_leading_ones_ruggedness3_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic leading_ones_ruggedness3 problem.
 */
static IOHprofiler_problem_t *f_leading_ones_ruggedness3_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("leading_ones_ruggedness3 function",
                                                                               f_leading_ones_ruggedness3_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "leading_ones_ruggedness3", number_of_variables);

    /* Compute best solution */
    f_leading_ones_ruggedness3_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler leading_ones_ruggedness3 problem.
 */
static IOHprofiler_problem_t *f_leading_ones_ruggedness3_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {
    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);
    sigma = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);

    problem = f_leading_ones_ruggedness3_allocate(dimension);
    if(instance == 1){
       problem = transform_obj_ruggedness3(problem);
    }
    else if(instance > 1 && instance <= 50){
        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_ruggedness3(problem);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_sigma(problem, sigma, 0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_ruggedness3(problem);
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
    IOHprofiler_free_memory(sigma);
    IOHprofiler_free_memory(xins);
    return problem;
}
#line 29 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_ising_1D.c"
/**
 * @file f_ising_1D.c
 * @brief Implementation of the ising_1D function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 10 "code-experiments/src/f_ising_1D.c"
#line 11 "code-experiments/src/f_ising_1D.c"
#line 12 "code-experiments/src/f_ising_1D.c"
#line 13 "code-experiments/src/f_ising_1D.c"
#line 14 "code-experiments/src/f_ising_1D.c"
#line 15 "code-experiments/src/f_ising_1D.c"
#line 16 "code-experiments/src/f_ising_1D.c"


/**
 * @brief Implements the Ising one dimension function without connections to any IOHprofiler structures.
 */


int modulo_ising_1D(int x,int N){
    return (x % N + N) %N;
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static int f_ising_1D_raw(const int *x, const size_t number_of_variables) {
    int result= 0;
    int i;
    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;

    for (i = 0; i < number_of_variables; ++i) {
        int first_neig=x[modulo_ising_1D((i+1), (int)number_of_variables)];
        int second_neig=x[modulo_ising_1D((i -1) , (int)number_of_variables)];

        result += (x[i] *first_neig) + ((1- x[i])*(1- first_neig));
        result += (x[i] *second_neig) + ((1- x[i])*(1- second_neig));
    }

    return result;
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static void f_ising_1D_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_ising_1D_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic ising_1D problem.
 */
static IOHprofiler_problem_t *f_ising_1D_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("ising_1D function",
                                                                               f_ising_1D_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "ising_1D", number_of_variables);

    /* Compute best solution */
    f_ising_1D_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler ising_1D problem.
 */
static IOHprofiler_problem_t *f_ising_1D_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {


    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);
    sigma = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);
    problem = f_ising_1D_allocate(dimension);

    if(instance == 1){
        for(i = 0; i < dimension; i++)
            z[i] = 0;
        a = 0.0;
        problem = transform_vars_xor(problem,z,0);
        problem = transform_obj_shift(problem,a);
    }
    else if(instance > 1 && instance <= 50){
        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_sigma(problem, sigma, 0);
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
    IOHprofiler_free_memory(sigma);
    IOHprofiler_free_memory(xins);
    return problem;
}
#line 30 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_ising_2D.c"
/**
 * @file f_ising_2D.c
 * @brief Implementation of the ising_2D function and problem.
 */

#include <assert.h>
#include <stdio.h>
#include <math.h>

#line 11 "code-experiments/src/f_ising_2D.c"
#line 12 "code-experiments/src/f_ising_2D.c"
#line 13 "code-experiments/src/f_ising_2D.c"
#line 14 "code-experiments/src/f_ising_2D.c"
#line 15 "code-experiments/src/f_ising_2D.c"
#line 16 "code-experiments/src/f_ising_2D.c"
#line 17 "code-experiments/src/f_ising_2D.c"

/**
 * @brief Implements the ising_2D function without connections to any IOHprofiler structures.
 */

int modulo_ising_2D(int x,int N){
    return (x % N + N) %N;
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static int f_ising_2D_raw(const int *x, const size_t number_of_variables) {

    int i,j,neig;
    int result= 0;
    int neighbors[4];
    int lattice_size = (int)sqrt((double)number_of_variables);
    int (*spin_array)[lattice_size] = (int (*)[lattice_size])x;

    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;

    if(floor(sqrt((double)number_of_variables))!=sqrt((double)number_of_variables)){
      fprintf(stderr, "Number of parameters in the Ising square problem must be a square number\n");
      return NAN;
    }


    for (i = 0; i < lattice_size; ++i) {
           for (j = 0; j < lattice_size; ++j) {
                neighbors[0]  = spin_array[modulo_ising_2D(i - 1, lattice_size)][j];
                neighbors[1]  = spin_array[modulo_ising_2D(i + 1, lattice_size)][j];
                neighbors[2]  = spin_array[i][modulo_ising_2D((j - 1) , lattice_size)];
                neighbors[3]  = spin_array[i][modulo_ising_2D((j + 1) , lattice_size)];


                /*neighbors[0]  = x[modulo_ising_2D(i - 1, lattice_size) * lattice_size + j];
                neighbors[1]  = x[modulo_ising_2D(i + 1, lattice_size) * lattice_size + j];
                neighbors[2]  = x[i * lattice_size + modulo_ising_2D((j - 1) , lattice_size)];
                neighbors[3]  = x[i * lattice_size + modulo_ising_2D((j + 1) , lattice_size)];
                */
                for (neig=0; neig<4; neig++) {
                    result+= (spin_array[i][j] * neighbors[neig]) + ((1- spin_array[i][j])*(1- neighbors[neig]));
                    /*result+= (x[i*lattice_size + j] * neighbors[neig]) + ((1- x[i * lattice_size + j])*(1- neighbors[neig]));*/
                }
           }
    }

    return result;
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static void f_ising_2D_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_ising_2D_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic ising_2D problem.
 */
static IOHprofiler_problem_t *f_ising_2D_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("ising_2D function",
                                                                               f_ising_2D_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "ising_2D", number_of_variables);

    /* Compute best solution */
    f_ising_2D_evaluate(problem, problem->best_parameter, problem->best_value);
    return problem;
}

/**
 * @brief Creates the IOHprofiler ising_2D problem.
 */
static IOHprofiler_problem_t *f_ising_2D_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {


    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);
    sigma = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);
    problem = f_ising_2D_allocate(dimension);

    if(instance == 1){
        for(i = 0; i < dimension; i++)
            z[i] = 0;
        a = 0.0;
        problem = transform_vars_xor(problem,z,0);
        problem = transform_obj_shift(problem,a);
    }
    else if(instance > 1 && instance <= 50){
        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_sigma(problem, sigma, 0);
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
    IOHprofiler_free_memory(sigma);
    IOHprofiler_free_memory(xins);
    return problem;
}
#line 31 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_ising_triangle.c"
/**
 * @ising_triangle function
 */

/**
 * @file f_ising_triangle.c
 * @brief Implementation of the ising_triangle function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 14 "code-experiments/src/f_ising_triangle.c"
#line 15 "code-experiments/src/f_ising_triangle.c"
#line 16 "code-experiments/src/f_ising_triangle.c"
#line 17 "code-experiments/src/f_ising_triangle.c"
#line 18 "code-experiments/src/f_ising_triangle.c"
#line 19 "code-experiments/src/f_ising_triangle.c"
#line 20 "code-experiments/src/f_ising_triangle.c"

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

    int i,j,neig;
    int result = 0;
    int neighbors[6];
    int lattice_size = (int)sqrt((double)number_of_variables);
    int (*spin_array)[lattice_size] = (int (*)[lattice_size])x;

    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;

    for (i = 0; i < lattice_size; ++i) {
           for (j = 0; j < lattice_size; ++j) {
                neighbors[0] = spin_array[modulo_ising_triangle((i - 1), lattice_size)][j] ;
                neighbors[1] = spin_array[modulo_ising_triangle((i + 1) , lattice_size)][j] ;
                neighbors[2] = spin_array[i][modulo_ising_triangle((j - 1) , lattice_size)] ;
                neighbors[3] = spin_array[i][modulo_ising_triangle((i + 1) , lattice_size)] ;
                neighbors[4] = spin_array[modulo_ising_triangle((i - 1) , lattice_size)][modulo_ising_triangle((j - 1) , lattice_size)] ;
                neighbors[5] = spin_array[modulo_ising_triangle((i + 1) , lattice_size)][modulo_ising_triangle((j + 1) , lattice_size)];


                /*neighbors[0] = x[modulo_ising_triangle((i - 1), lattice_size) * lattice_size + j] ;
                neighbors[1] = x[modulo_ising_triangle((i + 1), lattice_size) * lattice_size + j] ;
                neighbors[2] = x[i * lattice_size + modulo_ising_triangle((j - 1) , lattice_size)] ;
                neighbors[3] = x[i * lattice_size + modulo_ising_triangle((i + 1) , lattice_size)] ;
                neighbors[4] = x[modulo_ising_triangle((i - 1) , lattice_size) * lattice_size + modulo_ising_triangle((j - 1) , lattice_size)] ;
                neighbors[5] = x[modulo_ising_triangle((i + 1) , lattice_size) * lattice_size + modulo_ising_triangle((j + 1) , lattice_size)];
                */

                for (neig=0; neig<6; neig++) {
                     result+= (spin_array[i][j] * neighbors[neig]) + ((1- spin_array[i][j])*(1- neighbors[neig]));
                   /* result+= (x[i * lattice_size + j] * neighbors[neig]) + ((1- spin_array[i * lattice_size + j])*(1- neighbors[neig]));*/
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


    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);
    sigma = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);
    problem = f_ising_triangle_allocate(dimension);

    if(instance == 1){
        for(i = 0; i < dimension; i++)
            z[i] = 0;
        a = 0.0;
        problem = transform_vars_xor(problem,z,0);
        problem = transform_obj_shift(problem,a);
    }
    else if(instance > 1 && instance <= 50){
        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_sigma(problem, sigma, 0);
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
    IOHprofiler_free_memory(sigma);
    IOHprofiler_free_memory(xins);
    return problem;
}
#line 32 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_N_queens.c"

/**
 * @file f_N_queens.c
 * @brief Implementation of the N_queens function and problem.
 */

#include <assert.h>
#include <stdio.h>
#include <math.h>

#line 12 "code-experiments/src/f_N_queens.c"
#line 13 "code-experiments/src/f_N_queens.c"
#line 14 "code-experiments/src/f_N_queens.c"
#line 15 "code-experiments/src/f_N_queens.c"
#line 16 "code-experiments/src/f_N_queens.c"
#line 17 "code-experiments/src/f_N_queens.c"
#line 18 "code-experiments/src/f_N_queens.c"

/**
 * @brief Implements the N Queens one dimension function without connections to any IOHprofiler structures.
 */

static double _max(double element1 , double element2) {
    if (element1>element2) return element1;
    else return element2;
}
/**
 * @brief Implements the N_queens function without connections to any IOHprofiler structures.
 */
static double f_N_queens_raw(const int *x, const size_t number_of_variables) {

    double result;
    int index,j,i,k,l;
    int N_queens=(int)(sqrt((double)number_of_variables)+0.5);
    int number_of_queens_on_board = 0;
    double k_penalty=0.0;
    double l_penalty =0.0;
    double raws_penalty=0.0;
    double columns_penalty=0.0;
    int indx=0;
    float C = (float)N_queens;

    if(floor(sqrt((double)number_of_variables))!=sqrt((double)number_of_variables)){
      fprintf(stderr, "Number of parameters in the N Queen problem must be a square number\n");
      return NAN;
    }


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
        columns_penalty+=_max(0.0, (-1.0+sum_column));
    }

    for(i=1; i<=N_queens; i++){
        double sum_raw = 0.0;
        /*double sum_k = 0.0;
        double sum_l = 0.0;*/
        for(j=1; j <=N_queens; j++){
            indx=((i-1)*N_queens) + ((j-1)%N_queens);
            sum_raw+=(double)x[indx];
        }
        raws_penalty+=_max(0.0, (-1.0+sum_raw));
    }

    for(k=2-N_queens; k<=N_queens-2; k++){
        double sum_k=0.0;
        for(i=1; i<=N_queens; i++){
            if (k+i>=1 && k+i<=N_queens){
                indx=((i-1)*N_queens) + ((k+i-1)%N_queens);
                sum_k += (double)x[indx];
            }
        }
        k_penalty+=_max(0.0, (-1.0+sum_k));
    }
    for(l=3; l<=2*N_queens-1; l++){
        double sum_l=0.0;
        for(i=1; i<=N_queens; i++){
            if (l-i>=1 && l-i<=N_queens){
                indx=((i-1)*N_queens) + ((l-i-1)%N_queens);
                sum_l += (double)x[indx];
            }
        }
        l_penalty+=_max(0.0, (-1.0+sum_l));
    }
    result = (double) (number_of_queens_on_board - (C*raws_penalty) - (C*columns_penalty) -(C*k_penalty) - (C*l_penalty));
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
    problem->best_value[0] = sqrt((double)number_of_variables);
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


    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);
    sigma = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);
    problem = f_N_queens_allocate(dimension);

    if(instance == 1){
        for(i = 0; i < dimension; i++)
            z[i] = 0;
        a = 0.0;
        problem = transform_vars_xor(problem,z,0);
        problem = transform_obj_shift(problem,a);
    }
    else if(instance > 1 && instance <= 50){
        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_sigma(problem, sigma, 0);
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
    IOHprofiler_free_memory(sigma);
    IOHprofiler_free_memory(xins);
    return problem;
}
#line 33 "code-experiments/src/suite_PBO.c"
#line 1 "code-experiments/src/f_MIS.c"
/**
 * @file f_MIS.c
 * @brief Implementation of the Maximum Independent Set function and problem.
 */

#include <assert.h>
#include <stdio.h>

#line 10 "code-experiments/src/f_MIS.c"
#line 11 "code-experiments/src/f_MIS.c"
#line 12 "code-experiments/src/f_MIS.c"
#line 13 "code-experiments/src/f_MIS.c"
#line 14 "code-experiments/src/f_MIS.c"
#line 15 "code-experiments/src/f_MIS.c"
#line 16 "code-experiments/src/f_MIS.c"

/**
 * @brief Implements the Maximum Independent Set function without connections to any IOHprofiler structures.
 */

 static int isEdge(int i, int j, size_t problem_size) {
    if (i!=problem_size/2 && j==i+1){return 1;}
    else if (i<=((int)problem_size/2)-1 && j==i+((int)problem_size/2)+1 ){return 1;}
    else if (i<=((int)problem_size/2) && i>=2 && j==i+((int)problem_size/2)-1){return 1;}
    else {return 0;}
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static int f_MIS_raw(const int *x, const size_t number_of_variables) {

    int i, j,index;
    int result= 0;
    int num_of_ones=0;
    int sum_edges_in_the_set=0;
    int number_of_variables_even=(int)number_of_variables;
    int ones_array[number_of_variables_even+1];

    if (IOHprofiler_vector_contains_nan(x, number_of_variables))
        return NAN;

    if (number_of_variables%2!=0){
        number_of_variables_even=(int)number_of_variables-1;
    }


    for (index=0; index<number_of_variables_even; index++){
        if (x[index]==1){
            ones_array[num_of_ones] = index;
            num_of_ones+=1;
        }
    }

    for (i=0; i<num_of_ones; i++){
        for (j=i+1; j<num_of_ones; j++){
            if(isEdge(ones_array[i]+1,ones_array[j]+1,(size_t)number_of_variables_even)==1){
                sum_edges_in_the_set+=1;
            }
        }
    }
    result=num_of_ones - (number_of_variables_even*sum_edges_in_the_set);
    return result;
}

/**
 * @brief Uses the raw function to evaluate the IOHprofiler problem.
 */
static void f_MIS_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
    assert(problem->number_of_objectives == 1);
    y[0] = f_MIS_raw(x, problem->number_of_variables);
    assert(y[0] <= problem->best_value[0]);
}

/**
 * @brief Allocates the basic MIS problem.
 */
static IOHprofiler_problem_t *f_MIS_allocate(const size_t number_of_variables) {
    IOHprofiler_problem_t *problem = IOHprofiler_problem_allocate_from_scalars("MIS function",
                                                                               f_MIS_evaluate, NULL, number_of_variables, 0, 1, 1);

    IOHprofiler_problem_set_id(problem, "%s_d%02lu", "MIS", number_of_variables);

    /* Compute best solution */
    /*f_MIS_evaluate(problem, problem->best_parameter, problem->best_value);*/
    problem->best_value[0] = (double)(number_of_variables / 2) + 1.0;
    return problem;
}

/**
 * @brief Creates the IOHprofiler MIS problem.
 */
static IOHprofiler_problem_t *f_MIS_IOHprofiler_problem_allocate(const size_t function,
                                                                     const size_t dimension,
                                                                     const size_t instance,
                                                                     const long rseed,
                                                                     const char *problem_id_template,
                                                                     const char *problem_name_template) {


    int *z, *sigma;
    int temp,t;
    size_t i;
    double a;
    double b;
    double *xins;
    IOHprofiler_problem_t *problem;
    z = IOHprofiler_allocate_int_vector(dimension);
    sigma = IOHprofiler_allocate_int_vector(dimension);
    xins = IOHprofiler_allocate_vector(dimension);
    problem = f_MIS_allocate(dimension);

    if(instance == 1){
        for(i = 0; i < dimension; i++)
            z[i] = 0;
        a = 0.0;
        problem = transform_vars_xor(problem,z,0);
        problem = transform_obj_shift(problem,a);
    }
    else if(instance > 1 && instance <= 50){
        IOHprofiler_compute_xopt(z,rseed,dimension);
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function,instance);
        problem = transform_vars_xor(problem,z,0);
        assert(a <= 5.0 && a >= 0.2);
        problem = transform_obj_scale(problem,a);
        problem = transform_obj_shift(problem,b);
    }
    else if(instance > 50 && instance <= 100)
    {
        IOHprofiler_compute_xopt_double(xins,rseed,dimension);
        for(i = 0; i < dimension; i++){
            sigma[i] = (int)i;
        }
        for(i = 0; i < dimension; i++){
            t = (int)(xins[i] * (double)dimension);
            assert(t >= 0 && t < dimension);
            temp = sigma[0];
            sigma[0] = sigma[t];
            sigma[t] = temp;
        }
        a = IOHprofiler_compute_fopt(function,instance + 100);
        a = fabs(a) / 1000 * 4.8 + 0.2;
        b = IOHprofiler_compute_fopt(function, instance);
        problem = transform_vars_sigma(problem, sigma, 0);
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
    IOHprofiler_free_memory(sigma);
    IOHprofiler_free_memory(xins);
    return problem;
}
#line 34 "code-experiments/src/suite_PBO.c"


static IOHprofiler_suite_t *IOHprofiler_suite_allocate(const char *suite_name,
                                                       const size_t number_of_functions,
                                                       const size_t number_of_dimensions,
                                                       const size_t *dimensions,
                                                       const char *default_instances);

/**
 * @brief Sets the dimensions and default instances for the IOHprofiler suite.
 */
static IOHprofiler_suite_t *suite_PBO_initialize(void) {
    IOHprofiler_suite_t *suite;
    size_t i;
    const char *suite_name = "PBO";
    const size_t number_of_functions = 24;

    size_t dimensions[20000];

    const size_t number_of_dimensions = sizeof(dimensions) / sizeof(dimensions[0]);
    for (i = 0; i < number_of_dimensions; ++i){
        dimensions[i] = i + 1;
    }
    /* IMPORTANT: Make sure to change the default instance for every new workshop! */
    suite = IOHprofiler_suite_allocate(suite_name, number_of_functions, number_of_dimensions, dimensions, "year: 2018");

    return suite;
}

/**
 * @brief Sets the instances associated with years for the IOHprofiler suite.
 */
static const char *suite_PBO_get_instances_by_year(const int year) {
    if (year == 2018) {
        return "1-100";
    } else {
        IOHprofiler_error("suite_PBO_get_instances_by_year(): year %d not defined for suite_IOHprofiler", year);
        return NULL;
    }
}

/**
 * @brief Creates and returns a IOHprofiler problem without needing the actual IOHprofiler suite.
 *
 * Useful for other suites as well (see for example suite_biobj.c).
 */
static IOHprofiler_problem_t *PBO_get_problem(const size_t function,
                                              const size_t dimension,
                                              const size_t instance) {
    IOHprofiler_problem_t *problem = NULL;

    const char *problem_id_template = "PBO_f%03lu_i%02lu_d%02lu";
    const char *problem_name_template = "PBO suite problem f%lu instance %lu in %luD";

    const long rseed = (long)(function + 10000 * instance);

    if (function == 1) {
        problem = f_one_max_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                         problem_id_template, problem_name_template);
    } else if (function == 2) {
        problem = f_leading_ones_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                              problem_id_template, problem_name_template);
    } else if (function == 3) {
        problem = f_linear_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                      problem_id_template, problem_name_template);
    } else if (function == 4) {
        problem = f_one_max_dummy1_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 5) {
        problem = f_one_max_dummy2_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 6) {
        problem = f_one_max_neutrality_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 7) {
        problem = f_one_max_epistasis_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 8) {
        problem = f_one_max_ruggedness1_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 9) {
        problem = f_one_max_ruggedness2_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 10) {
        problem = f_one_max_ruggedness3_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 11) {
        problem = f_leading_ones_dummy1_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 12) {
        problem = f_leading_ones_dummy2_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 13) {
        problem = f_leading_ones_neutrality_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 14) {
        problem = f_leading_ones_epistasis_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 15) {
        problem = f_leading_ones_ruggedness1_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 16) {
        problem = f_leading_ones_ruggedness2_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 17) {
        problem = f_leading_ones_ruggedness3_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 18) {
        problem = f_labs_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 19) {
        problem = f_ising_1D_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 20) {
        problem = f_ising_2D_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 21) {
        problem = f_ising_triangle_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 22) {
        problem = f_MIS_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 23) {
        problem = f_N_queens_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    } else if (function == 24) {
        problem = f_jump_IOHprofiler_problem_allocate(function, dimension, instance, rseed,
                                                        problem_id_template, problem_name_template);
    }else {
        IOHprofiler_error("get_IOHprofiler_problem(): cannot retrieve problem f%lu instance %lu in %luD",
                          (unsigned long)function, (unsigned long)instance, (unsigned long)dimension);
        return NULL; /* Never reached */
    }

    return problem;
}

/**
 * @brief Returns the problem from the IOHprofiler suite that corresponds to the given parameters.
 *
 * @param suite The IOHprofiler suite.
 * @param function_idx Index of the function (starting from 0).
 * @param dimension_idx Index of the dimension (starting from 0).
 * @param instance_idx Index of the instance (starting from 0).
 * @return The problem that corresponds to the given parameters.
 */
static IOHprofiler_problem_t *suite_PBO_get_problem(IOHprofiler_suite_t *suite,
                                                    const size_t function_idx,
                                                    const size_t dimension_idx,
                                                    const size_t instance_idx) {
    IOHprofiler_problem_t *problem = NULL;

    const size_t function = suite->functions[function_idx];
    const size_t dimension = suite->dimensions[dimension_idx];
    const size_t instance = suite->instances[instance_idx];

    problem = PBO_get_problem(function, dimension, instance);

    problem->suite_dep_function = function;
    problem->suite_dep_instance = instance;
    problem->suite_dep_index = IOHprofiler_suite_encode_problem_index(suite, function_idx, dimension_idx, instance_idx);

    return problem;
}
#line 17 "code-experiments/src/IOHprofiler_suite.c"

/** @brief The maximum number of different instances in a suite. */
#define IOHprofiler_MAX_INSTANCES 1000

/**
 * @brief Calls the initializer of the given suite.
 *
 * @note This function needs to be updated when a new suite is added to IOHprofiler.
 */
static IOHprofiler_suite_t *IOHprofiler_suite_initialize(const char *suite_name) {
    IOHprofiler_suite_t *suite;

    if (strcmp(suite_name, "PBO") == 0) {
        suite = suite_PBO_initialize();
    } else {
        IOHprofiler_error("IOHprofiler_suite_initialize(): unknown problem suite");
        return NULL;
    }

    return suite;
}

/**
 * @brief Calls the function that sets the instanced by year for the given suite.
 * @note This function needs to be updated when a new suite is added to IOHprofiler.
 */
static const char *IOHprofiler_suite_get_instances_by_year(const IOHprofiler_suite_t *suite, const int year) {
    const char *year_string;

    if (strcmp(suite->suite_name, "PBO") == 0) {
        year_string = suite_PBO_get_instances_by_year(year);
    } else {
        IOHprofiler_error("IOHprofiler_suite_get_instances_by_year(): suite '%s' has no years defined", suite->suite_name);
        return NULL;
    }

    return year_string;
}

/**
 * @brief Calls the function that returns the problem corresponding to the given suite, function index,
 * dimension index and instance index. If the indices don't correspond to a problem because of suite
 * filtering, it returns NULL.
 *
 * @note This function needs to be updated when a new suite is added to IOHprofiler.
 */
static IOHprofiler_problem_t *IOHprofiler_suite_get_problem_from_indices(IOHprofiler_suite_t *suite,
                                                                         const size_t function_idx,
                                                                         const size_t dimension_idx,
                                                                         const size_t instance_idx) {
    IOHprofiler_problem_t *problem;

    if ((suite->functions[function_idx] == 0) ||
        (suite->dimensions[dimension_idx] == 0)) {
        return NULL;
    }

    if (strcmp(suite->suite_name, "PBO") == 0) {
        problem = suite_PBO_get_problem(suite, function_idx, dimension_idx, instance_idx);
    } else {
        IOHprofiler_error("IOHprofiler_suite_get_problem_from_indices(): unknown problem suite");
        return NULL;
    }
    IOHprofiler_problem_set_suite(problem, suite);

    return problem;
}

/**
 * @note: While a suite can contain multiple problems with equal function, dimension and instance, this
 * function always returns the first problem in the suite with the given function, dimension and instance
 * values. If the given values don't correspond to a problem, the function returns NULL.
 */
IOHprofiler_problem_t *IOHprofiler_suite_get_problem_by_function_dimension_instance(IOHprofiler_suite_t *suite,
                                                                                    const size_t function,
                                                                                    const size_t dimension,
                                                                                    const size_t instance) {
    size_t i;
    int function_idx, dimension_idx, instance_idx;
    int found;

    found = 0;
    for (i = 0; i < suite->number_of_functions; i++) {
        if (suite->functions[i] == function) {
            function_idx = (int)i;
            found = 1;
            break;
        }
    }
    if (!found)
        return NULL;

    found = 0;
    for (i = 0; i < suite->number_of_dimensions; i++) {
        if (suite->dimensions[i] == dimension) {
            dimension_idx = (int)i;
            found = 1;
            break;
        }
    }
    if (!found)
        return NULL;

    found = 0;
    for (i = 0; i < suite->number_of_instances; i++) {
        if (suite->instances[i] == instance) {
            instance_idx = (int)i;
            found = 1;
            break;
        }
    }
    if (!found)
        return NULL;

    return IOHprofiler_suite_get_problem_from_indices(suite, (size_t)function_idx, (size_t)dimension_idx, (size_t)instance_idx);
}

/**
 * @brief Allocates the space for a IOHprofiler_suite_t instance.
 *
 * This function sets the functions and dimensions contained in the suite, while the instances are set by
 * the function IOHprofiler_suite_set_instance.
 */
static IOHprofiler_suite_t *IOHprofiler_suite_allocate(const char *suite_name,
                                                       const size_t number_of_functions,
                                                       const size_t number_of_dimensions,
                                                       const size_t *dimensions,
                                                       const char *default_instances) {
    IOHprofiler_suite_t *suite;
    size_t i;

    suite = (IOHprofiler_suite_t *)IOHprofiler_allocate_memory(sizeof(*suite));

    suite->suite_name = IOHprofiler_strdup(suite_name);

    suite->number_of_dimensions = number_of_dimensions;
    assert(number_of_dimensions > 0);
    suite->dimensions = IOHprofiler_allocate_vector_size_t(suite->number_of_dimensions);
    for (i = 0; i < suite->number_of_dimensions; i++) {
        suite->dimensions[i] = dimensions[i];
    }

    suite->number_of_functions = number_of_functions;
    assert(number_of_functions > 0);
    suite->functions = IOHprofiler_allocate_vector_size_t(suite->number_of_functions);
    for (i = 0; i < suite->number_of_functions; i++) {
        suite->functions[i] = i + 1;
    }

    assert(strlen(default_instances) > 0);
    suite->default_instances = IOHprofiler_strdup(default_instances);

    /* Will be set to the first valid dimension index before the constructor ends */
    suite->current_dimension_idx = -1;
    /* Will be set to the first valid function index before the constructor ends  */
    suite->current_function_idx = -1;

    suite->current_instance_idx = -1;
    suite->current_problem = NULL;

    /* To be set in IOHprofiler_suite_set_instance() */
    suite->number_of_instances = 0;
    suite->instances = NULL;

    /* To be set in particular suites if needed */
    suite->data = NULL;
    suite->data_free_function = NULL;

    return suite;
}

/**
 * @brief Sets the suite instance to the given instance_numbers.
 */
static void IOHprofiler_suite_set_instance(IOHprofiler_suite_t *suite,
                                           const size_t *instance_numbers) {
    size_t i;

    if (!instance_numbers) {
        IOHprofiler_error("IOHprofiler_suite_set_instance(): no instance given");
        return;
    }

    suite->number_of_instances = IOHprofiler_count_numbers(instance_numbers, IOHprofiler_MAX_INSTANCES, "suite instance numbers");
    suite->instances = IOHprofiler_allocate_vector_size_t(suite->number_of_instances);
    for (i = 0; i < suite->number_of_instances; i++) {
        suite->instances[i] = instance_numbers[i];
    }
}

/**
 * @brief Filters the given items w.r.t. the given indices (starting from 1).
 *
 * Sets items[i - 1] to 0 for every i that cannot be found in indices (this function performs the conversion
 * from user-friendly indices starting from 1 to C-friendly indices starting from 0).
 */
static void IOHprofiler_suite_filter_indices(size_t *items, const size_t number_of_items, const size_t *indices, const char *name) {
    size_t i, j;
    size_t count = IOHprofiler_count_numbers(indices, IOHprofiler_MAX_INSTANCES, name);
    int found;

    for (i = 1; i <= number_of_items; i++) {
        found = 0;
        for (j = 0; j < count; j++) {
            if (i == indices[j]) {
                found = 1;
                break;
            }
        }
        if (!found)
            items[i - 1] = 0;
    }
}

/**
 * @brief Filters dimensions w.r.t. the given dimension_numbers.
 *
 * Sets suite->dimensions[i] to 0 for every dimension value that cannot be found in dimension_numbers.
 */
static void IOHprofiler_suite_filter_dimensions(IOHprofiler_suite_t *suite, const size_t *dimension_numbers) {
    size_t i, j;
    size_t count = IOHprofiler_count_numbers(dimension_numbers, IOHprofiler_MAX_INSTANCES, "dimensions");
    int found;

    for (i = 0; i < suite->number_of_dimensions; i++) {
        found = 0;
        for (j = 0; j < count; j++) {
            if (suite->dimensions[i] == dimension_numbers[j])
                found = 1;
        }
        if (!found)
            suite->dimensions[i] = 0;
    }
}

/**
 * @param suite The given suite.
 * @param function_idx The index of the function in question (starting from 0).
 *
 * @return The function number in position function_idx in the suite. If the function has been filtered out
 * through suite_options in the IOHprofiler_suite function, the result is 0.
 */
size_t IOHprofiler_suite_get_function_from_function_index(const IOHprofiler_suite_t *suite, const size_t function_idx) {
    if (function_idx >= suite->number_of_functions) {
        IOHprofiler_error("IOHprofiler_suite_get_function_from_function_index(): function index exceeding the number of functions in the suite");
        return 0; /* Never reached*/
    }

    return suite->functions[function_idx];
}

/**
 * @param suite The given suite.
 * @param dimension_idx The index of the dimension in question (starting from 0).
 *
 * @return The dimension number in position dimension_idx in the suite. If the dimension has been filtered out
 * through suite_options in the IOHprofiler_suite function, the result is 0.
 */
size_t IOHprofiler_suite_get_dimension_from_dimension_index(const IOHprofiler_suite_t *suite, const size_t dimension_idx) {
    if (dimension_idx >= suite->number_of_dimensions) {
        IOHprofiler_error("IOHprofiler_suite_get_dimension_from_dimension_index(): dimensions index exceeding the number of dimensions in the suite");
        return 0; /* Never reached*/
    }

    return suite->dimensions[dimension_idx];
}

/**
 * @param suite The given suite.
 * @param instance_idx The index of the instance in question (starting from 0).
 *
 * @return The instance number in position instance_idx in the suite. If the instance has been filtered out
 * through suite_options in the IOHprofiler_suite function, the result is 0.
 */
size_t IOHprofiler_suite_get_instance_from_instance_index(const IOHprofiler_suite_t *suite, const size_t instance_idx) {
    if (instance_idx >= suite->number_of_instances) {
        IOHprofiler_error("IOHprofiler_suite_get_instance_from_instance_index(): instance index exceeding the number of instances in the suite");
        return 0; /* Never reached*/
    }

    return suite->functions[instance_idx];
}

void IOHprofiler_suite_free(IOHprofiler_suite_t *suite) {
    if (suite != NULL) {
        if (suite->suite_name) {
            IOHprofiler_free_memory(suite->suite_name);
            suite->suite_name = NULL;
        }
        if (suite->dimensions) {
            IOHprofiler_free_memory(suite->dimensions);
            suite->dimensions = NULL;
        }
        if (suite->functions) {
            IOHprofiler_free_memory(suite->functions);
            suite->functions = NULL;
        }
        if (suite->instances) {
            IOHprofiler_free_memory(suite->instances);
            suite->instances = NULL;
        }
        if (suite->default_instances) {
            IOHprofiler_free_memory(suite->default_instances);
            suite->default_instances = NULL;
        }

        if (suite->current_problem) {
            IOHprofiler_problem_free(suite->current_problem);
            suite->current_problem = NULL;
        }

        if (suite->data != NULL) {
            if (suite->data_free_function != NULL) {
                suite->data_free_function(suite->data);
            }
            IOHprofiler_free_memory(suite->data);
            suite->data = NULL;
        }

        IOHprofiler_free_memory(suite);
        suite = NULL;
    }
}

/**
 * Note that the problem_index depends on the number of instances a suite is defined with.
 *
 * @param suite The given suite.
 * @param problem_index The index of the problem to be returned.
 *
 * @return The problem of the suite defined by problem_index (NULL if this problem has been filtered out
 * from the suite).
 */
IOHprofiler_problem_t *IOHprofiler_suite_get_problem(IOHprofiler_suite_t *suite, const size_t problem_index) {
    size_t function_idx = 0, instance_idx = 0, dimension_idx = 0;
    IOHprofiler_suite_decode_problem_index(suite, problem_index, &function_idx, &dimension_idx, &instance_idx);

    return IOHprofiler_suite_get_problem_from_indices(suite, function_idx, dimension_idx, instance_idx);
}

/**
 * The number of problems in the suite is computed as a product of the number of instances, number of
 * functions and number of dimensions and therefore doesn't account for any filtering done through the
 * suite_options parameter of the IOHprofiler_suite function.
 *
 * @param suite The given suite.
 *
 * @return The number of problems in the suite.
 */
size_t IOHprofiler_suite_get_number_of_problems(const IOHprofiler_suite_t *suite) {
    return (suite->number_of_instances * suite->number_of_functions * suite->number_of_dimensions);
}

/**
 * @brief Returns the instances read from either a "year: YEAR" or "instances: NUMBERS" string.
 *
 * If both "year" and "instances" are given, the second is ignored (and a warning is raised). See the
 * IOHprofiler_suite function for more information about the required format.
 */
static size_t *IOHprofiler_suite_get_instance_indices(const IOHprofiler_suite_t *suite, const char *suite_instance) {
    int year = -1;
    char *instances = NULL;
    const char *year_string = NULL;
    long year_found, instances_found;
    int parce_year = 1, parce_instances = 1;
    size_t *result = NULL;

    if (suite_instance == NULL)
        return NULL;

    year_found = IOHprofiler_strfind(suite_instance, "year");
    instances_found = IOHprofiler_strfind(suite_instance, "instances");

    if ((year_found < 0) && (instances_found < 0))
        return NULL;

    if ((year_found > 0) && (instances_found > 0)) {
        if (year_found < instances_found) {
            parce_instances = 0;
            IOHprofiler_warning("IOHprofiler_suite_get_instance_indices(): 'instances' suite option ignored because it follows 'year'");
        } else {
            parce_year = 0;
            IOHprofiler_warning("IOHprofiler_suite_get_instance_indices(): 'year' suite option ignored because it follows 'instances'");
        }
    }

    if ((year_found >= 0) && (parce_year == 1)) {
        if (IOHprofiler_options_read_int(suite_instance, "year", &(year)) != 0) {
            year_string = IOHprofiler_suite_get_instances_by_year(suite, year);
            result = IOHprofiler_string_parse_ranges(year_string, 1, 0, "instances", IOHprofiler_MAX_INSTANCES);
        } else {
            IOHprofiler_warning("IOHprofiler_suite_get_instance_indices(): problems parsing the 'year' suite_instance option, ignored");
        }
    }

    instances = IOHprofiler_allocate_string(IOHprofiler_MAX_INSTANCES);
    if ((instances_found >= 0) && (parce_instances == 1)) {
        if (IOHprofiler_options_read_values(suite_instance, "instances", instances) > 0) {
            result = IOHprofiler_string_parse_ranges(instances, 1, 0, "instances", IOHprofiler_MAX_INSTANCES);
        } else {
            IOHprofiler_warning("IOHprofiler_suite_get_instance_indices(): problems parsing the 'instance' suite_instance option, ignored");
        }
    }
    IOHprofiler_free_memory(instances);

    return result;
}

/**
 * @brief Iterates through the items from the current_item_id position on in search for the next positive
 * item.
 *
 * If such an item is found, current_item_id points to this item and the method returns 1. If such an
 * item cannot be found, current_item_id points to the first positive item and the method returns 0.
 */
static int IOHprofiler_suite_is_next_item_found(const size_t *items, const size_t number_of_items, long *current_item_id) {
    if ((*current_item_id) != number_of_items - 1) {
        /* Not the last item, iterate through items */
        do {
            (*current_item_id)++;
        } while (((*current_item_id) < number_of_items - 1) && (items[*current_item_id] == 0));

        assert((*current_item_id) < number_of_items);
        if (items[*current_item_id] != 0) {
            /* Next item is found, return true */
            return 1;
        }
    }

    /* Next item cannot be found, move to the first good item and return false */
    *current_item_id = -1;
    do {
        (*current_item_id)++;
    } while ((*current_item_id < number_of_items - 1) && (items[*current_item_id] == 0));
    if (items[*current_item_id] == 0)
        IOHprofiler_error("IOHprofiler_suite_is_next_item_found(): the chosen suite has no valid (positive) items");
    return 0;
}

/**
 * @brief Iterates through the instances of the given suite from the current_instance_idx position on in
 * search for the next positive instance.
 *
 * If such an instance is found, current_instance_idx points to this instance and the method returns 1. If
 * such an instance cannot be found, current_instance_idx points to the first positive instance and the
 * method returns 0.
 */
static int IOHprofiler_suite_is_next_instance_found(IOHprofiler_suite_t *suite) {
    return IOHprofiler_suite_is_next_item_found(suite->instances, suite->number_of_instances,
                                                &suite->current_instance_idx);
}

/**
 * @brief Iterates through the functions of the given suite from the current_function_idx position on in
 * search for the next positive function.
 *
 * If such a function is found, current_function_idx points to this function and the method returns 1. If
 * such a function cannot be found, current_function_idx points to the first positive function,
 * current_instance_idx points to the first positive instance and the method returns 0.
 */
static int IOHprofiler_suite_is_next_function_found(IOHprofiler_suite_t *suite) {
    int result = IOHprofiler_suite_is_next_item_found(suite->functions, suite->number_of_functions,
                                                      &suite->current_function_idx);
    if (!result) {
        /* Reset the instances */
        suite->current_instance_idx = -1;
        IOHprofiler_suite_is_next_instance_found(suite);
    }
    return result;
}

/**
 * @brief Iterates through the dimensions of the given suite from the current_dimension_idx position on in
 * search for the next positive dimension.
 *
 * If such a dimension is found, current_dimension_idx points to this dimension and the method returns 1. If
 * such a dimension cannot be found, current_dimension_idx points to the first positive dimension and the
 * method returns 0.
 */
static int IOHprofiler_suite_is_next_dimension_found(IOHprofiler_suite_t *suite) {
    return IOHprofiler_suite_is_next_item_found(suite->dimensions, suite->number_of_dimensions,
                                                &suite->current_dimension_idx);
}

/**
 * Currently, five suites are supported:
 * - "IOHprofiler" contains 5 single-objective functions in 6 dimensions (500,1000,1500,2000,2500,3000)
 *
 * Only the suite_name parameter needs to be non-empty. The suite_instance and suite_options can be "" or
 * NULL. In this case, default values are taken (default instances of a suite are those used in the last year
 * and the suite is not filtered by default).
 *
 * @param suite_name A string containing the name of the suite. Currently supported suite name is "IOHprofiler".
 * @param suite_instance A string used for defining the suite instances. Two ways are supported:
 * - "year: TODO: definition of classification of instances.
 * - "instances: VALUES", where VALUES are instance numbers from 1 on written as a comma-separated list or a
 * range m-n.
 * @param suite_options A string of pairs "key: value" used to filter the suite (especially useful for
 * parallelizing the experiments). Supported options:
 * - "dimensions: LIST", where LIST is the list of dimensions to keep in the suite (range-style syntax is
 * not allowed here),
 * - "dimension_indices: VALUES", where VALUES is a list or a range of dimension indices (starting from 1) to keep
 * in the suite, and
 * - "function_indices: VALUES", where VALUES is a list or a range of function indices (starting from 1) to keep
 * in the suite, and
 * - "instance_indices: VALUES", where VALUES is a list or a range of instance indices (starting from 1) to keep
 * in the suite.
 *
 * @return The constructed suite object.
 */
IOHprofiler_suite_t *IOHprofiler_suite(const char *suite_name, const char *suite_instance, const char *suite_options) {
    IOHprofiler_suite_t *suite;
    size_t *instances;
    char *option_string = NULL;
    char *ptr;
    size_t *indices = NULL;
    size_t *dimensions = NULL;
    long dim_found, dim_idx_found;
    int parce_dim = 1, parce_dim_idx = 1;

    IOHprofiler_option_keys_t *known_option_keys, *given_option_keys, *redundant_option_keys;

    /* Sets the valid keys for suite options and suite instance */
    const char *known_keys_o[] = { "dimensions", "dimension_indices", "function_indices", "instance_indices" };
    const char *known_keys_i[] = { "year", "instances" };

    /* Initialize the suite */
    suite = IOHprofiler_suite_initialize(suite_name);

    /* Set the instance */
    if ((!suite_instance) || (strlen(suite_instance) == 0))
        instances = IOHprofiler_suite_get_instance_indices(suite, suite->default_instances);
    else {
        instances = IOHprofiler_suite_get_instance_indices(suite, suite_instance);

        if (!instances) {
            /* Something wrong in the suite_instance string, use default instead */
            instances = IOHprofiler_suite_get_instance_indices(suite, suite->default_instances);
        }

        /* Check for redundant option keys for suite instance */
        known_option_keys = IOHprofiler_option_keys_allocate(sizeof(known_keys_i) / sizeof(char *), known_keys_i);
        given_option_keys = IOHprofiler_option_keys(suite_instance);

        if (given_option_keys) {
            redundant_option_keys = IOHprofiler_option_keys_get_redundant(known_option_keys, given_option_keys);

            if ((redundant_option_keys != NULL) && (redundant_option_keys->count > 0)) {
                /* Warn the user that some of given options are being ignored and output the valid options */
                char *output_redundant = IOHprofiler_option_keys_get_output_string(redundant_option_keys,
                    "IOHprofiler_suite(): Some keys in suite instance were ignored:\n");
                char *output_valid = IOHprofiler_option_keys_get_output_string(known_option_keys,
                    "Valid keys for suite instance are:\n");
                IOHprofiler_warning("%s%s", output_redundant, output_valid);
                IOHprofiler_free_memory(output_redundant);
                IOHprofiler_free_memory(output_valid);
            }

            IOHprofiler_option_keys_free(given_option_keys);
            IOHprofiler_option_keys_free(redundant_option_keys);
        }
        IOHprofiler_option_keys_free(known_option_keys);
    }
    IOHprofiler_suite_set_instance(suite, instances);
    IOHprofiler_free_memory(instances);

    /* Apply filter if any given by the suite_options */
    if ((suite_options) && (strlen(suite_options) > 0)) {
        option_string = IOHprofiler_allocate_string(IOHprofiler_PATH_MAX);
        if (IOHprofiler_options_read_values(suite_options, "function_indices", option_string) > 0) {
            indices = IOHprofiler_string_parse_ranges(option_string, 1, suite->number_of_functions, "function_indices", IOHprofiler_MAX_INSTANCES);
            if (indices != NULL) {
                IOHprofiler_suite_filter_indices(suite->functions, suite->number_of_functions, indices, "function_indices");
                IOHprofiler_free_memory(indices);
            }
        }
        IOHprofiler_free_memory(option_string);

        option_string = IOHprofiler_allocate_string(IOHprofiler_PATH_MAX);
        if (IOHprofiler_options_read_values(suite_options, "instance_indices", option_string) > 0) {
            indices = IOHprofiler_string_parse_ranges(option_string, 1, suite->number_of_instances, "instance_indices", IOHprofiler_MAX_INSTANCES);
            if (indices != NULL) {
                IOHprofiler_suite_filter_indices(suite->instances, suite->number_of_instances, indices, "instance_indices");
                IOHprofiler_free_memory(indices);
            }
        }
        IOHprofiler_free_memory(option_string);

        dim_found = IOHprofiler_strfind(suite_options, "dimensions");
        dim_idx_found = IOHprofiler_strfind(suite_options, "dimension_indices");

        if ((dim_found > 0) && (dim_idx_found > 0)) {
            if (dim_found < dim_idx_found) {
                parce_dim_idx = 0;
                IOHprofiler_warning("IOHprofiler_suite(): 'dimension_indices' suite option ignored because it follows 'dimensions'");
            } else {
                parce_dim = 0;
                IOHprofiler_warning("IOHprofiler_suite(): 'dimensions' suite option ignored because it follows 'dimension_indices'");
            }
        }

        option_string = IOHprofiler_allocate_string(IOHprofiler_PATH_MAX);
        if ((dim_idx_found >= 0) && (parce_dim_idx == 1) && (IOHprofiler_options_read_values(suite_options, "dimension_indices", option_string) > 0)) {
            indices = IOHprofiler_string_parse_ranges(option_string, 1, suite->number_of_dimensions, "dimension_indices",
                                                      IOHprofiler_MAX_INSTANCES);
            if (indices != NULL) {
                IOHprofiler_suite_filter_indices(suite->dimensions, suite->number_of_dimensions, indices, "dimension_indices");
                IOHprofiler_free_memory(indices);
            }
        }
        IOHprofiler_free_memory(option_string);

        option_string = IOHprofiler_allocate_string(IOHprofiler_PATH_MAX);
        if ((dim_found >= 0) && (parce_dim == 1) && (IOHprofiler_options_read_values(suite_options, "dimensions", option_string) > 0)) {
            ptr = option_string;
            /* Check for disallowed characters */
            while (*ptr != '\0') {
                if ((*ptr != ',') && !isdigit((unsigned char)*ptr)) {
                    IOHprofiler_warning("IOHprofiler_suite(): 'dimensions' suite option ignored because of disallowed characters");
                    return NULL;
                } else
                    ptr++;
            }
            dimensions = IOHprofiler_string_parse_ranges(option_string, suite->dimensions[0],
                                                         suite->dimensions[suite->number_of_dimensions - 1], "dimensions", IOHprofiler_MAX_INSTANCES);
            if (dimensions != NULL) {
                IOHprofiler_suite_filter_dimensions(suite, dimensions);
                IOHprofiler_free_memory(dimensions);
            }
        }
        IOHprofiler_free_memory(option_string);

        /* Check for redundant option keys for suite options */
        known_option_keys = IOHprofiler_option_keys_allocate(sizeof(known_keys_o) / sizeof(char *), known_keys_o);
        given_option_keys = IOHprofiler_option_keys(suite_options);

        if (given_option_keys) {
            redundant_option_keys = IOHprofiler_option_keys_get_redundant(known_option_keys, given_option_keys);

            if ((redundant_option_keys != NULL) && (redundant_option_keys->count > 0)) {
                /* Warn the user that some of given options are being ignored and output the valid options */
                char *output_redundant = IOHprofiler_option_keys_get_output_string(redundant_option_keys,
                                                                                   "IOHprofiler_suite(): Some keys in suite options were ignored:\n");
                char *output_valid = IOHprofiler_option_keys_get_output_string(known_option_keys,
                                                                               "Valid keys for suite options are:\n");
                IOHprofiler_warning("%s%s", output_redundant, output_valid);
                IOHprofiler_free_memory(output_redundant);
                IOHprofiler_free_memory(output_valid);
            }

            IOHprofiler_option_keys_free(given_option_keys);
            IOHprofiler_option_keys_free(redundant_option_keys);
        }
        IOHprofiler_option_keys_free(known_option_keys);
    }

    /* Check that there are enough dimensions, functions and instances left */
    if ((suite->number_of_dimensions < 1) || (suite->number_of_functions < 1) || (suite->number_of_instances < 1)) {
        IOHprofiler_error("IOHprofiler_suite(): the suite does not contain at least one dimension, function and instance");
        return NULL;
    }

    /* Set the starting values of the current indices in such a way, that when the instance_idx is incremented,
   * this results in a valid problem */
    IOHprofiler_suite_is_next_function_found(suite);
    IOHprofiler_suite_is_next_dimension_found(suite);

    return suite;
}

/**
 * Iterates through the suite first by instances, then by functions and finally by dimensions.
 * The instances/functions/dimensions that have been filtered out using the suite_options of the IOHprofiler_suite
 * function are skipped. Outputs some information regarding the current place in the iteration. The returned
 * problem is wrapped with the observer. If the observer is NULL, the returned problem is unobserved.
 *
 * @param suite The given suite.
 * @param observer The observer used to wrap the problem. If NULL, the problem is returned unobserved.
 *
 * @returns The next problem of the suite or NULL if there is no next problem left.
 */
IOHprofiler_problem_t *IOHprofiler_suite_get_next_problem(IOHprofiler_suite_t *suite, IOHprofiler_observer_t *observer) {
    size_t function_idx;
    size_t dimension_idx;
    size_t instance_idx;
    IOHprofiler_problem_t *problem;

    long previous_function_idx;
    long previous_dimension_idx;
    long previous_instance_idx;

    assert(suite != NULL);

    previous_function_idx = suite->current_function_idx;
    previous_dimension_idx = suite->current_dimension_idx;
    previous_instance_idx = suite->current_instance_idx;

    /* Iterate through the suite by instances, then functions and lastly dimensions in search for the next
   * problem. Note that these functions set the values of suite fields current_instance_idx,
   * current_function_idx and current_dimension_idx. */
    if (!IOHprofiler_suite_is_next_instance_found(suite) && !IOHprofiler_suite_is_next_function_found(suite) && !IOHprofiler_suite_is_next_dimension_found(suite)) {
        IOHprofiler_info_partial("done\n");
        return NULL;
    }

    if (suite->current_problem) {
        IOHprofiler_problem_free(suite->current_problem);
    }

    assert(suite->current_function_idx >= 0);
    assert(suite->current_dimension_idx >= 0);
    assert(suite->current_instance_idx >= 0);

    function_idx = (size_t)suite->current_function_idx;
    dimension_idx = (size_t)suite->current_dimension_idx;
    instance_idx = (size_t)suite->current_instance_idx;

    problem = IOHprofiler_suite_get_problem_from_indices(suite, function_idx, dimension_idx, instance_idx);
    if (observer != NULL)
        problem = IOHprofiler_problem_add_observer(problem, observer);

    suite->current_problem = problem;

    /* Output information regarding the current place in the iteration */
    if (IOHprofiler_log_level >= IOHprofiler_INFO) {
        if (((long)dimension_idx != previous_dimension_idx) || (previous_instance_idx < 0)) {
            /* A new dimension started */
            char *time_string = IOHprofiler_current_time_get_string();
            if (dimension_idx > 0)
                IOHprofiler_info_partial("done\n");
            else
                IOHprofiler_info_partial("\n");
            IOHprofiler_info_partial("IOHprofiler INFO: %s, d=%lu, running: \nf%02lu", time_string,
                                     (unsigned long)suite->dimensions[dimension_idx], (unsigned long)suite->functions[function_idx]);
            IOHprofiler_free_memory(time_string);
        } else if ((long)function_idx != previous_function_idx) {
            /* A new function started */
            IOHprofiler_info_partial("\nf%02lu", (unsigned long)suite->functions[function_idx]);
        }
        /* One dot for each instance */
        IOHprofiler_info_partial(".", suite->instances[instance_idx]);
    }
    return problem;
}

/**
 *
 * @returns The current problem of the suite.
 */
IOHprofiler_problem_t *IOHprofiler_suite_reset_problem(IOHprofiler_suite_t *suite, IOHprofiler_observer_t *observer) {
    size_t function_idx;
    size_t dimension_idx;
    size_t instance_idx;
    IOHprofiler_problem_t *problem;

    long previous_function_idx;
    long previous_dimension_idx;
    long previous_instance_idx;

    assert(suite != NULL);
    if (suite->current_problem) {
        IOHprofiler_problem_free(suite->current_problem);
    }

    previous_function_idx = suite->current_function_idx;
    previous_dimension_idx = suite->current_dimension_idx;
    previous_instance_idx = suite->current_instance_idx;

    function_idx = (size_t)suite->current_function_idx;
    dimension_idx = (size_t)suite->current_dimension_idx;
    instance_idx = (size_t)suite->current_instance_idx;

    problem = IOHprofiler_suite_get_problem_from_indices(suite, function_idx, dimension_idx, instance_idx);

    if (observer != NULL)
        problem = IOHprofiler_problem_add_observer(problem, observer);

    suite->current_problem = problem;

    /* Output information regarding the current place in the iteration */
    if (IOHprofiler_log_level >= IOHprofiler_INFO) {
        if (((long)dimension_idx != previous_dimension_idx) || (previous_instance_idx < 0)) {
            /* A new dimension started */
            char *time_string = IOHprofiler_current_time_get_string();
            if (dimension_idx > 0)
                IOHprofiler_info_partial("done\n");
            else
                IOHprofiler_info_partial("\n");
            IOHprofiler_info_partial("IOHprofiler INFO: %s, d=%lu, running: \nf%02lu", time_string,
                                     (unsigned long)suite->dimensions[dimension_idx], (unsigned long)suite->functions[function_idx]);
            IOHprofiler_free_memory(time_string);
        } else if ((long)function_idx != previous_function_idx) {
            /* A new function started */
            IOHprofiler_info_partial("\nf%02lu", (unsigned long)suite->functions[function_idx]);
        }
        /* One dot for each instance */
        IOHprofiler_info_partial(".", suite->instances[instance_idx]);
    }

    return problem;
}

/* See IOHprofiler.h for more information on encoding and decoding problem index */

/**
 * @param suite The suite.
 * @param function_idx Index of the function (starting with 0).
 * @param dimension_idx Index of the dimension (starting with 0).
 * @param instance_idx Index of the insatnce (starting with 0).
 *
 * @return The problem index in the suite computed from function_idx, dimension_idx and instance_idx.
 */
size_t IOHprofiler_suite_encode_problem_index(const IOHprofiler_suite_t *suite,
                                              const size_t function_idx,
                                              const size_t dimension_idx,
                                              const size_t instance_idx) {
    return instance_idx + (function_idx * suite->number_of_instances) +
           (dimension_idx * suite->number_of_instances * suite->number_of_functions);
}

/**
 * @param suite The suite.
 * @param problem_index Index of the problem in the suite (starting with 0).
 * @param function_idx Pointer to the index of the function, which is set by this function.
 * @param dimension_idx Pointer to the index of the dimension, which is set by this function.
 * @param instance_idx Pointer to the index of the instance, which is set by this function.
 */
void IOHprofiler_suite_decode_problem_index(const IOHprofiler_suite_t *suite,
                                            const size_t problem_index,
                                            size_t *function_idx,
                                            size_t *dimension_idx,
                                            size_t *instance_idx) {
    if (problem_index > (suite->number_of_instances * suite->number_of_functions * suite->number_of_dimensions) - 1) {
        IOHprofiler_warning("IOHprofiler_suite_decode_problem_index(): problem_index too large");
        function_idx = 0;
        instance_idx = 0;
        dimension_idx = 0;
        return;
    }

    *instance_idx = problem_index % suite->number_of_instances;
    *function_idx = (problem_index / suite->number_of_instances) % suite->number_of_functions;
    *dimension_idx = problem_index / (suite->number_of_instances * suite->number_of_functions);
}
#line 1 "code-experiments/src/IOHprofiler_observer.c"
/**
 * @file IOHprofiler_observer.c
 * @brief Definitions of functions regarding IOHprofiler observers.
 */

#line 7 "code-experiments/src/IOHprofiler_observer.c"
#line 8 "code-experiments/src/IOHprofiler_observer.c"
#include <limits.h>
#include <float.h>
#include <math.h>

/**
 * @brief The type for triggers based on target values.
 *
 * The target values that trigger logging are at every 10**(exponent/number_of_triggers) from positive
 * infinity down to precision, at 0, and from -precision on with step -10**(exponent/number_of_triggers) until
 * negative infinity.
 */
typedef struct {

  int exponent;               /**< @brief Value used to compare with the previously hit target. */
  double value;               /**< @brief Value of the currently hit target. */
  size_t number_of_triggers;  /**< @brief Number of target triggers between 10**i and 10**(i+1) for any i. */
  double precision;           /**< @brief Minimal precision of interest. */

} IOHprofiler_observer_targets_t;

/**
 * @brief The type for triggers based on numbers of evaluations.
 *
 * The numbers of evaluations that trigger logging are any of the two:
 * - every 10**(exponent1/number_of_triggers) for exponent1 >= 0
 * - every base_evaluation * dimension * (10**exponent2) for exponent2 >= 0
 */
typedef struct {

  /* First trigger */
  size_t value1;              /**< @brief The next value for the first trigger. */
  size_t exponent1;           /**< @brief Exponent used to compute the first trigger. */
  size_t number_of_triggers;  /**< @brief Number of target triggers between 10**i and 10**(i+1) for any i. */

  /* Second trigger */
  size_t value2;              /**< @brief The next value for the second trigger. */
  size_t exponent2;           /**< @brief Exponent used to compute the second trigger. */
  size_t *base_evaluations;   /**< @brief The base evaluation numbers used to compute the actual evaluation
                                   numbers that trigger logging. */
  size_t base_count;          /**< @brief The number of base evaluations. */
  size_t base_index;          /**< @brief The next index of the base evaluations. */
  size_t dimension;           /**< @brief Dimension used in the calculation of the first trigger. */

}IOHprofiler_observer_evaluations_t;

/**
 * @brief The type for triggers based on updating.
 * The traget values that trigger logging are at every updating iteration.
 */
typedef struct {
  double previous_value;      /**< @brief Evaluation last updated. */
}IOHprofiler_observer_update_t;



/**
 * @brief The maximum number of evaluations to trigger logging.
 *
 * @note This is not the maximal evaluation number to be logged, but the maximal number of times logging is
 * triggered by the number of evaluations.
 */
#define IOHprofiler_MAX_EVALS_TO_LOG 1000

/***********************************************************************************************************/

/**
 * @name Methods regarding triggers based on target values
 */

/**
 * @brief Creates and returns a structure containing information on targets.
 *
 * @param number_of_targets The number of targets between 10**(i/n) and 10**((i+1)/n) for each i.
 * @param precision Minimal precision of interest.
 */
static IOHprofiler_observer_targets_t *IOHprofiler_observer_targets(const size_t number_of_targets,
                                                      const double precision) {

  IOHprofiler_observer_targets_t *targets = (IOHprofiler_observer_targets_t *) IOHprofiler_allocate_memory(sizeof(*targets));
  targets->exponent = INT_MAX;
  targets->value = DBL_MAX;
  targets->number_of_triggers = number_of_targets;
  targets->precision = precision;

  return targets;
}

/**
 * @brief Computes and returns whether the given value should trigger logging.
 */
static int IOHprofiler_observer_targets_trigger(IOHprofiler_observer_targets_t *targets, const double given_value) {

  int update_performed = 0;

  const double number_of_targets_double = (double) (long) targets->number_of_triggers;

  double verified_value = 0;
  int last_exponent = 0;
  int current_exponent = 0;
  int adjusted_exponent = 0;

  assert(targets != NULL);

  /* The given_value is positive or zero */
  if (given_value >= 0) {

    if (given_value == 0) {
      /* If zero, use even smaller value than precision */
      verified_value = targets->precision / 10.0;
    } else if (given_value < targets->precision) {
      /* If close to zero, use precision instead of the given_value*/
      verified_value = targets->precision;
    } else {
      verified_value = given_value;
    }

    current_exponent = (int) (ceil(log10(verified_value) * number_of_targets_double));

    /* If this is the first time the update was called, set the last_exponent to some value greater than the
     * current exponent */
    if (last_exponent == INT_MAX) {
      last_exponent = current_exponent + 1;
    } else {
      last_exponent = targets->exponent;
    }

    if (current_exponent < last_exponent) {
      /* Update the target information */
      targets->exponent = current_exponent;
      if (given_value == 0)
        targets->value = 0;
      else
        targets->value = pow(10, (double) current_exponent / number_of_targets_double);
      update_performed = 1;
    }
  }
  /* The given_value is negative, therefore adjustments need to be made */
  else {

    /* If close to zero, use precision instead of the given_value*/
    if (given_value > -targets->precision) {
      verified_value = targets->precision;
    } else {
      verified_value = -given_value;
    }

    /* Adjustment: use floor instead of ceil! */
    current_exponent = (int) (floor(log10(verified_value) * number_of_targets_double));

    /* If this is the first time the update was called, set the last_exponent to some value greater than the
     * current exponent */
    if (last_exponent == INT_MAX) {
      last_exponent = current_exponent + 1;
    } else {
      last_exponent = targets->exponent;
    }

    /* Compute the adjusted_exponent in such a way, that it is always diminishing in value. The adjusted
     * exponent can only be used to verify if a new target has been hit. To compute the actual target
     * value, the current_exponent needs to be used. */
    adjusted_exponent = 2 * (int) (ceil(log10(targets->precision / 10.0) * number_of_targets_double))
        - current_exponent - 1;

    if (adjusted_exponent < last_exponent) {
      /* Update the target information */
      targets->exponent = adjusted_exponent;
      targets->value = - pow(10, (double) current_exponent / number_of_targets_double);
      update_performed = 1;
    }
  }

  return update_performed;
}

/**
 * @brief Creates and returns a structure containing information on triggers based on evaluation numbers.
 *
 * The numbers of evaluations that trigger logging are any of the two:
 * - every 10**(exponent1/number_of_triggers) for exponent1 >= 0
 * - every base_evaluation * dimension * (10**exponent2) for exponent2 >= 0
 *
 * @note The IOHprofiler_observer_evaluations_t object instances need to be freed using the
 * IOHprofiler_observer_evaluations_free function!
 *
 * @param base_evaluations Evaluation numbers formatted as a string, which are used as the base to compute
 * the second trigger. For example, if base_evaluations = "1,2,5", the logger will be triggered by
 * evaluations dim*1, dim*2, dim*5, 10*dim*1, 10*dim*2, 10*dim*5, 100*dim*1, 100*dim*2, 100*dim*5, ...
 */
static IOHprofiler_observer_evaluations_t *IOHprofiler_observer_evaluations(const char *base_evaluations,
                                                              const size_t dimension) {

  IOHprofiler_observer_evaluations_t *evaluations = (IOHprofiler_observer_evaluations_t *) IOHprofiler_allocate_memory(
      sizeof(*evaluations));

  /* First trigger */
  evaluations->value1 = 1;
  evaluations->exponent1 = 0;
  evaluations->number_of_triggers = 20;

  /* Second trigger */
  evaluations->base_evaluations = IOHprofiler_string_parse_ranges(base_evaluations, 1, 0, "base_evaluations",
      IOHprofiler_MAX_EVALS_TO_LOG);
  evaluations->dimension = dimension;
  evaluations->base_count = IOHprofiler_count_numbers(evaluations->base_evaluations, IOHprofiler_MAX_EVALS_TO_LOG,
      "base_evaluations");
  evaluations->base_index = 0;
  evaluations->value2 = dimension * evaluations->base_evaluations[0];
  evaluations->exponent2 = 0;

  return evaluations;
}

/**
 * @brief Computes and returns whether the given evaluation number triggers the first condition of the
 * logging based on the number of evaluations.
 *
 * The second condition is:
 * evaluation_number == 10**(exponent1/number_of_triggers)
 */
static int IOHprofiler_observer_evaluations_trigger_first(IOHprofiler_observer_evaluations_t *evaluations,
                                                   const size_t evaluation_number) {

  assert(evaluations != NULL);

  if (evaluation_number == evaluations->value1) {
    /* Compute the next value for the first trigger */
    while (IOHprofiler_double_to_size_t(floor(pow(10, (double) evaluations->exponent1 / (double) evaluations->number_of_triggers)) <= evaluations->value1)) {
      evaluations->exponent1++;
    }
    evaluations->value1 = IOHprofiler_double_to_size_t(floor(pow(10, (double) evaluations->exponent1 / (double) evaluations->number_of_triggers)));
    return 1;
  }
  return 0;
}

/**
 * @brief Computes and returns whether the given evaluation number triggers the second condition of the
 * logging based on the number of evaluations.
 *
 * The second condition is:
 * evaluation_number == base_evaluation[base_index] * dimension * (10**exponent2)
 */
static int IOHprofiler_observer_evaluations_trigger_second(IOHprofiler_observer_evaluations_t *evaluations,
                                                    const size_t evaluation_number) {

  assert(evaluations != NULL);

  if (evaluation_number == evaluations->value2) {
    /* Compute the next value for the second trigger */
    if (evaluations->base_index < evaluations->base_count - 1) {
      evaluations->base_index++;
    } else {
      evaluations->base_index = 0;
      evaluations->exponent2++;
    }
    evaluations->value2 = IOHprofiler_double_to_size_t(pow(10, (double) evaluations->exponent2)
        * (double) (long) evaluations->dimension
        * (double) (long) evaluations->base_evaluations[evaluations->base_index]);
    return 1;
  }
  return 0;
}

/**
 * @brief Returns 1 if any of the two triggers based on the number of evaluations equal 1 and 0 otherwise.
 *
 * The numbers of evaluations that trigger logging are any of the two:
 * - every 10**(exponent1/number_of_triggers) for exponent1 >= 0
 * - every base_evaluation * dimension * (10**exponent2) for exponent2 >= 0
 */
static int IOHprofiler_observer_evaluations_trigger(IOHprofiler_observer_evaluations_t *evaluations,
                                             const size_t evaluation_number) {

  /* Both functions need to be called so that both triggers are correctly updated */
  int first = IOHprofiler_observer_evaluations_trigger_first(evaluations, evaluation_number);
  int second = IOHprofiler_observer_evaluations_trigger_second(evaluations, evaluation_number);

  return (first + second > 0) ? 1: 0;
}

/**
 * @brief Frees the given evaluations object.
 */
static void IOHprofiler_observer_evaluations_free(IOHprofiler_observer_evaluations_t *evaluations) {

  assert(evaluations != NULL);
  IOHprofiler_free_memory(evaluations->base_evaluations);
  IOHprofiler_free_memory(evaluations);
}


/**
 * @brief Creates and returns a structure containing information on triggers every time updating.
 */
static IOHprofiler_observer_update_t  *IOHprofiler_observer_update(void) {
   IOHprofiler_observer_update_t *updating = (IOHprofiler_observer_update_t *) IOHprofiler_allocate_memory(sizeof(*updating));
   updating->previous_value = -DBL_MAX;

   return updating;
}


/**
 * @brief Returns 1 if current evaluation is larger than previous one.
 */
static int IOHprofiler_observer_update_trigger(IOHprofiler_observer_update_t *updating,
                                             const double evaluation) {

  int update_performed = 0;
  if(evaluation > updating->previous_value){
    updating->previous_value = evaluation;
    update_performed = 1;
  }
  return update_performed;
}

static void IOHprofiler_observer_update_free(IOHprofiler_observer_update_t *updating){
  assert(updating != NULL);
  IOHprofiler_free_memory(updating);
}

/**
 * @brief Returns 1 every interval times.
 * Probably being useful in future.
 */
/*
static int IOHprofiler_observer_interval_trigger(const int interval,
                                             const size_t evaluation_number) {

  int update_performed = 0;
  if(interval%evaluation_number == 0){
    update_performed = 1;
  }
  return update_performed;
}
*/


/**
 * @brief Allocates memory for a IOHprofiler_observer_t instance.
 */
static IOHprofiler_observer_t *IOHprofiler_observer_allocate(const char *result_folder,
                                               const char *observer_name,
                                               const char *algorithm_name,
                                               const char *algorithm_info,
                                               const size_t number_target_triggers,
                                               const double target_precision,
                                               const size_t number_evaluation_triggers,
                                               const char *base_evaluation_triggers,
                                               const size_t number_interval_triggers,
                                               const char *complete_triggers,
                                               const int precision_x,
                                               const int precision_f,
                                               const char *parameters_name) {

  IOHprofiler_observer_t *observer;
  observer = (IOHprofiler_observer_t *) IOHprofiler_allocate_memory(sizeof(*observer));
  /* Initialize fields to sane/safe defaults */
  observer->result_folder = IOHprofiler_strdup(result_folder);
  observer->observer_name = IOHprofiler_strdup(observer_name);
  observer->algorithm_name = IOHprofiler_strdup(algorithm_name);
  observer->algorithm_info = IOHprofiler_strdup(algorithm_info);
  observer->parameters_name = IOHprofiler_strdup(parameters_name);
  observer->number_target_triggers = number_target_triggers;
  observer->target_precision = target_precision;
  observer->number_evaluation_triggers = number_evaluation_triggers;
  observer->base_evaluation_triggers = IOHprofiler_strdup(base_evaluation_triggers);
  observer->number_interval_triggers = number_interval_triggers;
  observer->complete_triggers = IOHprofiler_strdup(complete_triggers);
  observer->precision_x = precision_x;
  observer->precision_f = precision_f;
  observer->data = NULL;
  observer->data_free_function = NULL;
  observer->logger_allocate_function = NULL;
  observer->logger_free_function = NULL;
  observer->is_active = 1;
  return observer;
}

void IOHprofiler_observer_free(IOHprofiler_observer_t *observer) {
  if (observer != NULL) {
    observer->is_active = 0;
    if (observer->observer_name != NULL)
      IOHprofiler_free_memory(observer->observer_name);
    if (observer->result_folder != NULL)
      IOHprofiler_free_memory(observer->result_folder);
    if (observer->algorithm_name != NULL)
      IOHprofiler_free_memory(observer->algorithm_name);
    if (observer->algorithm_info != NULL)
      IOHprofiler_free_memory(observer->algorithm_info);
    if (observer->base_evaluation_triggers != NULL)
      IOHprofiler_free_memory(observer->base_evaluation_triggers);
    if (observer->complete_triggers != NULL)
      IOHprofiler_free_memory(observer->complete_triggers);
    if (observer->data != NULL) {
      if (observer->data_free_function != NULL) {
        observer->data_free_function(observer->data);
      }
      IOHprofiler_free_memory(observer->data);
      observer->data = NULL;
    }

    observer->logger_allocate_function = NULL;
    observer->logger_free_function = NULL;

    IOHprofiler_free_memory(observer);
    observer = NULL;
  }
}

#line 1 "code-experiments/src/logger_PBO.c"
/**
 * @file logger_PBO.c
 * @brief Implementation of the IOHprofiler logger.
 *
 * Logs the performance of a single-objective optimizer on noisy or noiseless problems.
 * It produces four kinds of files:
 * - The "info" files ...
 * - The "dat" files ...
 * - The "tdat" files ...
 * - The "rdat" files ...
 */

/* TODO: Document this file in doxygen style! */

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <errno.h>
#include <stdarg.h>
#line 23 "code-experiments/src/logger_PBO.c"

#line 25 "code-experiments/src/logger_PBO.c"
#line 26 "code-experiments/src/logger_PBO.c"
#line 27 "code-experiments/src/logger_PBO.c"
#line 1 "code-experiments/src/observer_PBO.c"
/**
 * @file observer_bbob.c
 * @brief Implementation of the PBO observer.
 */

#line 7 "code-experiments/src/observer_PBO.c"
#line 8 "code-experiments/src/observer_PBO.c"

static IOHprofiler_problem_t *logger_PBO(IOHprofiler_observer_t *observer, IOHprofiler_problem_t *problem);
static void logger_PBO_free(void *logger);

/**
 * @brief The IOHprofiler observer data type.
 */
typedef struct {
    /* TODO: Can be used to store variables that need to be accessible during one run (i.e. for multiple
   * problems). For example, the following global variables from logger_bbob.c could be stored here: */
    size_t current_dim;
    size_t current_fun_id;
    /* ... and others */
} observer_PBO_data_t;

/**
 * @brief Initializes the bbob observer.
 */
static void observer_IOHprofiler(IOHprofiler_observer_t *observer, const char *options, IOHprofiler_option_keys_t **option_keys) {
    observer->logger_allocate_function = logger_PBO;
    observer->logger_free_function = logger_PBO_free;
    observer->data_free_function = NULL;
    observer->data = NULL;

    *option_keys = NULL;

    (void)options; /* To silence the compiler */
}
#line 28 "code-experiments/src/logger_PBO.c"

/*static const size_t IOHprofiler_nbpts_nbevals = 20; Wassim: tentative, are now observer options with these default values*/
/*static const size_t IOHprofiler_nbpts_fval = 5;*/
static size_t PBO_current_dim = 0;
static size_t PBO_current_funId = 0;
static size_t PBO_infoFile_firstInstance = 0;
char PBO_infoFile_firstInstance_char[3];
/* a possible solution: have a list of dims that are already in the file, if the ones we're about to log
 * is != PBO_current_dim and the funId is currend_funId, create a new .info file with as suffix the
 * number of the first instance */
static const int PBO_number_of_dimensions = 6;
static size_t PBO_dimensions_in_current_infoFile[6] = { 0, 0, 0, 0, 0, 0 }; /* TODO should use dimensions from the suite */

/* The current_... mechanism fails if several problems are open.
 * For the time being this should lead to an error.
 *
 * A possible solution: PBO_logger_is_open becomes a reference
 * counter and as long as another logger is open, always a new info
 * file is generated.
 * TODO: Shouldn't the new way of handling observers already fix this?
 */
static int PBO_logger_is_open = 0; /* this could become lock-list of .info files */

/* TODO: add possibility of adding a prefix to the index files (easy to do through observer options) */
static double PBO_fmax(double a, double b) {
  return (a > b) ? a : b;
}

/**
 * @brief The IOHprofiler logger data type.
 */
typedef struct {
  IOHprofiler_observer_t *observer;
  int is_initialized;
  /*char *path;// relative path to the data folder. //Wassim: now fetched from the observer */
  /*const char *alg_name; the alg name, for now, temporarily the same as the path. Wassim: Now in the observer */
  FILE *index_file; /* index file */
  FILE *fdata_file; /* function value aligned data file */
  FILE *tdata_file; /* number of function evaluations aligned data file */
  /*
  FILE *rdata_file;  restart info data file */

  FILE *idata_file; /* fixed interval aligned data file */
  FILE *cdata_file; /* complete data file */

  int f_flag; /* symbol for writing fdata_file*/
  int t_flag; /* symbol for writing tdata_file*/
  int i_flag; /* symbol for writing idata_file*/
  int c_flag; /* symbol for writing cdata_file*/
  size_t number_of_evaluations;
  double best_fvalue;
  double last_fvalue;
  double raw_value;
  double best_raw_value;
  short written_last_eval; /* allows writing the the data of the final fun eval in the .tdat file if not already written by the t_trigger*/
  int *best_solution;
  /* The following are to only pass data as a parameter in the free function. The
   * interface should probably be the same for all free functions so passing the
   * problem as a second parameter is not an option even though we need info
   * form it.*/
  size_t function_id; /*TODO: consider changing name*/
  size_t instance_id;
  size_t number_of_variables;
  size_t inner_dimension;
  double optimal_fvalue;

  size_t number_of_parameters;
  double *parameters;

  IOHprofiler_observer_targets_t *targets;          /**< @brief Triggers based on target values. */
  IOHprofiler_observer_evaluations_t *evaluations;  /**< @brief Triggers based on the number of evaluations. */
  IOHprofiler_observer_update_t *updating;          /**< @brief Triggers based on the updating. */
  size_t interval;                    /**< @brief The value of triggers interval. */

} logger_PBO_data_t;

static const char *PBO_file_header_str = "\"function evaluation\" "
    "\"current f(x)\" "
    "\"best-so-far f(x)\" "
    "\"current af(x)+b\"  "
    "\"best af(x)+b\" ";

static const char *PBO_file_para_str = "\"%s\" ";

/**
 * adds a formated line to a data file
 */
static void logger_PBO_write_data(FILE *target_file,
                                   size_t number_of_evaluations,
                                   double fvalue,
                                   double best_fvalue,
                                   double best_raw_value,
                                   double raw_value,
                                   const double *parameter,
                                   size_t number_of_parameters) {
  /* for some reason, it's %.0f in the old code instead of the 10.9e
   * in the documentation
   */
  size_t i;
  fprintf(target_file, "%lu %+10.5e %+10.5e %+10.5e %+10.5e", (unsigned long) number_of_evaluations,
      raw_value, best_raw_value, fvalue , best_fvalue);

  for (i = 0; i < number_of_parameters; i++) {
    fprintf(target_file, " %.6f", parameter[i]);
  }

  fprintf(target_file, "\n");
}

/**
 * Error when trying to create the file "path"
 */
static void logger_PBO_error_io(FILE *path, int errnum) {
  const char *error_format = "Error opening file: %s\n ";
  IOHprofiler_error(error_format, strerror(errnum), path);
}

/**
 * Creates the data files or simply opens it
 */


static void logger_PBO_open_dataFile(FILE **target_file,
                                      const char *path,
                                      const char *dataFile_path,
                                      const char *file_extension) {
  char file_path[IOHprofiler_PATH_MAX] = { 0 };
  char relative_filePath[IOHprofiler_PATH_MAX] = { 0 };
  int errnum;
  strncpy(relative_filePath, dataFile_path,
  IOHprofiler_PATH_MAX - strlen(relative_filePath) - 1);
  strncat(relative_filePath, file_extension,
  IOHprofiler_PATH_MAX - strlen(relative_filePath) - 1);
  IOHprofiler_join_path(file_path, sizeof(file_path), path, relative_filePath, NULL);
  if (*target_file == NULL) {
    *target_file = fopen(file_path, "a+");
    errnum = errno;
    if (*target_file == NULL) {
      logger_PBO_error_io(*target_file, errnum);
    }
  }
}


/**
 * Creates the index file fileName_prefix+problem_id+file_extension in
 * folde_path
 */
static void logger_PBO_openIndexFile(logger_PBO_data_t *logger,
                                      const char *folder_path,
                                      const char *indexFile_prefix,
                                      const char *function_id,
                                      const char *dataFile_path,
                                      const char *suite_name) {
  /* to add the instance number TODO: this should be done outside to avoid redoing this for the .*dat files */
  char used_dataFile_path[IOHprofiler_PATH_MAX] = { 0 };
  int errnum, newLine; /* newLine is at 1 if we need a new line in the info file */
  char function_id_char[3]; /* TODO: consider adding them to logger */
  char file_name[IOHprofiler_PATH_MAX] = { 0 };
  char file_path[IOHprofiler_PATH_MAX] = { 0 };
  FILE **target_file;
  FILE *tmp_file;
  strncpy(used_dataFile_path, dataFile_path, IOHprofiler_PATH_MAX - strlen(used_dataFile_path) - 1);
  if (PBO_infoFile_firstInstance == 0) {
    PBO_infoFile_firstInstance = logger->instance_id;
  }
  sprintf(function_id_char, "%lu", (unsigned long) logger->function_id);
  sprintf(PBO_infoFile_firstInstance_char, "%lu", (unsigned long) PBO_infoFile_firstInstance);
  target_file = &(logger->index_file);
  tmp_file = NULL; /* to check whether the file already exists. Don't want to use target_file */
  strncpy(file_name, indexFile_prefix, IOHprofiler_PATH_MAX - strlen(file_name) - 1);
  strncat(file_name, "_f", IOHprofiler_PATH_MAX - strlen(file_name) - 1);
  strncat(file_name, function_id_char, IOHprofiler_PATH_MAX - strlen(file_name) - 1);
  strncat(file_name, "_i", IOHprofiler_PATH_MAX - strlen(file_name) - 1);
  strncat(file_name, PBO_infoFile_firstInstance_char, IOHprofiler_PATH_MAX - strlen(file_name) - 1);
  strncat(file_name, ".info", IOHprofiler_PATH_MAX - strlen(file_name) - 1);
  IOHprofiler_join_path(file_path, sizeof(file_path), folder_path, file_name, NULL);
  if (*target_file == NULL) {
    tmp_file = fopen(file_path, "r"); /* to check for existence */
    if ((tmp_file) && (PBO_current_dim == logger->inner_dimension)
        && (PBO_current_funId == logger->function_id)) {
        /* new instance of current funId and current dim */
      newLine = 0;
      *target_file = fopen(file_path, "a+");
      if (*target_file == NULL) {
        errnum = errno;
        logger_PBO_error_io(*target_file, errnum);
      }
      fclose(tmp_file);
    } else { /* either file doesn't exist (new funId) or new Dim */
      /* check that the dim was not already present earlier in the file, if so, create a new info file */
      if (PBO_current_dim != logger->inner_dimension) {
        int i, j;
        for (i = 0;
            i < PBO_number_of_dimensions && PBO_dimensions_in_current_infoFile[i] != 0
                && PBO_dimensions_in_current_infoFile[i] != logger->inner_dimension; i++) {
          ; /* checks whether dimension already present in the current infoFile */
        }
        if (i < PBO_number_of_dimensions && PBO_dimensions_in_current_infoFile[i] == 0) {
          /* new dimension seen for the first time */
          PBO_dimensions_in_current_infoFile[i] = logger->inner_dimension;
          newLine = 1;
        } else {
          if (i < PBO_number_of_dimensions) { /* dimension already present, need to create a new file */
            newLine = 0;
            file_path[strlen(file_path) - strlen(PBO_infoFile_firstInstance_char) - 7] = 0; /* truncate the instance part */
            PBO_infoFile_firstInstance = logger->instance_id;
            sprintf(PBO_infoFile_firstInstance_char, "%lu", (unsigned long) PBO_infoFile_firstInstance);
            strncat(file_path, "_i", IOHprofiler_PATH_MAX - strlen(file_name) - 1);
            strncat(file_path, PBO_infoFile_firstInstance_char, IOHprofiler_PATH_MAX - strlen(file_name) - 1);
            strncat(file_path, ".info", IOHprofiler_PATH_MAX - strlen(file_name) - 1);
          } else {/*we have all dimensions*/
            newLine = 1;
          }
          for (j = 0; j < PBO_number_of_dimensions; j++) { /* new info file, reinitialize list of dims */
            PBO_dimensions_in_current_infoFile[j] = 0;
          }
          PBO_dimensions_in_current_infoFile[i] = logger->inner_dimension;
        }
      } else {
        if ( PBO_current_funId != logger->function_id ) {
          /*new function in the same file */
          newLine = 1;
        }
      }
      *target_file = fopen(file_path, "a+"); /* in any case, we append */
      if (*target_file == NULL) {
        errnum = errno;
        logger_PBO_error_io(*target_file, errnum);
      }
      if (tmp_file) { /* File already exists, new dim so just a new line. Also, close the tmp_file */
        if (newLine) {
          fprintf(*target_file, "\n");
        }
        fclose(tmp_file);
      }

      fprintf(*target_file,
          "suite = '%s', funcId = %d, DIM = %lu, algId = '%s', algInfo = '%s'\n",
          suite_name, (int) strtol(function_id, NULL, 10), (unsigned long) logger->inner_dimension,
          logger->observer->algorithm_name, logger->observer->algorithm_info);

      fprintf(*target_file, "%%\n");
      strncat(used_dataFile_path, "_i", IOHprofiler_PATH_MAX - strlen(used_dataFile_path) - 1);
      strncat(used_dataFile_path, PBO_infoFile_firstInstance_char,
      IOHprofiler_PATH_MAX - strlen(used_dataFile_path) - 1);
      fprintf(*target_file, "%s.dat", used_dataFile_path); /* dataFile_path does not have the extension */
      PBO_current_dim = logger->inner_dimension;
      PBO_current_funId = logger->function_id;
    }
  }
}

/**
 * Generates the different files and folder needed by the logger to store the
 * data if these don't already exist
 */
static void logger_PBO_initialize(logger_PBO_data_t *logger, IOHprofiler_problem_t *inner_problem) {
  /*
   Creates/opens the data and index files
   */
  char dataFile_path[IOHprofiler_PATH_MAX] = { 0 }; /* relative path to the .dat file from where the .info file is */
  char folder_path[IOHprofiler_PATH_MAX] = { 0 };
  char *tmpc_funId; /* serves to extract the function id as a char *. There should be a better way of doing this! */
  char *tmpc_dim; /* serves to extract the dimension as a char *. There should be a better way of doing this! */
  char indexFile_prefix[15] = "IOHprofiler"; /* TODO (minor): make the prefix IOHprofilerexp a parameter that the user can modify */
  size_t str_length_funId, str_length_dim;

  str_length_funId = IOHprofiler_double_to_size_t(PBO_fmax(1, ceil(log10((double) IOHprofiler_problem_get_suite_dep_function(inner_problem)))));
  str_length_dim = IOHprofiler_double_to_size_t(PBO_fmax(1, ceil(log10((double) inner_problem->dimension))));
  tmpc_funId = IOHprofiler_allocate_string(str_length_funId);
  tmpc_dim = IOHprofiler_allocate_string(str_length_dim);

  assert(logger != NULL);
  assert(inner_problem != NULL);
  assert(inner_problem->problem_id != NULL);

  sprintf(tmpc_funId, "%lu", (unsigned long) IOHprofiler_problem_get_suite_dep_function(inner_problem));
  sprintf(tmpc_dim, "%lu", (unsigned long) inner_problem->dimension);

  /* prepare paths and names */
  strncpy(dataFile_path, "data_f", IOHprofiler_PATH_MAX);
  strncat(dataFile_path, tmpc_funId,
  IOHprofiler_PATH_MAX - strlen(dataFile_path) - 1);
  IOHprofiler_join_path(folder_path, sizeof(folder_path), logger->observer->result_folder, dataFile_path,
  NULL);
  IOHprofiler_create_directory(folder_path);
  strncat(dataFile_path, "/IOHprofiler_f",
  IOHprofiler_PATH_MAX - strlen(dataFile_path) - 1);
  strncat(dataFile_path, tmpc_funId,
  IOHprofiler_PATH_MAX - strlen(dataFile_path) - 1);
  strncat(dataFile_path, "_DIM", IOHprofiler_PATH_MAX - strlen(dataFile_path) - 1);
  strncat(dataFile_path, tmpc_dim, IOHprofiler_PATH_MAX - strlen(dataFile_path) - 1);

  /* index/info file */
  assert(IOHprofiler_problem_get_suite(inner_problem));
  logger_PBO_openIndexFile(logger, logger->observer->result_folder, indexFile_prefix, tmpc_funId,
      dataFile_path, IOHprofiler_problem_get_suite(inner_problem)->suite_name);
  fprintf(logger->index_file, ", %lu", (unsigned long) IOHprofiler_problem_get_suite_dep_instance(inner_problem));
  /* data files */
  /* TODO: definitely improvable but works for now */
  strncat(dataFile_path, "_i", IOHprofiler_PATH_MAX - strlen(dataFile_path) - 1);
  strncat(dataFile_path, PBO_infoFile_firstInstance_char,
  IOHprofiler_PATH_MAX - strlen(dataFile_path) - 1);

  if(logger->f_flag == 1){
    logger_PBO_open_dataFile(&(logger->fdata_file), logger->observer->result_folder, dataFile_path, ".dat");
    fprintf(logger->fdata_file,"%s", PBO_file_header_str);
    if(logger->observer->parameters_name != NULL)
      fprintf(logger->fdata_file, PBO_file_para_str,logger->observer->parameters_name);
    fprintf(logger->fdata_file, "\n");
  }

  if(logger->t_flag == 1){
    logger_PBO_open_dataFile(&(logger->tdata_file), logger->observer->result_folder, dataFile_path, ".tdat");
    fprintf(logger->tdata_file,"%s", PBO_file_header_str);
    if(logger->observer->parameters_name != NULL)
      fprintf(logger->tdata_file, PBO_file_para_str,logger->observer->parameters_name);
    fprintf(logger->tdata_file, "\n");
  }

  if(logger->i_flag == 1){
    logger_PBO_open_dataFile(&(logger->idata_file), logger->observer->result_folder, dataFile_path, ".idat");
    fprintf(logger->idata_file, "%s",PBO_file_header_str);
    if(logger->observer->parameters_name != NULL)
      fprintf(logger->idata_file, PBO_file_para_str,logger->observer->parameters_name);
    fprintf(logger->idata_file, "\n");
  }

  if(logger->c_flag == 1){
    logger_PBO_open_dataFile(&(logger->cdata_file), logger->observer->result_folder, dataFile_path, ".cdat");
    fprintf(logger->cdata_file, "%s",PBO_file_header_str);
    if(logger->observer->parameters_name != NULL)
      fprintf(logger->cdata_file, PBO_file_para_str,logger->observer->parameters_name);
    fprintf(logger->cdata_file, "\n");
  }
  /*
  logger_PBO_open_dataFile(&(logger->rdata_file), logger->observer->result_folder, dataFile_path, ".rdat");
  fprintf(logger->rdata_file,"%s", PBO_file_header_str);
  if(logger->observer->parameters_name != NULL)
    fprintf(logger->rdata_file, PBO_file_para_str,logger->observer->parameters_name);
  fprintf(logger->rdata_file, "\n");
  */
  logger->is_initialized = 1;
  IOHprofiler_free_memory(tmpc_dim);
  IOHprofiler_free_memory(tmpc_funId);
}

/**
 * Layer added to the transformed-problem evaluate_function by the logger
 */
static void logger_PBO_evaluate(IOHprofiler_problem_t *problem, const int *x, double *y) {
  logger_PBO_data_t *logger = (logger_PBO_data_t *) IOHprofiler_problem_transformed_get_data(problem);
  IOHprofiler_problem_t * inner_problem = IOHprofiler_problem_transformed_get_inner_problem(problem);


  size_t i;
  if (!logger->is_initialized) {
    logger_PBO_initialize(logger, inner_problem);
  }
  if ((IOHprofiler_log_level >= IOHprofiler_DEBUG) && logger->number_of_evaluations == 0) {
    IOHprofiler_debug("%4lu: ", (unsigned long) inner_problem->suite_dep_index);
    IOHprofiler_debug("on problem %s ... ", IOHprofiler_problem_get_id(inner_problem));
  }
  IOHprofiler_evaluate_function(inner_problem, x, y);

  /*Update parameters' info*/
  logger->number_of_parameters = problem->number_of_parameters;
  if(logger->number_of_parameters > 0){
    if(logger->parameters != NULL){
        IOHprofiler_free_memory(logger->parameters);
    }
    logger->parameters = IOHprofiler_allocate_vector(logger->number_of_parameters);
    for(i = 0; i < logger->number_of_parameters; ++i)
      logger->parameters[i] = problem->parameters[i];
  }
  logger->last_fvalue = y[0];
  logger->written_last_eval = 0;
  if (logger->number_of_evaluations == 0 || y[0] > logger->best_fvalue) {
    logger->best_fvalue = y[0];

    for (i = 0; i < problem->number_of_variables; i++)
      logger->best_solution[i] = x[i];
  }
  logger->raw_value = inner_problem->raw_fitness[0];
  if(logger->number_of_evaluations == 0 || logger->raw_value > logger->best_raw_value)
    logger->best_raw_value = logger->raw_value;


  logger->number_of_evaluations++;
  if(logger->f_flag == 1){
    if (IOHprofiler_observer_update_trigger(logger->updating,y[0])) {
      logger->best_fvalue = y[0];
      for (i = 0; i < problem->number_of_variables; i++)
        logger->best_solution[i] = x[i];
        logger_PBO_write_data(
          logger->fdata_file,
          logger->number_of_evaluations,
          y[0],
          logger->best_fvalue,
          logger->best_raw_value,
          logger->raw_value,
          problem->parameters,
          problem->number_of_parameters);
    }
  }
  if(logger->t_flag == 1){
    /* Add a line in the .tdat file each time an fevals trigger is reached.*/
    if (IOHprofiler_observer_evaluations_trigger(logger->evaluations, logger->number_of_evaluations)) {
      logger_PBO_write_data(
          logger->tdata_file,
          logger->number_of_evaluations,
          y[0],
          logger->best_fvalue,
          logger->best_raw_value,
          logger->raw_value,
          problem->parameters,
          problem->number_of_parameters);
      logger->written_last_eval = 1;
    }
  }

  if(logger->i_flag == 1){
    if ( logger->number_of_evaluations % logger->interval == 0) {
      logger_PBO_write_data(
          logger->idata_file,
          logger->number_of_evaluations,
          y[0],
          logger->best_fvalue,
          logger->best_raw_value,
          logger->raw_value,
          problem->parameters,
          problem->number_of_parameters);
    }
  }

  if(logger->c_flag == 1){
    logger_PBO_write_data(
        logger->cdata_file,
        logger->number_of_evaluations,
        y[0],
        logger->best_fvalue,
        logger->best_raw_value,
        logger->raw_value,
        problem->parameters,
        problem->number_of_parameters);
  }
  /* Add sanity check for optimal f value */
  assert(y[0] <= logger->optimal_fvalue);

  /* Flush output so that impatient users can see progress. */
  if(logger->f_flag == 1)
    fflush(logger->fdata_file);
}

/**
 * Also serves as a finalize run method so. Must be called at the end
 * of Each run to correctly fill the index file
 *
 * TODO: make sure it is called at the end of each run or move the
 * writing into files to another function
 */
static void logger_PBO_free(void *stuff) {
  /* TODO: do all the "non simply freeing" stuff in another function
   * that can have problem as input
   */
  logger_PBO_data_t *logger = (logger_PBO_data_t *) stuff;

  if ((IOHprofiler_log_level >= IOHprofiler_DEBUG) && logger && logger->number_of_evaluations > 0) {
    IOHprofiler_debug("best f=%e after %lu fevals (done observing)\n", logger->best_fvalue,
        (unsigned long) logger->number_of_evaluations);
  }
  if (logger->index_file != NULL) {
    fprintf(logger->index_file, ":%lu|%.5e", (unsigned long) logger->number_of_evaluations,
        logger->best_fvalue);
    fclose(logger->index_file);
    logger->index_file = NULL;
  }
  if (logger->fdata_file != NULL) {
    fclose(logger->fdata_file);
    logger->fdata_file = NULL;
  }
  if (logger->tdata_file != NULL) {
    /* TODO: make sure it handles restarts well. i.e., it writes
     * at the end of a single run, not all the runs on a given
     * instance. Maybe start with forcing it to generate a new
     * "instance" of problem for each restart in the beginning
     */
    if (!logger->written_last_eval) {
      logger_PBO_write_data(logger->tdata_file, logger->number_of_evaluations, logger->last_fvalue,
          logger->best_fvalue, logger->best_raw_value, logger->raw_value, logger->parameters, logger->number_of_parameters);
    }
    fclose(logger->tdata_file);
    logger->tdata_file = NULL;
  }
  /*
  if (logger->rdata_file != NULL) {
    fclose(logger->rdata_file);
    logger->rdata_file = NULL;
  }*/
  if (logger->idata_file != NULL) {
    logger_PBO_write_data(logger->idata_file, logger->number_of_evaluations, logger->last_fvalue,
          logger->best_fvalue, logger->best_raw_value, logger->raw_value, logger->parameters, logger->number_of_parameters);
    fclose(logger->idata_file);
    logger->idata_file = NULL;
  }

  if (logger->cdata_file != NULL) {
    fclose(logger->cdata_file);
    logger->cdata_file = NULL;
  }

  if (logger->best_solution != NULL) {
    IOHprofiler_free_memory(logger->best_solution);
    logger->best_solution = NULL;
  }

  if (logger->targets != NULL){
    IOHprofiler_free_memory(logger->targets);
    logger->targets = NULL;
  }

  if (logger->evaluations != NULL){
    IOHprofiler_observer_evaluations_free(logger->evaluations);
    logger->evaluations = NULL;
  }

  if (logger->updating != NULL){
    IOHprofiler_observer_update_free(logger->updating);
    logger->updating = NULL;
  }

  if (logger->parameters != NULL){
    IOHprofiler_free_memory(logger->parameters);
    logger->parameters = NULL;
  }

  PBO_logger_is_open = 0;
}

static IOHprofiler_problem_t *logger_PBO(IOHprofiler_observer_t *observer, IOHprofiler_problem_t *inner_problem) {
  logger_PBO_data_t *logger_PBO;
  IOHprofiler_problem_t *problem;

  logger_PBO = (logger_PBO_data_t *) IOHprofiler_allocate_memory(sizeof(*logger_PBO));
  logger_PBO->observer = observer;

  if (inner_problem->number_of_objectives != 1) {
    IOHprofiler_warning("logger_PBO(): The IOHprofiler logger shouldn't be used to log a problem with %d objectives",
        inner_problem->number_of_objectives);
  }

  if (PBO_logger_is_open)
    IOHprofiler_error("The current IOHprofiler_logger (observer) must be closed before a new one is opened");
  /* This is the name of the folder which happens to be the algName */
  /*logger->path = IOHprofiler_strdup(observer->output_folder);*/
  logger_PBO->index_file = NULL;
  logger_PBO->fdata_file = NULL;
  logger_PBO->tdata_file = NULL;
  /* delete rdat files*/
  /*
  logger_PBO->rdata_file = NULL;
  */
  logger_PBO->cdata_file = NULL;
  logger_PBO->idata_file = NULL;
  logger_PBO->f_flag = 0;
  logger_PBO->t_flag = 0;
  logger_PBO->i_flag = 0;
  logger_PBO->c_flag = 0;




  logger_PBO->f_flag = 1;
  if(strcmp(observer->base_evaluation_triggers,"") != 0)
    logger_PBO->t_flag = 1;
  if(observer->number_interval_triggers != 0)
    logger_PBO->i_flag = 1;
  if(strcmp(observer->complete_triggers,"true") == 0)
    logger_PBO->c_flag = 1;
  logger_PBO->number_of_variables = inner_problem->number_of_variables;
  logger_PBO->inner_dimension = inner_problem->dimension;
  logger_PBO->number_of_parameters = inner_problem->number_of_parameters;
  logger_PBO->parameters = NULL;
  if (inner_problem->best_value == NULL) {
    /* IOHprofiler_error("Optimal f value must be defined for each problem in order for the logger to work properly"); */
    /* Setting the value to 0 results in the assertion y>=optimal_fvalue being susceptible to failure */
    IOHprofiler_warning("undefined optimal f value. Set to 0");
    logger_PBO->optimal_fvalue = 0;
  } else {
    logger_PBO->optimal_fvalue = *(inner_problem->best_value);
  }

  logger_PBO->number_of_evaluations = 0;
  logger_PBO->best_solution = IOHprofiler_allocate_int_vector(inner_problem->number_of_variables);
  /* TODO: the following inits are just to be in the safe side and
   * should eventually be removed. Some fields of the IOHprofiler_logger struct
   * might be useless
   */
  logger_PBO->function_id = IOHprofiler_problem_get_suite_dep_function(inner_problem);
  logger_PBO->instance_id = IOHprofiler_problem_get_suite_dep_instance(inner_problem);
  logger_PBO->written_last_eval = 1;
  logger_PBO->last_fvalue = -DBL_MAX;
  logger_PBO->is_initialized = 0;
  logger_PBO->best_fvalue = -DBL_MAX;
  logger_PBO->best_raw_value  = -DBL_MAX;
  /* Initialize triggers based on target values and number of evaluations */
  logger_PBO->targets = IOHprofiler_observer_targets(observer->number_target_triggers, observer->target_precision);
  logger_PBO->evaluations = IOHprofiler_observer_evaluations(observer->base_evaluation_triggers, inner_problem->number_of_variables);
  logger_PBO->updating = IOHprofiler_observer_update();
  logger_PBO->interval = observer->number_interval_triggers;
  problem = IOHprofiler_problem_transformed_allocate(inner_problem, logger_PBO, logger_PBO_free, observer->observer_name);

  problem->evaluate_function = logger_PBO_evaluate;
  PBO_logger_is_open = 1;
  return problem;
}
#line 419 "code-experiments/src/IOHprofiler_observer.c"


/**
 * Currently, one observers is supported:
 * - "IOHprofiler" is the observer for single-objective problems with known optima, which is available to
 * creates *.info, *.dat, *.tdat, *.idat, *rdat and *.cdat files and logs the distance to the optimum and parameters.
 *
 * @param observer_name A string containing the name of the observer. Currently supported observer name is
 * "IOHprofiler". Strings "no_observer", "" or NULL return NULL.
 * @param observer_options A string of pairs "key: value" used to pass the options to the observer.
 * - "result_folder: NAME" determines the folder within the "result_folder" folder into which the results will be
 * output. If the folder with the given name already exists, first NAME_001 will be tried, then NAME_002 and
 * so on. The default value is "default".
 * - "algorithm_name: NAME", where NAME is a short name of the algorithm that will be used in plots (no
 * spaces are allowed). The default value is "ALG".
 * - "algorithm_info: STRING" stores the description of the algorithm. If it contains spaces, it must be
 * surrounded by double quotes. The default value is "" (no description).
 * - "number_target_triggers: VALUE" defines the number of targets between each 10**i and 10**(i+1)
 * (equally spaced in the logarithmic scale) that trigger logging. The default value is 0, which means *.dat files
 * will not be generated.
 * - "target_precision: VALUE" defines the precision used for targets (there are no targets for
 * abs(values) < target_precision). The default value is 1e-8.
 * - "number_evaluation_triggers: VALUE" defines the number of evaluations to be logged between each 10**i
 * and 10**(i+1). The default value is 0.
 * - "base_evaluation_triggers: VALUES" defines the base evaluations used to produce an additional
 * evaluation-based logging. The numbers of evaluations that trigger logging are every
 * base_evaluation * dimension * (10**i). For example, if base_evaluation_triggers = "1,2,5", the logger will
 * be triggered by evaluations dim*1, dim*2, dim*5, 10*dim*1, 10*dim*2, 10*dim*5, 100*dim*1, 100*dim*2,
 * 100*dim*5, ... The default value is "", which means *.tdat files are not generated.
 * - "number_interval_triggers: VALUES" defines the frequency for targets. The default of number_interval_triggers
 * is set as 0, which means the *.idat files are not generated.
 * - "complete_triggers: VALUE" defines the option if generates *.cdat files. If complete_triggers is equal to "TRUE",
 * "True" or "true", *.cdat files are generated to save each evaluation. The default value is "FALES".
 * - "precision_x: VALUE" defines the precision used when outputting variables and corresponds to the number
 * of digits to be printed after the decimal point. The default value is 8.
 * - "precision_f: VALUE" defines the precision used when outputting f values and corresponds to the number of
 * digits to be printed after the decimal point. The default value is 15.
 * - "parameters_name: STRING" defines the names of paramters to be logged. The default value is parameters ( if exist )
 *
 * @return The constructed observer object or NULL if observer_name equals NULL, "" or "no_observer".
 */
IOHprofiler_observer_t *IOHprofiler_observer(const char *observer_name, const char *observer_options) {

  IOHprofiler_observer_t *observer;
  char *path, *result_folder, *algorithm_name, *algorithm_info;
  const char *outer_folder_name = "";
  int precision_x, precision_f;

  size_t number_target_triggers;
  size_t number_evaluation_triggers;
  size_t number_interval_triggers;
  double target_precision;
  char *base_evaluation_triggers;
  char *complete_triggers;
  char *parameters_namet;
  char *parameters_name;
  size_t i,j,flag,len;

  IOHprofiler_option_keys_t *known_option_keys, *given_option_keys, *additional_option_keys, *redundant_option_keys;

  /* Sets the valid keys for observer options
   * IMPORTANT: This list should be up-to-date with the code and the documentation */
  const char *known_keys[] = { "result_folder", "algorithm_name", "algorithm_info",
      "number_target_triggers", "target_precision", "number_evaluation_triggers", "base_evaluation_triggers", "number_interval_triggers",
      "complete_triggers","precision_x", "precision_f" };
  additional_option_keys = NULL; /* To be set by the chosen observer */

  if (0 == strcmp(observer_name, "no_observer")) {
    return NULL;
  } else if (strlen(observer_name) == 0) {
    IOHprofiler_warning("Empty observer_name has no effect. To prevent this warning use 'no_observer' instead");
    return NULL;
  }

  result_folder = IOHprofiler_allocate_string(IOHprofiler_PATH_MAX);
  algorithm_name = IOHprofiler_allocate_string(IOHprofiler_PATH_MAX);
  parameters_name = IOHprofiler_allocate_string(IOHprofiler_PATH_MAX);
  parameters_namet = IOHprofiler_allocate_string(IOHprofiler_PATH_MAX);
  algorithm_info = IOHprofiler_allocate_string(IOHprofiler_PATH_MAX);
  complete_triggers = IOHprofiler_allocate_string(IOHprofiler_PATH_MAX);
  /* Read result_folder, algorithm_name and algorithm_info from the observer_options and use
   * them to initialize the observer */
  if (IOHprofiler_options_read_string(observer_options, "result_folder", result_folder) == 0) {
    strcpy(result_folder, "EXP");
  }
  /* Create the result_folder inside the "exdata" folder */
  path = IOHprofiler_allocate_string(IOHprofiler_PATH_MAX);
  memcpy(path, outer_folder_name, strlen(outer_folder_name) + 1);
  IOHprofiler_join_path(path, IOHprofiler_PATH_MAX, result_folder, NULL);
  IOHprofiler_create_unique_directory(&path);
  IOHprofiler_info("Results will be output to folder %s", path);

  if (IOHprofiler_options_read_string(observer_options, "algorithm_name", algorithm_name) == 0) {
    strcpy(algorithm_name, "ALG");
  }

  if (IOHprofiler_options_read_string(observer_options, "algorithm_info", algorithm_info) == 0) {
    strcpy(algorithm_info, "");
  }

  number_target_triggers = 0;
  if (IOHprofiler_options_read_size_t(observer_options, "number_target_triggers", &number_target_triggers) != 0) {
    if (number_target_triggers <= 0)
      number_target_triggers = 0;
  }

  target_precision = 1e-8;
  if (IOHprofiler_options_read_double(observer_options, "target_precision", &target_precision) != 0) {
    if ((target_precision > 1) || (target_precision <= 0))
      target_precision = 1e-8;
  }

  number_evaluation_triggers = 0;
  if (IOHprofiler_options_read_size_t(observer_options, "number_evaluation_triggers", &number_evaluation_triggers) != 0) {
    if (number_evaluation_triggers < 4)
      number_evaluation_triggers = 0;
  }

  base_evaluation_triggers = IOHprofiler_allocate_string(IOHprofiler_PATH_MAX);
  if (IOHprofiler_options_read_string(observer_options, "base_evaluation_triggers", base_evaluation_triggers) == 0) {
    strcpy(base_evaluation_triggers, "1,2,5");
  }

  precision_x = 8;
  if (IOHprofiler_options_read_int(observer_options, "precision_x", &precision_x) != 0) {
    if ((precision_x < 1) || (precision_x > 32))
      precision_x = 8;
  }

  precision_f = 15;
  if (IOHprofiler_options_read_int(observer_options, "precision_f", &precision_f) != 0) {
    if ((precision_f < 1) || (precision_f > 32))
      precision_f = 15;
  }

  number_interval_triggers = 0;
  if (IOHprofiler_options_read_size_t(observer_options, "number_interval_triggers", &number_interval_triggers) != 0) {
    if(number_interval_triggers == 0)
      number_interval_triggers = 0;
  }

  if (IOHprofiler_options_read_string(observer_options, "complete_triggers", complete_triggers) != 0) {
    if(strcmp(complete_triggers,"TRUE") == 0 || strcmp(complete_triggers,"True") == 0 || strcmp(complete_triggers,"true") == 0)
      strcpy(complete_triggers,"true");
  }
  else
    strcpy(complete_triggers,"FALSE");
  if (IOHprofiler_options_read_string(observer_options, "parameters_name", parameters_namet) == 0 || strcmp(parameters_namet,"") == 0) {
     strncpy(parameters_name,"No parameters",IOHprofiler_PATH_MAX);
  }
  else{
    j = 0;
    flag = 0;
    len = strlen(parameters_namet);
    for(i = 0; i != len; ++i){
      if(parameters_namet[i] == ','){
         if(flag == 0){
          parameters_name[j] = '\"';
          j++;
          parameters_name[j] = ' ';
          j++;
          parameters_name[j] = '\"';
          j++;
          flag = 1;
        }
        else
          continue;
      }
      else{
        parameters_name[j] = parameters_namet[i];
        flag = 0;
        j++;
      }
    }
    parameters_name[j] = '\0';
  }
  observer = IOHprofiler_observer_allocate(path, observer_name, algorithm_name, algorithm_info,
      number_target_triggers, target_precision, number_evaluation_triggers, base_evaluation_triggers,number_interval_triggers,complete_triggers,
      precision_x, precision_f,parameters_name);
  IOHprofiler_free_memory(complete_triggers);
  IOHprofiler_free_memory(path);
  IOHprofiler_free_memory(result_folder);
  IOHprofiler_free_memory(algorithm_name);
  IOHprofiler_free_memory(algorithm_info);
  IOHprofiler_free_memory(base_evaluation_triggers);
  IOHprofiler_free_memory(parameters_name);
  IOHprofiler_free_memory(parameters_namet);
  /* Here each observer must have an entry - a call to a specific function that sets the additional_option_keys
   * and the following observer fields:
   * - logger_allocate_function
   * - logger_free_function
   * - data_free_function
   * - data */
  if (0 == strcmp(observer_name, "PBO")) {
    observer_IOHprofiler(observer, observer_options, &additional_option_keys);
  }else {
    IOHprofiler_warning("Unknown observer!");
    return NULL;
  }

  /* Check for redundant option keys */
  known_option_keys = IOHprofiler_option_keys_allocate(sizeof(known_keys) / sizeof(char *), known_keys);
  IOHprofiler_option_keys_add(&known_option_keys, additional_option_keys);
  given_option_keys = IOHprofiler_option_keys(observer_options);

  if (given_option_keys) {
    redundant_option_keys = IOHprofiler_option_keys_get_redundant(known_option_keys, given_option_keys);

    /*if ((redundant_option_keys != NULL) && (redundant_option_keys->count > 0)) {
      char *output_redundant = IOHprofiler_option_keys_get_output_string(redundant_option_keys,
          "IOHprofiler_observer(): Some keys in observer options were ignored:\n");
      char *output_valid = IOHprofiler_option_keys_get_output_string(known_option_keys,
          "Valid keys for observer options are:\n");
      IOHprofiler_warning("%s%s", output_redundant, output_valid);
      IOHprofiler_free_memory(output_redundant);
      IOHprofiler_free_memory(output_valid);
    }*/

    IOHprofiler_option_keys_free(given_option_keys);
    IOHprofiler_option_keys_free(redundant_option_keys);
  }
  IOHprofiler_option_keys_free(known_option_keys);
  IOHprofiler_option_keys_free(additional_option_keys);
  return observer;
}

/**
 * Wraps the observer's logger around the problem if the observer is not NULL and invokes the initialization
 * of this logger.
 *
 * @param problem The given IOHprofiler problem.
 * @param observer The IOHprofiler observer, whose logger will wrap the problem.
 *
 * @returns The observed problem in the form of a new IOHprofiler problem instance or the same problem if the
 * observer is NULL.
 */
IOHprofiler_problem_t *IOHprofiler_problem_add_observer(IOHprofiler_problem_t *problem, IOHprofiler_observer_t *observer) {

  if (problem == NULL)
    return NULL;

  if ((observer == NULL) || (observer->is_active == 0)) {
    IOHprofiler_warning("The problem will not be observed. %s",
        observer == NULL ? "(observer == NULL)" : "(observer not active)");
    return problem;
  }

  assert(observer->logger_allocate_function);
  return observer->logger_allocate_function(observer, problem);
}

/**
 * Frees the observer's logger and returns the inner problem.
 *
 * @param problem The observed IOHprofiler problem.
 * @param observer The IOHprofiler observer, whose logger was wrapping the problem.
 *
 * @returns The unobserved problem as a pointer to the inner problem or the same problem if the problem
 * was not observed.
 */
IOHprofiler_problem_t *IOHprofiler_problem_remove_observer(IOHprofiler_problem_t *problem, IOHprofiler_observer_t *observer) {

  IOHprofiler_problem_t *problem_unobserved;
  char *prefix;

  if ((observer == NULL) || (observer->is_active == 0)) {
    IOHprofiler_warning("The problem was not observed. %s",
        observer == NULL ? "(observer == NULL)" : "(observer not active)");
    return problem;
  }

  /* Check that we are removing the observer that is actually wrapping the problem.
   *
   * This is a hack - it assumes that the name of the problem is formatted as "observer_name(problem_name)".
   * While not elegant, it does the job and is better than nothing. */
  prefix = IOHprofiler_remove_from_string(problem->problem_name, "(", "");
  if (strcmp(prefix, observer->observer_name) != 0) {
    IOHprofiler_error("IOHprofiler_problem_remove_observer(): trying to remove observer %s instead of %s",
        observer->observer_name, prefix);
  }
  IOHprofiler_free_memory(prefix);

  /* Keep the inner problem and remove the logger data */
  problem_unobserved = IOHprofiler_problem_transformed_get_inner_problem(problem);
  IOHprofiler_problem_transformed_free_data(problem);

  problem = NULL;

  return problem_unobserved;
}
#line 1 "code-experiments/src/IOHprofiler_runtime_c.c"
/**
 * @file IOHprofiler_runtime_c.c
 * @brief Generic IOHprofiler runtime implementation for the C language.
 *
 * Other language interfaces might want to replace this so that memory allocation and error handling go
 * through the respective language runtime.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#line 14 "code-experiments/src/IOHprofiler_runtime_c.c"
#line 15 "code-experiments/src/IOHprofiler_runtime_c.c"

void IOHprofiler_error(const char *message, ...) {
    va_list args;

    fprintf(stderr, "IOHprofiler FATAL ERROR: ");
    va_start(args, message);
    vfprintf(stderr, message, args);
    va_end(args);
    fprintf(stderr, "\n");
    return;
}

void IOHprofiler_warning(const char *message, ...) {
    va_list args;

    if (IOHprofiler_log_level >= IOHprofiler_WARNING) {
        fprintf(stderr, "IOHprofiler WARNING: ");
        va_start(args, message);
        vfprintf(stderr, message, args);
        va_end(args);
        fprintf(stderr, "\n");
    }
}

void IOHprofiler_info(const char *message, ...) {
    va_list args;

    if (IOHprofiler_log_level >= IOHprofiler_INFO) {
        fprintf(stdout, "IOHprofiler INFO: ");
        va_start(args, message);
        vfprintf(stdout, message, args);
        va_end(args);
        fprintf(stdout, "\n");
        fflush(stdout);
    }
}

/**
 * A function similar to IOHprofiler_info that prints only the given message without any prefix and without
 * adding a new line.
 */
void IOHprofiler_info_partial(const char *message, ...) {
    va_list args;

    if (IOHprofiler_log_level >= IOHprofiler_INFO) {
        va_start(args, message);
        vfprintf(stdout, message, args);
        va_end(args);
        fflush(stdout);
    }
}

void IOHprofiler_debug(const char *message, ...) {
    va_list args;

    if (IOHprofiler_log_level >= IOHprofiler_DEBUG) {
        fprintf(stdout, "IOHprofiler DEBUG: ");
        va_start(args, message);
        vfprintf(stdout, message, args);
        va_end(args);
        fprintf(stdout, "\n");
        fflush(stdout);
    }
}

void *IOHprofiler_allocate_memory(const size_t size) {
    void *data;
    if (size == 0) {
        IOHprofiler_error("IOHprofiler_allocate_memory() called with 0 size.");
        return NULL; /* never reached */
    }
    data = malloc(size);
    if (data == NULL)
        IOHprofiler_error("IOHprofiler_allocate_memory() failed.");
    return data;
}

void IOHprofiler_free_memory(void *data) {
    free(data);
}
