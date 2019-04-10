#ifndef _IOHPROFILER_COMMON_H
#define _IOHPROFILER_COMMON_H

#include "IOHprofiler_platform.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <assert.h>
#include <math.h>


//The maximum length for path of IOHprofiler files.
#define IOHprofiler_PATH_MAX 200

template<class valueType>
void copyVector(const std::vector<valueType> v1, std::vector<valueType> &v2){
  v2.assign(v1.begin(),v1.end());
};

template<class valueType>
bool compareVector(std::vector<valueType> &v1, std::vector<valueType> v2){
  int n = v1.size();
  if(n != v2.size()){
    printf("Two compared vector must be with the same size\n");
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

#endif //_IOHPROFILER_COMMON_H