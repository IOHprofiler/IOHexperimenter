#ifndef _IOHPROFILER_SUITE_HPP
#define _IOHPROFILER_SUITE_HPP

#include "Problems/f_one_max.hpp"
#include "Problems/f_leading_ones.hpp"
#include "Problems/f_linear.hpp"
#include "Problems/f_one_max_dummy1.hpp"
#include "Problems/f_one_max_dummy2.hpp"
#include "Problems/f_one_max_neutrality.hpp"
#include "Problems/f_one_max_epistasis.hpp"
#include "Problems/f_one_max_ruggedness1.hpp"
#include "Problems/f_one_max_ruggedness2.hpp"
#include "Problems/f_one_max_ruggedness3.hpp"
#include "Problems/f_leading_ones_dummy1.hpp"
#include "Problems/f_leading_ones_dummy2.hpp"
#include "Problems/f_leading_ones_neutrality.hpp"
#include "Problems/f_leading_ones_epistasis.hpp"
#include "Problems/f_leading_ones_ruggedness1.hpp"
#include "Problems/f_leading_ones_ruggedness2.hpp"
#include "Problems/f_leading_ones_ruggedness3.hpp"
#include "Problems/f_labs.hpp"
#include "Problems/f_ising_1D.hpp"
#include "Problems/f_ising_2D.hpp"
#include "Problems/f_ising_triangle.hpp"
#include "Problems/f_MIS.hpp"
#include "Problems/f_N_queens.hpp"
#include "IOHprofiler_problem.hpp"
#include "common.h"
#include "IOHprofiler_csv_logger.hpp"

template <class InputType> class IOHprofiler_suite {
public:

  IOHprofiler_suite() {
  };

  IOHprofiler_suite(std::vector<int> problem_id, std::vector<int> instance_id, std::vector<int> dimension){};

  void init_problem_set(){
    size = number_of_problems * number_of_instances * number_of_dimensions;
    for(int i = 0; i != number_of_problems; ++i)
      for (int j = 0; j != number_of_instances; ++j)
        for (int h = 0; h != number_of_dimensions; ++h)
          add_problem(problem_id[i],instance_id[j],dimension[h]);
  };

  void virtual add_problem(int problem_id, int instance_id, int dimension){
  };

  // The function to acquire problems of the suite one by one until NULL returns.
  std::shared_ptr<IOHprofiler_problem<InputType>> get_next_problem(int problem_index) {
    current_problem =  problem_list[problem_index];
    current_problem->addCSVLogger(this->csv_logger);
    return current_problem;
  };



  // Add a csvLogger for the suite. 
  // The logger of the suite will only control .info files.
  // To output files for evaluation of problems, this logger needs to be added to problem_list.
  void addCSVLogger(IOHprofiler_csv_logger &logger) {
    this->csv_logger = logger;
    this->csv_logger.target_suite(this->suite_name);
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

  std::vector<IOHprofiler_problem<int> > get_problems(){
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

  std::vector< std::shared_ptr< IOHprofiler_problem<InputType> > > problem_list;

  int get_size(){
    return this->size;
  };

private:
  int size = 0;

  std::string suite_name = "default";
  int number_of_problems;
  int number_of_instances;
  int number_of_dimensions;

  std::vector<int> problem_id;
  std::vector<int> instance_id;
  std::vector<int> dimension;

  // An common interface of the problem to be tested.
  std::shared_ptr< IOHprofiler_problem<InputType> > current_problem = NULL;

  IOHprofiler_csv_logger csv_logger;
};

#endif