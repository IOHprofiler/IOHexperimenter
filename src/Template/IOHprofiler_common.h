#ifndef _IOHPROFILER_COMMON_H
#define _IOHPROFILER_COMMON_H

#include "IOHprofiler_platform.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>
#include <math.h>
#include <cstdlib> 
#include <cstddef>
#include <climits>
#include <cfloat>
#include <algorithm>
#include <memory>
#include <map>

/// < The maximum length for path of IOHprofiler files.
#define IOHprofiler_PATH_MAX 200

/// < The maximum allowed testing dimension.
#define IOHprofiler_MAX_DIMENSION 20000

/// < Default problem id as not being assigned to a suite.
#define DEFAULT_PROBLEM_ID 0

/// < Default instance_id
#define DEFAULT_INSTANCE 1

/// < Default dimension
#define DEFAULT_DIMENSION 4

void IOH_error(std::string error_info);

void IOH_warning(std::string warning_info);

void IOH_log(std::string log_info);

void IOH_log(std::string log_info, std::ofstream &log_stream);


template<class valueType>
void copyVector(const std::vector<valueType> v1, std::vector<valueType> &v2){
  v2.assign(v1.begin(),v1.end());
};

/// bool compareVector(std::vector<valueType> &v1, std::vector<valueType> v2)
///
/// Return 'true' if all elements in two vectors are the same.
template<class valueType>
bool compareVector(const std::vector<valueType> &v1, const std::vector<valueType> &v2){
  int n = v1.size();
  if(n != v2.size()){
    IOH_error("Two compared vector must be with the same size\n");
    return false;
  }
  for (int i = 0; i != n; ++i)
  {
    if(v1[i] != v2[i]){
      return false;
    }
  }
  return true;
};

/// \fn bool compareObjectives(std::vector<valueType> &v1, std::vector<valueType> v2,  const int optimization_type)
///
/// Return true if values of vl's elements are better than v2's in each index.
/// set as maximization if optimization_type = 1, otherwise minimization.
/// This is used to compare to objectives vector, details needs to be discussed
/// for multi-objective optimization.
template<class valueType>
bool compareObjectives(const std::vector<valueType> &v1, const std::vector<valueType> &v2, const int optimization_type){
  int n = v1.size();
  if(n != v2.size()){
    IOH_error("Two compared objective vector must be with the same size\n");
    return false;
  }
  if (optimization_type == 1) {
    for (int i = 0; i != n; ++i)
    {
      if (v1[i] <= v2[i]) {
        return false;
      }
    }
    return true;
  } else {
    for (int i = 0; i != n; ++i)
    {
      if (v1[i] >= v2[i]) {
        return false;
      }
    }
    return true;
  }
};


/// \fn bool compareObjectives(std::vector<valueType> &v1, std::vector<valueType> v2,  const int optimization_type)
///
/// Return true if values of vl's elements are better than v2's in each index.
/// set as maximization if optimization_type = 1, otherwise minimization.
/// This is used to compare to objectives vector, details needs to be discussed
/// for multi-objective optimization.
template<class valueType>
bool compareObjectives(const valueType v1, const valueType v2, const int optimization_type){
  if (optimization_type == 1) {
    if (v1 <= v2){
      return false;
    }
    return true;
  } else {
    if (v1 >= v2) {
      return false;
    }
    return true;
  }
};

#endif //_IOHPROFILER_COMMON_H
