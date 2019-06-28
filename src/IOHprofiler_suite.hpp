#ifndef _IOHPROFILER_SUITE_HPP
#define _IOHPROFILER_SUITE_HPP

#include "IOHprofiler_problem_generator.hpp"
#include "IOHprofiler_problem.hpp"

using PROBLEM_NAME_ID =  std::map<std::string, int>; 
using PROBLEM_ID_NAME =  std::map<int, std::string>; 

template <class InputType> class IOHprofiler_suite {
public:

  IOHprofiler_suite() {
  };

  IOHprofiler_suite(std::vector<int> problem_id, std::vector<int> instance_id, std::vector<int> dimension){};

  // The function to acquire problems of the suite one by one until NULL returns.
  std::shared_ptr<IOHprofiler_problem<InputType>> get_next_problem() {
    instance_index++;
    if(instance_index == number_of_instances) {
      instance_index = 0;
      dimension_index++;
      if(dimension_index == number_of_dimensions) {
        dimension_index = 0;
        problem_index++;
        if(problem_index == number_of_problems) return nullptr;
      }
    }
    current_problem = get_problem(problem_id_name_map[problem_id[problem_index]],instance_id[instance_index],dimension[dimension_index]);
    if(this->csv_logger) current_problem->addCSVLogger(this->csv_logger);
    return current_problem;
  };

  virtual void registerProblem() {};

  std::shared_ptr<IOHprofiler_problem<InputType>> get_problem(std::string problem_name, int instance, int dimension) {
    std::shared_ptr<IOHprofiler_problem<InputType>> p = genericGenerator<IOHprofiler_problem<InputType>>::instance().create(problem_name);
    p->IOHprofiler_set_problem_id(problem_name_id_map[problem_name]);
    p->IOHprofiler_set_instance_id(instance);
    p->IOHprofiler_set_number_of_variables(dimension);
    //p->reset_problem();
    if(p->IOHprofiler_get_optimal().size() == 0) 
        std::cout << "asafdsa";
    return p;
  }

  // Add a csvLogger for the suite. 
  // The logger of the suite will only control .info files.
  // To output files for evaluation of problems, this logger needs to be added to problem_list.
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

  std::vector< std::shared_ptr< IOHprofiler_problem<InputType> > > get_problems(){
    return this->problem_list;
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

  // An common interface of the problem to be tested.
  std::shared_ptr< IOHprofiler_problem<InputType> > current_problem = nullptr;

  std::shared_ptr<IOHprofiler_csv_logger> csv_logger = nullptr;
};

#endif