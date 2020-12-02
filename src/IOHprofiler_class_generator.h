/// \file IOHprofiler_class_generator.hpp
/// \brief Head file for the classes: 'genericGenerator' and 'registerInFactory'.
///
/// These classes are used for registering problems and suites.
///
/// \author Ofer M. Sher, Furong Ye

#ifndef _IOHPROFILER_PROBLEM_GENERATOR_H
#define _IOHPROFILER_PROBLEM_GENERATOR_H

#include "IOHprofiler_common.h"

typedef std::string defaultIDKeyType;

template <class manufacturedObj> class genericGenerator {
  
  /// typedef std::shared_ptr<manufacturedObj> (*BASE_CREATE_FN)() ;
  
  /// FN_REGISTRY is the registry of all the BASE_CREATE_FN
  /// pointers registered.  Functions are registered using the
  /// regCreateFn member function (see below).
  typedef  std::map<std::string, std::shared_ptr<manufacturedObj> (*)()> FN_registry;
  FN_registry registry;

  genericGenerator();
  genericGenerator(const genericGenerator&) = delete; 
  genericGenerator &operator=(const genericGenerator&) = delete;

public:
  /// Singleton access.
  static genericGenerator &instance();
  
  /// \fn regCreateFn(std::string, std::shared_ptr<manufacturedObj> (*)());
  ///
  /// Classes derived from manufacturedObj call this function once
  /// per program to register the class ID key, and a pointer to
  /// the function that creates the class.
  void regCreateFn(std::string, std::shared_ptr<manufacturedObj> (*)());
  
  /// \fn std::shared_ptr<manufacturedObj> create(std::string className) const;
  ///
  /// Create a new class of the type specified by className.
  /// The create function simple looks up the class ID, and if it's in the list, the statement "(*i).second();" calls the function.
  std::shared_ptr<manufacturedObj> create(std::string className) const;
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
    
    /// \fn registerInFactory(const classIDKey id)
    /// \brief Register the creation function.  
    ///
    /// This simply associates the classIDKey with the function used to create the class.  
    /// The return value is a dummy value, which is used to allow static initialization of the registry.
    registerInFactory(const classIDKey id) {
      genericGenerator<ancestorType>::instance().regCreateFn(id, createInstance);
    }
};

#include "IOHprofiler_class_generator.hpp"

#endif