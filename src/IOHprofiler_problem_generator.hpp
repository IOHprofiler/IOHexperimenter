/// \file IOHprofiler_observer.hpp
/// \brief Head file for class IOHprofiler_observer.
///
/// A detailed file description.
///
/// \author Furong Ye, Ofer M. Sher
/// \date 2019-06-27
#ifndef _IOHPROFILER_PROBLEM_GENERATOR_HPP
#define _IOHPROFILER_PROBLEM_GENERATOR_HPP

#include "common.h"

using defaultIDKeyType = std::string;

template <class problemObj> class genericGenerator {
  
  using BASE_CREATE_FN = std::shared_ptr<problemObj> (*)();
  
  using FN_REGISTRY_INT = std::map<int, BASE_CREATE_FN>;
  using FN_REGISTRY_STRING = std::map<std::string, BASE_CREATE_FN>;
  FN_REGISTRY_INT registry_int;
  FN_REGISTRY_STRING registry_string;

  genericGenerator();
  genericGenerator(const genericGenerator&) = delete; 
  genericGenerator &operator=(const genericGenerator&) = delete;

public:
  static genericGenerator &instance();
  
  void regCreateFn(std::string, BASE_CREATE_FN);
  void regCreateFn(int, BASE_CREATE_FN);
  
  std::shared_ptr<problemObj> create(int  className) const;
  std::shared_ptr<problemObj> create(std::string className) const;
};

template <class problemObj> genericGenerator<problemObj>::genericGenerator() {
}

template <class problemObj> genericGenerator<problemObj> &genericGenerator<problemObj>::instance() {
    // Note that this is not thread-safe!
    static genericGenerator theInstance;
    return theInstance;
}

template <class problemObj> void genericGenerator<problemObj>::regCreateFn(std::string clName, BASE_CREATE_FN func) {
  registry_string[clName]=func;
}

template <class problemObj> void genericGenerator<problemObj>::regCreateFn(int clName, BASE_CREATE_FN func) {
  registry_int[clName]=func;
}


template <class problemObj> std::shared_ptr<problemObj> genericGenerator<problemObj>::create(int className) const {
  std::shared_ptr<problemObj> ret(nullptr);

  typename FN_REGISTRY_INT::const_iterator regEntry = registry_int.find(className);
  if (regEntry != registry_int.end()) {
    return (*regEntry).second();
  }
 
  return ret;
};

template <class problemObj> std::shared_ptr<problemObj> genericGenerator<problemObj>::create(std::string className) const {
  std::shared_ptr<problemObj> ret(nullptr);

  typename FN_REGISTRY_STRING::const_iterator regEntry = registry_string.find(className);
  if (regEntry != registry_string.end()) {
    return (*regEntry).second();
  }
  return ret;
};

template <class ancestorType  ,
          class problemObj,
          typename classIDKey=defaultIDKeyType>
class registerInFactory {
public:
    static std::shared_ptr<ancestorType> createInstance() {
        return std::shared_ptr<ancestorType>(problemObj::createInstance());
    }
    registerInFactory(const classIDKey id) {
      genericGenerator<ancestorType>::instance().regCreateFn(id, createInstance);
    }
};

#endif