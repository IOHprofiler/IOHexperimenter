#pragma once

#include <cstddef>

#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW64__) || defined(__CYGWIN__)
#define NOMINMAX
#include <io.h>
#include <Windows.h>
#elif defined(__gnu_linux__)
	#include <linux/limits.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <unistd.h>
#elif defined(__APPLE__)
	#include <sys/stat.h>
	#include <sys/syslimits.h>
	#include <sys/types.h>
	#include <unistd.h>
#elif defined(__FreeBSD__)
	#include <limits.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <unistd.h>
#elif (defined(__sun) || defined(sun)) && (defined(__SVR4) || defined(__svr4__))
	#include <limits.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <unistd.h>
#else
	#include <sys/stat.h>
	#include <unistd.h>
#endif


/* To silence the Visual Studio compiler (C4996 warnings in the python build). */
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#define IOH_PATH_MAX 200 //NOLINT

/// < The maximum allowed testing dimension.
#define IOH_MAX_DIMENSION 20000 //NOLINT

/// < Default problem id as not being assigned to a suite.
#define IOH_DEFAULT_PROBLEM_ID 0 //NOLINT

/// < Default instance_id
#define IOH_DEFAULT_INSTANCE 1 //NOLINT

/// < Default dimension
#define IOH_DEFAULT_DIMENSION 4 //NOLINT

/// < Max buffer size
#define IOH_MAX_BUFFER_SIZE 65534 //NOLINT


// Random
/* Use polar transformation method */
#define IOH_NORMAL_POLAR 

#define IOH_PI 3.14159265358979323846 //NOLINT
#define IOH_DEFAULT_SEED 1000 //NOLINT


#define IOH_RND_MULTIPLIER 16807 //NOLINT
#define IOH_RND_MODULUS 2147483647 //NOLINT
#define IOH_RND_MODULUS_DIV 127773 //NOLINT
#define IOH_RND_MOD_MULTIPLIER 2836 //NOLINT

#define IOH_SHORT_LAG  273 //NOLINT 
#define IOH_LONG_LAG 607 //NOLINT

#define IOH_MAX_LINE_SIZE 1024 //NOLINT
#define IOH_MAX_KEY_NUMBER 100 //NOLINT
