#include "../Problems/f_one_max.hpp"
#include "../Problems/f_leading_ones.hpp"
#include "../IOHprofiler_problem.hpp"
#include "../common.h"
// This is a suite consists of pseudo-Boolean problmes. By calling problem from a suite, .info
// files will be generated with log files.

class PBO_suite {
public:
  std::string suite_name = "PBO";
  int number_of_problems;
  int number_of_instances;
  int number_of_dimensions;

  std::vector<int> problem_id;
  std::vector<int> instance_id;
  std::vector<int> dimension;

  int problem_id_index = 0;
  int instance_id_index = 0;
  int dimension_id_index = 0;

  IOHprofiler_problem<int> *current_problem;
  // OneMax om;
  // LeadingOnes lo;

  PBO_suite() {
    number_of_problems = 2;
    number_of_instances = 100;
    number_of_dimensions = 3;
    for (int i = 0; i < number_of_problems; ++i) {
      problem_id.push_back(i+1);
    }
    for (int i = 0; i < number_of_instances; ++i) {
      instance_id.push_back(i+1);
    }
    dimension.push_back(100);
    dimension.push_back(500);
    dimension.push_back(1000);
  };

  PBO_suite(std::vector<int> problem_id, std::vector<int> instance_id, std::vector<int> dimension){


    // Need to check if the input values are valid.
    number_of_problems = problem_id.size();
    number_of_instances = instance_id.size();
    number_of_dimensions = dimension.size();

    copyVector(problem_id,this->problem_id);
    copyVector(instance_id,this->instance_id);
    copyVector(dimension,this->dimension);
  }

  IOHprofiler_problem<int> * get_next_problem() {
    if(problem_id_index == number_of_problems) return NULL;


    // More problems need to be added here
    if(problem_id[problem_id_index] == 1) {
      OneMax om(instance_id[instance_id_index],dimension[dimension_id_index]);
      current_problem = &om; 
    } else if(problem_id[problem_id_index] == 2 ) {
      LeadingOnes lo(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = &lo;
    }
    
    instance_id_index++;
    if(instance_id_index == instance_id.size()){
      instance_id_index= 0; 
      dimension_id_index++;
      if(dimension_id_index == number_of_dimensions) {
        dimension_id_index = 0;
        problem_id_index++;
      }
    }
    return current_problem;
  };

};