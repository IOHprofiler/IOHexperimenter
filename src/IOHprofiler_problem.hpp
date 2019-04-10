#ifndef _IOHPROFILER_PROBLEM_HPP
#define _IOHPROFILER_PROBLEM_HPP


#include "common.h"
#include "IOHprofiler_transformation.hpp"
#include "IOHprofiler_logger.h"

// void(*EVALUATE_METHOD) (std::vector<int>,std::vector<double>);
//typedef void(*TRANSFER_VARIABLES_METHOD) (std::vector<int>, std::vector<int>)
//typedef void(*TRANSFER_OBJECTIVES_METHOD) (std::vector<double>, std::vector<double>)
//typedef void(*CONSTRAINT_METHOD) (std::vector<int>,std::vector<double>);


// Basic structure for IOHExperimentor, which is used for generating benchmark problems.
// To define a new problem, values of 'number_of_variables', 'lowerbound', 'upperbound',
// 'best_variables', 'optimal' must be given. An corresponding 'internal_evaluate' method
// must be defined as well.
// Example:
//     void Initilize_problem(int  dimension) {
//       number_of_variables = dimension;
//       lowerbound.reserve(dimension);
//       upperbound.reserve(dimension);
//       best_variables.reserve(dimension);
//       optimal.reserve(number_of_objectives);
//
//       for(int i = 0; i != dimension; ++i) {
//         lowerbound[i] = 0;
//         upperbound[i] = 1;
//         best_variables[i] = 1;
//       }
//       optimal.push_back((double)dimension);
//    };
//
//    void internal_evaluate(std::vector<int> x,std::vector<double> &y) {
//      y.clear();
//      y.reserve(number_of_variables);
//      int n = x.size();
//      int result = 0;
//      for(int i = 0; i != n; ++i) {
//        result += x[i];
//      }
//      y.push_back((double)result);
//    };

template <class InputType> class IOHprofiler_problem : public IOHprofiler_transformation, public IOHprofiler_logger {
public:
  //void problem();
  //~problem();
  void evaluate(std::vector<InputType> x, std::vector<double> &y) {
    if(instance_id >= 1 && instance_id < 50) { 
      transform_vars_xor(x,instance_id);
    } else if(instance_id >= 50 && instance_id < 100) {
      transform_vars_sigma(x,instance_id);
    }
    internal_evaluate(x,y);
    copyVector(y,raw_objectives);
    transform_obj_scale(y,instance_id);
    transform_obj_shift(y,instance_id);
    //internal_transfer_objectives(y);
    if(compareVector(y,optimal)) {
      optimalFound = true;
    }
  };
  virtual void internal_evaluate(std::vector<InputType> x, std::vector<double> &y) {
    printf("No evaluate function defined");
  };
  virtual void constraints(std::vector<InputType> x, std::vector<double> c) {
    printf("No constraints function defined");
  };
  void internal_transfer_variables(std::vector<InputType> x) {
    printf("No constraints function defined");
  };
  void internal_transfer_objectives(std::vector<InputType> y) {
    printf("No constraints function defined");
  };  

  void addLogger(IOHprofiler_logger logger);

  //Interface for info of problems
  int IOHprofiler_get_number_of_varibles();
  int IOHprofiler_get_number_of_objectives();
  int IOHprofiler_get_transformed_number_of_variables();
  int IOHprofiler_get_transformed_number_of_objectives();
  std::string IOHprofiler_get_problem_name();
  std::string IOHprofiler_get_problem_type();
  int IOHprofiler_get_problem_id();
  int IOHprofiler_get_instance_id();
  bool IOHprofiler_hit_optimal();

  //Internal variables for transformation
  std::vector<double> IOHprofiler_get_objectives();
  std::vector<InputType> IOHprofiler_get_transformed_variables();
  std::vector<double> IOHprofiler_get_transformed_objectives();

  int problem_id;
  int instance_id;
  std::string problem_name;
  /* If could be good to set as enum{}*/
  std::string problem_type;

  int number_of_variables;
  int number_of_objectives;
  std::vector<double> raw_objectives;
  //size_t number_of_constraints;
  //std::vector<double> constraints;

  int transformed_number_of_variables;;
  std::vector<InputType> transformed_variables; 

  std::vector<InputType> lowerbound;
  std::vector<InputType> upperbound;
  std::vector<InputType> best_variables;
  std::vector<InputType> best_transformed_variables;
  std::vector<double> optimal;


  IOHprofiler_logger logger = NULL;

  bool optimalFound = false;
  //IOHprofiler_logger logger;
};

#endif