#pragma once

#include <stddef.h>

#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW64__) || defined(__CYGWIN__)
#define NOMINMAX
#include <Windows.h>
#include <io.h>
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

#define IOH_PATH_MAX 200

/// < The maximum allowed testing dimension.
#define IOH_MAX_DIMENSION 20000

/// < Default problem id as not being assigned to a suite.
#define IOH_DEFAULT_PROBLEM_ID 0

/// < Default instance_id
#define IOH_DEFAULT_INSTANCE 1

/// < Default dimension
#define IOH_DEFAULT_DIMENSION 4

/// < Max buffer size
#define IOH_MAX_BUFFER_SIZE 65534


// Random
/* Use polar transformation method */
#define IOH_NORMAL_POLAR 

#define IOH_PI 3.14159265358979323846
#define IOH_DEFAULT_SEED 1000


#define IOH_RND_MULTIPLIER 16807
#define IOH_RND_MODULUS 2147483647
#define IOH_RND_MODULUS_DIV 127773
#define IOH_RND_MOD_MULTIPLIER 2836

#define IOH_SHORT_LAG  273
#define IOH_LONG_LAG 607

#define IOH_MAX_LINESIZE 1024
#define IOH_MAX_KEYNUMBER 100
