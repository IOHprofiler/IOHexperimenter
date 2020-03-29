/// \file IOHprofiler_suite.hpp
/// \brief Hpp file for the template class IOHprofiler_suite.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _IOHPROFILER_SUITE_HPP
#define _IOHPROFILER_SUITE_HPP

#include "IOHprofiler_class_generator.hpp"
#include "IOHprofiler_problem.hpp"

typedef std::map<std::string, int> PROBLEM_NAME_ID; 
typedef std::map<int, std::string> PROBLEM_ID_NAME; 

/// \brief A base class for construct a suite for sets of IOHprofiler_problems.
///
/// To specify available problems of a suite, registerProblem must be implemented in derived class.
/// The default lable of problems are string type. Integer type are also optional, but we highly
/// recommond registering problem with string lable and creating a map of string problem_name and integer problem_id.
template <class InputType> class IOHprofiler_suite : public IOHprofiler_problem<InputType>, public std::vector<std::shared_ptr<IOHprofiler_problem<InputType> > > {
public:
  IOHprofiler_suite(std::vector<int> problem_id = std::vector<int>(0), std::vector<int> instance_id = std::vector<int>(0), std::vector<int> dimension = std::vector<int>(0)) : 
    suite_name("no suite"),
    problem_list_index(0),
    get_problem_flag(false),
    current_problem(nullptr) {}
  ~IOHprofiler_suite() {}

  IOHprofiler_suite(const IOHprofiler_suite&) = delete;
  IOHprofiler_suite &operator =(const IOHprofiler_suite&) = delete;

  typedef std::shared_ptr<IOHprofiler_problem<InputType> > Problem_ptr;
  
  /// \fn virtual void registerProblem()
  /// \brief A virtual function for registering problems.
  ///
  /// This function implements interfaces of available problems of a suite. With those interface,
  /// user are able to request problem together with problem_id, instance_id, and dimension.
  virtual void registerInSuite() {}

  /// \fn loadProblems()
  /// \brief Allocating memeory and creating instances of problems to be included in the suite.
  ///
  /// Before acquiring a problem from the suite, this function must be invoked.
  /// Otherwise the list of problem is empty.
  void loadProblem();

  /// \fn std::shared_ptr<IOHprofiler_problem<InputType>> get_next_problem()
  /// \brief An interface of requesting problems in suite.
  ///
  /// To request 'the next' problem in the suite of correponding problem_id, instance_id and dimension index.
  Problem_ptr get_next_problem();
  
  /// \fn std::shared_ptr<IOHprofiler_problem<InputType>> get_current_problem()
  /// \brief An interface of requesting problems in suite.
  ///
  /// To request 'the current' problem in the suite of correponding problem_id, instance_id and dimension index.
  Problem_ptr get_current_problem();
  

  /// \fn Problem_ptr get_next_problem()
  /// \brief An interface of requesting problems in suite.
  ///
  /// To request a specific problem with corresponding problem_id, instance_id and dimension,
  /// without concerning the order of testing problems.
  Problem_ptr get_problem(std::string problem_name, int instance, int dimension);

  /// \fn Problem_ptr get_next_problem()
  /// \brief An interface of requesting problems in suite.
  ///
  /// To request a specific problem with corresponding problem_id, instance_id and dimension,
  /// without concerning the order of testing problems.
  Problem_ptr get_problem(int problem_id, int instance, int dimension);

  int IOHprofiler_suite_get_number_of_problems() const;

  int IOHprofiler_suite_get_number_of_instances() const;

  int IOHprofiler_suite_get_number_of_dimensions() const;

  std::vector<int> IOHprofiler_suite_get_problem_id() const;

  std::map<int, std::string> IOHprofiler_suite_get_problem_name() const;

  //! Return a map of problem name to problem ID, for all the registered problem.
  std::map<std::string, int> IOHprofiler_suite_get_problems() const;

  std::vector<int> IOHprofiler_suite_get_instance_id() const;

  std::vector<int> IOHprofiler_suite_get_dimension() const;

  std::string IOHprofiler_suite_get_suite_name() const;

  void IOHprofiler_set_suite_problem_id(const std::vector<int> &problem_id);

  void IOHprofiler_set_suite_instance_id(const std::vector<int> &instance_id);

  void IOHprofiler_set_suite_dimension(const std::vector<int> &dimension);

  void IOHprofiler_set_suite_name(const std::string suite_name);

  void mapIDTOName(const int id, const std::string name);

private:
  std::string suite_name;
  int number_of_problems;
  int number_of_instances;
  int number_of_dimensions;

  std::vector<int> problem_id;
  std::vector<int> instance_id;
  std::vector<int> dimension;

  PROBLEM_ID_NAME problem_id_name_map;
  PROBLEM_NAME_ID problem_name_id_map;

  size_t problem_list_index;
  size_t size_of_problem_list;
  bool get_problem_flag;

  Problem_ptr current_problem;
};

