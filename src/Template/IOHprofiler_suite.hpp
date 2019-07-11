/// \file IOHprofiler_suite.hpp
/// \brief Head file for class IOHprofiler_suite.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _IOHPROFILER_SUITE_HPP
#define _IOHPROFILER_SUITE_HPP

#include "IOHprofiler_class_generator.hpp"
#include "IOHprofiler_problem.hpp"

using PROBLEM_NAME_ID =  std::map<std::string, int>; 
using PROBLEM_ID_NAME =  std::map<int, std::string>; 

/// \brief A base class for construct a suite for sets of IOHprofiler_problems.
///
/// To specify available problems of a suite, registerProblem must be implemented in derived class.
/// The default lable of problems are string type. Integer type are also optional, but we highly
/// recommond registering problem with string lable and creating a map of string problem_name and integer problem_id.
template <class InputType> class IOHprofiler_suite {
public:

  IOHprofiler_suite() {};
  IOHprofiler_suite(std::vector<int> problem_id, std::vector<int> instance_id, std::vector<int> dimension) {};
  ~IOHprofiler_suite() {};

  IOHprofiler_suite(const IOHprofiler_suite&) = delete;
  IOHprofiler_suite &operator =(const IOHprofiler_suite&) = delete;

  /// \fn virtual void registerProblem()
  /// \brief A virtual function for registering problems.
  ///
  /// This function implements interfaces of available problems of a suite. With those interface,
  /// user are able to request problem together with problem_id, instance_id, and dimension.
  virtual void registerInSuite() {};

  /// \fn std::shared_ptr<IOHprofiler_problem<InputType>> get_next_problem()
  /// \brief An interface of requesting problems in suite.
  ///
  /// To request 'the next' problem in the suite of correponding problem_id, instance_id and dimension index.
  std::shared_ptr<IOHprofiler_problem<InputType>> get_next_problem() {
    if(this->problem_index == this->number_of_problems) {
          return nullptr;
    }

    this->current_problem = get_problem(this->problem_id_name_map[this->problem_id[this->problem_index]],
                                        this->instance_id[this->instance_index],
                                        this->dimension[this->dimension_index]);
    if (this->csv_logger) {
      this->current_problem->addCSVLogger(this->csv_logger);
    }

    this->instance_index++;
    if (this->instance_index == this->number_of_instances) {
      this->instance_index = 0;
      this->dimension_index++;
      if (this->dimension_index == this->number_of_dimensions) {
        this->dimension_index = 0;
        this->problem_index++;
      }
    }
    return this->current_problem;
  };
  
  /// \fn std::shared_ptr<IOHprofiler_problem<InputType>> get_current_problem()
  /// \brief An interface of requesting problems in suite.
  ///
  /// To request 'the current' problem in the suite of correponding problem_id, instance_id and dimension index.
  
  std::shared_ptr<IOHprofiler_problem<InputType>> get_current_problem() {
    
    this->instance_index--;
    if(this->instance_index < 0) {
      this->instance_index = this->number_of_instances - 1;
      this->dimension_index--;
      if(this->dimension_index < 0) {
        this->dimension_index = this->number_of_dimensions - 1;
        this->problem_index--;
        if(this->problem_index < 0) {
          this->problem_index = 0;
        }
      }
    }

    this->current_problem = get_problem(this->problem_id_name_map[this->problem_id[this->problem_index]],
                                        this->instance_id[this->instance_index],
                                        this->dimension[this->dimension_index]);
    if (this->csv_logger) {
      this->current_problem->addCSVLogger(this->csv_logger);
    }

    this->instance_index++;
    if (this->instance_index == this->number_of_instances) {
      this->instance_index = 0;
      this->dimension_index++;
      if (this->dimension_index == this->number_of_dimensions) {
        this->dimension_index = 0;
        this->problem_index++;
      }
    }
    return this->current_problem;
  };
  

  /// \fn std::shared_ptr<IOHprofiler_problem<InputType>> get_next_problem()
  /// \brief An interface of requesting problems in suite.
  ///
  /// To request a specific problem with corresponding problem_id, instance_id and dimension,
  /// without concerning the order of testing problems.
  std::shared_ptr<IOHprofiler_problem<InputType>> get_problem(std::string problem_name, int instance, int dimension) {
    std::shared_ptr<IOHprofiler_problem<InputType>> p = genericGenerator<IOHprofiler_problem<InputType>>::instance().create(problem_name);
    p->reset_problem();
    p->IOHprofiler_set_problem_id(this->problem_name_id_map[problem_name]);
    p->IOHprofiler_set_instance_id(instance);
    p->IOHprofiler_set_number_of_variables(dimension);
    return p;
  }


  void addCSVLogger(std::shared_ptr<IOHprofiler_csv_logger> logger) {
    this->csv_logger = logger;
    this->csv_logger->target_suite(this->suite_name);
  };

  int IOHprofiler_suite_get_number_of_problems() {
    return this->number_of_problems;
  };

  int IOHprofiler_suite_get_number_of_instances() {
    return this->number_of_instances;
  };

  int IOHprofiler_suite_get_number_of_dimensions() {
    return this->number_of_dimensions;
  };

  std::vector<int> IOHprofiler_suite_get_problem_id() {
    return this->problem_id;
  };

  std::vector<int> IOHprofiler_suite_get_instance_id() {
    return this->instance_id;
  };

  std::vector<int> IOHprofiler_suite_get_dimension() {
    return this->dimension;
  };

  std::string IOHprofiler_suite_get_suite_name() {
    return this->suite_name;
  };

  void IOHprofiler_set_suite_problem_id(std::vector<int> problem_id) {
    copyVector(problem_id,this->problem_id);
    this->number_of_problems = this->problem_id.size();
  };

  void IOHprofiler_set_suite_instance_id(std::vector<int> instance_id) {
    copyVector(instance_id,this->instance_id);
    this->number_of_instances = this->instance_id.size();
  };

  void IOHprofiler_set_suite_dimension(std::vector<int> dimension) {
    copyVector(dimension,this->dimension);
    this->number_of_dimensions = this->dimension.size();
  };

  void IOHprofiler_set_suite_name(std::string suite_name) {
    this->suite_name = suite_name;
  };

  void mapIDTOName(int id, std::string name){
    problem_id_name_map[id] = name;
    problem_name_id_map[name] = id;
  }

private:

  std::string suite_name = "default";
  int number_of_problems;
  int number_of_instances;
  int number_of_dimensions;

  std::vector<int> problem_id;
  std::vector<int> instance_id;
  std::vector<int> dimension;

  int problem_index = 0;
  int instance_index = 0;
  int dimension_index = 0;

  PROBLEM_ID_NAME problem_id_name_map;
  PROBLEM_NAME_ID problem_name_id_map;

  std::shared_ptr< IOHprofiler_problem<InputType> > current_problem = nullptr;

  std::shared_ptr<IOHprofiler_csv_logger> csv_logger = nullptr;
};

#endif //_IOHPROFILER_SUITE_HPP