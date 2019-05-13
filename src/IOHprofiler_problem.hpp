#ifndef _IOHPROFILER_PROBLEM_HPP
#define _IOHPROFILER_PROBLEM_HPP


#include "common.h"
#include "IOHprofiler_transformation.hpp"
#include "IOHprofiler_csv_logger.hpp"

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

template <class InputType> class IOHprofiler_problem {
public:
  //void problem();
  //~problem();
  IOHprofiler_problem(){}

  IOHprofiler_problem(int instance_id, int dimension){}

  IOHprofiler_problem( const IOHprofiler_problem &problem) {
    this->problem_id = problem.problem_id;
    this->instance_id = problem.instance_id;
    this->problem_name = problem.problem_name;
    this->problem_type = problem.problem_type;
    
    copyVector(problem.lowerbound,this->lowerbound);
    copyVector(problem.upperbound,this->upperbound);
    if(problem.evaluate_int_info.size() != 0) copyVector(problem.evaluate_int_info,this->evaluate_int_info);
    if(problem.evaluate_double_info.size() != 0) copyVector(problem.evaluate_double_info,this->evaluate_double_info);
  
    this->number_of_variables = problem.number_of_variables;
    this->number_of_objectives = problem.number_of_objectives;
    
    copyVector(problem.raw_objectives,this->raw_objectives);

    this->transformed_number_of_variables = problem.transformed_number_of_variables;
    copyVector(problem.transformed_variables,this->transformed_variables);

    copyVector(problem.best_variables,this->best_variables);
    copyVector(problem.best_transformed_variables,this->best_variables);
    copyVector(problem.optimal,this->optimal);


    this->evaluations = 0;

    copyVector(problem.best_so_far_raw_objectives,this->best_so_far_raw_objectives);
    int best_so_far_raw_evaluations = 0;
    copyVector(problem.best_so_far_transformed_objectives,this->best_so_far_transformed_objectives);
    int best_so_far_transformed_evaluations = 0;
    if(problem.csv_logger != NULL) this->csv_logger = IOHprofiler_csv_logger(problem.csv_logger);

  }
  ~IOHprofiler_problem(){};

  // A common function for evaluating fitness of problems.
  // Raw evaluate process, tranformation operations, and logging process are excuted 
  // in this function.
  void evaluate(std::vector<InputType> x, std::vector<double> &y) {
    if(evaluations == 0) {
      best_so_far_raw_objectives.reserve(number_of_objectives);
      best_so_far_transformed_objectives.reserve(number_of_objectives);
      for(int i = 0; i !=  number_of_objectives; ++i) {
        best_so_far_raw_objectives.push_back(DBL_MIN_EXP);
        best_so_far_transformed_objectives.push_back(DBL_MIN_EXP);
      }

      copyVector(best_variables,best_transformed_variables);
      if(instance_id > 1 && instance_id <= 50) { 
        transformation.transform_vars_xor(best_transformed_variables,instance_id);
      } else if(instance_id > 50 && instance_id <= 100) {
        transformation.transform_vars_sigma(best_transformed_variables,instance_id);
      }
      internal_evaluate(best_transformed_variables,optimal);
      if(instance_id > 1) {
        transformation.transform_obj_scale(optimal,instance_id);
        transformation.transform_obj_shift(optimal,instance_id);
      }

    }
    ++evaluations;

    if(instance_id > 1 && instance_id <= 50) { 
      transformation.transform_vars_xor(x,instance_id);
    } else if(instance_id > 50 && instance_id <= 100) {
      transformation.transform_vars_sigma(x,instance_id);
    }
    
    internal_evaluate(x,y);
    
    raw_objectives.reserve(number_of_objectives);
    copyVector(y,raw_objectives);
    if(compareObjectives(y,best_so_far_raw_objectives)) {
      copyVector(y,best_so_far_raw_objectives);
      best_so_far_raw_evaluations = evaluations;
    }
    
    if(instance_id > 1) {
      transformation.transform_obj_scale(y,instance_id);
      transformation.transform_obj_shift(y,instance_id);
    }

    if(compareObjectives(y,best_so_far_transformed_objectives)){
      copyVector(y,best_so_far_transformed_objectives);
      best_so_far_transformed_evaluations = evaluations;
    }
    if(compareVector(y,optimal)) {
      optimalFound = true;
    }

    if(&this->csv_logger != NULL) {
      (this->csv_logger).write_line(this->evaluations,
                                  this->raw_objectives[0],this->best_so_far_raw_objectives[0],
                                  y[0],this->best_so_far_transformed_objectives[0]);
    }

  };
  
  virtual void internal_evaluate(std::vector<InputType> x, std::vector<double> &y) {
    printf("No evaluate function defined\n");
  };
  
  // virtual double constraints() {};
  // virtual void constraints(std::vector<InputType> x, std::vector<double> c) {
  //   printf("No constraints function defined\n");
  // };

  // If want to output csv files, a csv_logger needs to be assigned to the problem.
  // Otherwise there will be no csv files outputed with 'csv_logger == NULL'.
  void addCSVLogger(IOHprofiler_csv_logger &logger) {
    csv_logger = logger;
    csv_logger.target_problem(this->problem_id,this->number_of_variables,this->instance_id);
  };
  //void reset_logger(IOHprofiler_logger logger);

  void clearLogger() {
    this->csv_logger.clear_logger();
  }
    
  
  bool IOHprofiler_hit_optimal() {
    return optimalFound;
  }

  int IOHprofiler_get_problem_id() {
    return this->problem_id;
  };

  void IOHprofiler_set_problem_id(int problem_id) {
    this->problem_id = problem_id;
  };
  
  int IOHprofiler_get_instance_id() {
    return this->instance_id;
  };

  void IOHprofiler_set_instance_id(int instance_id) {
    this->instance_id = instance_id;
  };

  std::string IOHprofiler_get_problem_name() {
    return this->problem_name;
  };

  void IOHprofiler_set_problem_name(std::string problem_name) {
    this->problem_name = problem_name;
  };

  std::string IOHprofiler_get_problem_type() {
    return this->problem_type;
  };

  void IOHprofiler_set_problem_type(std::string problem_type) {
    this->problem_type = problem_type;
  };

  std::vector<InputType> IOHprofiler_get_lowerbound() {
    return this->lowerbound;
  };
  
  void IOHprofiler_set_lowerbound(int lowerbound) {
    std::vector<InputType>().swap(this->lowerbound);
    this->lowerbound.reserve(this->number_of_variables);
    for (int i = 0; i < this->number_of_variables; ++i) {
      this->lowerbound.push_back(lowerbound);
    }
  };

  void IOHprofiler_set_lowerbound(std::vector<InputType> lowerbound) {
    copyVector(lowerbound,this->lowerbound);
  };

  std::vector<InputType> IOHprofiler_get_upperbound() {
    return this->lowerbound;
  };

  void IOHprofiler_set_upperbound(int upperbound) {
    std::vector<InputType>().swap(this->upperbound);
    this->upperbound.reserve(this->number_of_variables);
    for (int i = 0; i < this->number_of_variables; ++i) {
      this->upperbound.push_back(upperbound);
    }
  };

  void IOHprofiler_set_upperbound(std::vector<InputType> upperbound) {
    copyVector(upperbound,this->upperbound);
  };

  std::vector<int> IOHprofiler_get_evaluate_int_info() {
    return this->evaluate_int_info;
  };

  void IOHprofiler_set_evaluate_int_info(std::vector<int> evaluate_int_info) {
    copyVector(evaluate_int_info, this->evaluate_int_info);
  };

  std::vector<double> IOHprofiler_get_evaluate_double_info() {
    return this->evaluate_double_info;
  };

  void IOHprofiler_set_evaluate_double_info(std::vector<double> evaluate_double_info) {
    copyVector(evaluate_double_info, this->evaluate_double_info);
  };
 
  int IOHprofiler_get_number_of_variables() {
    return this->number_of_variables;
  };

  void IOHprofiler_set_number_of_variables(int number_of_variables) {
    this->number_of_variables = number_of_variables;
  };

  int IOHprofiler_get_number_of_objectives() {
    return this->number_of_objectives;
  };

  void IOHprofiler_set_number_of_objectives(int number_of_objectives) {
    this->number_of_objectives = number_of_objectives;
  };

  std::vector<double> IOHprofiler_get_raw_objectives() {
    return this->raw_objectives;
  };

  int IOHprofiler_get_transformed_number_of_variables() {
    return this->transformed_number_of_variables;
  };

  std::vector<InputType> IOHprofiler_get_transformed_variables() {
    return this->transformed_variables;
  };

  std::vector<InputType> IOHprofiler_get_best_variables() {
    return this->best_variables;
  };

  void IOHprofiler_set_best_variables(int best_variables) {
    std::vector<InputType>().swap(this->best_variables);
    this->best_variables.reserve(this->number_of_variables);
    for (int i = 0; i < this->number_of_variables; ++i) {
      this->best_variables.push_back(best_variables);
    }
  };

  void IOHprofiler_set_best_variables(std::vector<InputType> best_variables) {
    copyVector(best_variables,this->best_variables);
  };

  std::vector<double> IOHprofiler_get_optimal() {
    return this->optimal;
  };

  void IOHprofiler_set_optimal(double optimal) {
    std::vector<double>().swap(this->optimal);
    this->optimal.reserve(this->number_of_objectives);
    for (int i = 0; i < this->number_of_objectives; ++i) {
      this->optimal.push_back(optimal);
    }
  };

  void IOHprofiler_set_optimal(std::vector<double> optimal) {
    copyVector(optimal,this->optimal);
  };

  void IOHprofiler_evaluate_optimal(std::vector<InputType> best_variables) {
    this->evaluate(best_variables,this->optimal);
  };

  void IOHprofiler_evaluate_optimal() {
    this->evaluate(this->best_variables,this->optimal);
  };

  int IOHprofiler_get_evaluations() {
    return evaluations;
  };

  std::vector<double> IOHprofiler_get_best_so_far_raw_objectives() {
    return this->best_so_far_raw_objectives;
  };
  int IOHprofiler_get_best_so_far_raw_evaluations() {
    return this-> best_so_far_raw_evaluations;
  };
  std::vector<double> IOHprofiler_get_best_so_far_transformed_objectives(){
    return this->best_so_far_transformed_objectives;
  };
  int IOHprofiler_get_best_so_far_transformed_evaluations() {
    return this->best_so_far_transformed_evaluations;
  };

  void * IOHprofiler_get_csv_logger() {
    if(this->csv_logger != NULL) return &this->csv_logger;
  }

private:
  
  int problem_id;
  int instance_id;
  std::string problem_name;
  /* If could be good to set as enum{}*/
  std::string problem_type;
  std::vector<InputType> lowerbound;
  std::vector<InputType> upperbound;
  // These are used for potential common used info for evaluating variables.
  std::vector<int> evaluate_int_info;
  std::vector<double> evaluate_double_info;

  std::size_t number_of_variables;
  std::size_t number_of_objectives;

  std::vector<double> raw_objectives;
  
  int transformed_number_of_variables;;
  std::vector<InputType> transformed_variables; 


  std::vector<InputType> best_variables;
  std::vector<InputType> best_transformed_variables;
  std::vector<double> optimal;

  //std::size_t number_of_constraints;
  //std::vector<double> constraints;

  int evaluations = 0;
  std::vector<double> best_so_far_raw_objectives;
  int best_so_far_raw_evaluations = 0;
  std::vector<double> best_so_far_transformed_objectives;
  int best_so_far_transformed_evaluations;

  IOHprofiler_transformation transformation;

  IOHprofiler_csv_logger csv_logger;

  bool optimalFound = false;

};

#endif //_IOHPROFILER_PROBLEM_HPP