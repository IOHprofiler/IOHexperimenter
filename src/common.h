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

//The maximum length for path of IOHprofiler files.
#define IOHprofiler_PATH_MAX 200

void IOH_error(std::string error_info);

void IOH_warning(std::string warning_info);

void IOH_log(std::string log_info);

void IOH_log(std::string log_info, std::ofstream &log_stream);


template<class valueType>
void copyVector(const std::vector<valueType> v1, std::vector<valueType> &v2){
  v2.assign(v1.begin(),v1.end());
};

// Reture 'true' if all elements in two vectors are the same.
template<class valueType>
bool compareVector(std::vector<valueType> &v1, std::vector<valueType> v2){
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

// Return true if values of vl's elements are larger than v2's in each index.
// This is used to compare to objectives vector, details needs to be discussed
// for multi-objective optimization.
template<class valueType>
bool compareObjectives(std::vector<valueType> &v1, std::vector<valueType> v2){
  int n = v1.size();
  if(n != v2.size()){
    IOH_error("Two compared objective vector must be with the same size\n");
    return false;
  }
  for (int i = 0; i != n; ++i)
  {
    if(v1[i] <= v2[i]){
      return false;
    }
  }
  return true;
};

#endif //_IOHPROFILER_COMMON_H