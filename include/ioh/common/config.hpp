#pragma once

#include <cstddef>

#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW64__) || defined(__CYGWIN__)
#define NOMINMAX
#include <io.h>

// #include <Windows.h>
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

#ifndef PROJECT_VER
#define PROJECT_VER "0.0.0"
#endif // !1