template <class InputType> void IOHprofiler_suite<InputType>::loadProblem() {
  if (this->size() != 0) {
    this->clear();
  }
  this->size_of_problem_list = this->number_of_dimensions * this->number_of_instances * this->number_of_problems;
  this->problem_list_index = 0;

  for (int i = 0; i != this->number_of_problems; ++i) {
    for (int j = 0; j != this->number_of_dimensions; ++j) {
      for (int h = 0; h != this->number_of_instances; ++h) {
        Problem_ptr p = get_problem(this->problem_id_name_map[this->problem_id[i]],
                                      this->instance_id[h],
                                      this->dimension[j]);
        this->push_back(p);
      }
    }
  }
  this->get_problem_flag = false;
}

template <class InputType> std::shared_ptr<IOHprofiler_problem<InputType> > IOHprofiler_suite<InputType>::get_next_problem() {
  if (this->size_of_problem_list == 0) {
    IOH_warning("There is no problem in the suite");
    return nullptr;
  }

  if (this->problem_list_index == this->size_of_problem_list - 1 && this->get_problem_flag == true) {
    return nullptr;
  }

  if (this->get_problem_flag == false) {
    this->get_problem_flag = true;
  } else {
    this->problem_list_index++;
  }    
  this->current_problem = (*this)[problem_list_index];
  
  this->current_problem->reset_problem();
  return this->current_problem;
}

template <class InputType> std::shared_ptr<IOHprofiler_problem<InputType> > IOHprofiler_suite<InputType>::get_current_problem() {
  if (this->get_problem_flag == false) {
    this->get_problem_flag = true;
  }
  this->current_problem = (*this)[this->problem_list_index];
  this->current_problem->reset_problem();
  return this->current_problem;
}

template <class InputType> std::shared_ptr<IOHprofiler_problem<InputType> > IOHprofiler_suite<InputType>::get_problem(std::string problem_name, int instance, int dimension) {
  Problem_ptr p = genericGenerator<IOHprofiler_problem<InputType> >::instance().create(problem_name);
  p->reset_problem();
  p->IOHprofiler_set_problem_id(this->problem_name_id_map[problem_name]);
  p->IOHprofiler_set_instance_id(instance);
  p->IOHprofiler_set_number_of_variables(dimension);
  return p;
}

template <class InputType> std::shared_ptr<IOHprofiler_problem<InputType> > IOHprofiler_suite<InputType>::get_problem(int problem_id, int instance, int dimension) {
  Problem_ptr p = genericGenerator<IOHprofiler_problem<InputType> >::instance().create(this->problem_id_name_map[problem_id]);
  p->reset_problem();
  p->IOHprofiler_set_problem_id(problem_id);
  p->IOHprofiler_set_instance_id(instance);
  p->IOHprofiler_set_number_of_variables(dimension);
  return p;
}

template <class InputType> int IOHprofiler_suite<InputType>::IOHprofiler_suite_get_number_of_problems() const {
  return this->number_of_problems;
}

template <class InputType> int IOHprofiler_suite<InputType>::IOHprofiler_suite_get_number_of_instances() const {
  return this->number_of_instances;
}

template <class InputType> int IOHprofiler_suite<InputType>::IOHprofiler_suite_get_number_of_dimensions() const {
  return this->number_of_dimensions;
}

template <class InputType> std::vector<int> IOHprofiler_suite<InputType>::IOHprofiler_suite_get_problem_id() const {
  return this->problem_id;
}

template <class InputType> std::map<int, std::string> IOHprofiler_suite<InputType>::IOHprofiler_suite_get_problem_name() const {
  return this->problem_id_name_map;
}

template <class InputType> std::map<std::string, int> IOHprofiler_suite<InputType>::IOHprofiler_suite_get_problems() const {
  return this->problem_name_id_map;
}

template <class InputType> std::vector<int> IOHprofiler_suite<InputType>::IOHprofiler_suite_get_instance_id() const {
  return this->instance_id;
}

template <class InputType> std::vector<int> IOHprofiler_suite<InputType>::IOHprofiler_suite_get_dimension() const {
  return this->dimension;
}

template <class InputType> std::string IOHprofiler_suite<InputType>::IOHprofiler_suite_get_suite_name() const {
  return this->suite_name;
}

template <class InputType> void IOHprofiler_suite<InputType>::IOHprofiler_set_suite_problem_id(const std::vector<int> &problem_id) {
  copyVector(problem_id,this->problem_id);
  this->number_of_problems = this->problem_id.size();
}

template <class InputType> void IOHprofiler_suite<InputType>::IOHprofiler_set_suite_instance_id(const std::vector<int> &instance_id) {
  copyVector(instance_id,this->instance_id);
  this->number_of_instances = this->instance_id.size();
}

template <class InputType> void IOHprofiler_suite<InputType>::IOHprofiler_set_suite_dimension(const std::vector<int> &dimension) {
  copyVector(dimension,this->dimension);
  this->number_of_dimensions = this->dimension.size();
}

template <class InputType> void IOHprofiler_suite<InputType>::IOHprofiler_set_suite_name(const std::string suite_name) {
  this->suite_name = suite_name;
}

template <class InputType> void IOHprofiler_suite<InputType>::mapIDTOName(const int id, const std::string name){
  problem_id_name_map[id] = name;
  problem_name_id_map[name] = id;
}

#endif //_IOHPROFILER_SUITE_HPP
