/// \file IOHprofiler_observer.hpp
/// \brief Head file for class IOHprofiler_observer.
///
/// A detailed file description.
///
/// \author Furong Ye, Ofer M. Sher
/// \date 2019-06-27
#ifndef _IOHPROFILER_PROBLEM_GENERATOR_HPP
#define _IOHPROFILER_PROBLEM_GENERATOR_HPP

#include "IOHprofiler_common.h"

using defaultIDKeyType = std::string;

template <class manufacturedObj> class genericGenerator {
  
  using BASE_CREATE_FN = std::shared_ptr<manufacturedObj> (*)();
  
  /// FN_REGISTRY is the registry of all the BASE_CREATE_FN
  /// pointers registered.  Functions are registered using the
  /// regCreateFn member function (see below).
  using FN_registry = std::map<std::string, BASE_CREATE_FN>;
  FN_registry registry;

  genericGenerator();
  genericGenerator(const genericGenerator&) = delete; 
  genericGenerator &operator=(const genericGenerator&) = delete;

public:
  /// Singleton access.
  static genericGenerator &instance();
  
  /// Classes derived from manufacturedObj call this function once
  /// per program to register the class ID key, and a pointer to
  /// the function that creates the class.
  void regCreateFn(std::string, BASE_CREATE_FN);
  
  /// Create a new class of the type specified by className.
  std::shared_ptr<manufacturedObj> create(std::string className) const;
};

template <class manufacturedObj> genericGenerator<manufacturedObj>::genericGenerator() {
}

template <class manufacturedObj> genericGenerator<manufacturedObj> &genericGenerator<manufacturedObj>::instance() {
    /// Note that this is not thread-safe!
    static genericGenerator theInstance;
    return theInstance;
}

/// Register the creation function.  
/// This simply associates the classIDKey with the function used to create the class.  
/// The return value is a dummy value, which is used to allow static initialization of the registry.
template <class manufacturedObj> void genericGenerator<manufacturedObj>::regCreateFn(std::string clName, BASE_CREATE_FN func) {
  registry[clName]=func;
}

/// The create function simple looks up the class ID, and if it's in the list, the statement "(*i).second();" calls the function.

template <class manufacturedObj> std::shared_ptr<manufacturedObj> genericGenerator<manufacturedObj>::create(std::string className) const {
  std::shared_ptr<manufacturedObj> ret(nullptr);

  typename FN_registry::const_iterator regEntry = registry.find(className);
  if (regEntry != registry.end()) {
    return (*regEntry).second();
  }
  return ret;
};

/// Helper template to make registration simple.
template <class ancestorType  ,
          class manufacturedObj,
          typename classIDKey=defaultIDKeyType>
class registerInFactory {
public:
    static std::shared_ptr<ancestorType> createInstance() {
        return std::shared_ptr<ancestorType>(manufacturedObj::createInstance());
    }
    registerInFactory(const classIDKey id) {
      genericGenerator<ancestorType>::instance().regCreateFn(id, createInstance);
    }
};

#endif