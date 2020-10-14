#pragma once

#include <stddef.h>


#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW64__) || defined(__CYGWIN__)
#define NOMINMAX
#include <windows.h>
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

/* Definitions needed for creating and removing directories */
/* Separately handle the special case of Microsoft Visual Studio 2008 with x86_64-w64-mingw32-gcc */
#if _MSC_VER
#include <direct.h>
#elif defined(__MINGW32__) || defined(__MINGW64__)
	#include <dirent.h>
#else
	#include <dirent.h>
#endif

/* Definition of the S_IRWXU constant needed to set file permissions */
#if defined(HAVE_GFA)
	#define S_IRWXU 0700
#endif

/* To silence the Visual Studio compiler (C4996 warnings in the python build). */
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#define IOH_PATH_MAX 200

/// < The maximum allowed testing dimension.
#define MAX_DIMENSION 20000

/// < Default problem id as not being assigned to a suite.
#define DEFAULT_PROBLEM_ID 0

/// < Default instance_id
#define DEFAULT_INSTANCE 1

/// < Default dimension
#define DEFAULT_DIMENSION 4

/// < Max buffer size
#define MAX_BUFFER_SIZE 65534


// Random

/* Use polar transformation method */
#define NORMAL_POLAR 

#define IOH_PI 3.14159265358979323846
#define DEFAULT_SEED 1000


#define RND_MULTIPLIER 16807
#define RND_MODULUS 2147483647
#define RND_MODULUS_DIV 127773
#define RND_MOD_MULTIPLIER 2836

#define SHORT_LAG  273
#define LONG_LAG 607

#define MAXLINESIZE 1024
#define MAXKEYNUMBER 100