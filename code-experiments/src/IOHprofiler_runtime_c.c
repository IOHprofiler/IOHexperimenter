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

#include "IOHprofiler.h"
#include "IOHprofiler_utilities.c"

void IOHprofiler_error(const char *message, ...) {
    va_list args;

    fprintf(stderr, "IOHprofiler FATAL ERROR: ");
    va_start(args, message);
    vfprintf(stderr, message, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
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
