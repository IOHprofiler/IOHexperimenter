/// \file f_one_max_ruggedness1.hpp
/// \brief cpp file for class f_one_max_ruggedness1.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _F_ONE_MAX_RUGGEDNESSONE_H
#define _F_ONE_MAX_RUGGEDNESSONE_H

#include "../Template/IOHprofiler_problem.hpp"
#include "common_used_functions/wmodels.hpp"

class OneMax_Ruggedness1 : public IOHprofiler_problem<int> {
public:
  OneMax_Ruggedness1() {
    IOHprofiler_set_problem_name("OneMax_Ruggedness1");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
  }

  OneMax_Ruggedness1(int instance_id, int dimension) {

    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("OneMax_Ruggedness1");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
    Initilize_problem(dimension);
  }
  
  ~OneMax_Ruggedness1() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };

  double internal_evaluate(const std::vector<int> &x) {
    
    int n = x.size();
    int result = 0;
    for (int i = 0; i != n; ++i) {
      result += x[i];
    }
    result = ruggedness1(result,n);
    return (double)result;
  };

  static OneMax_Ruggedness1 * createInstance() {
    return new OneMax_Ruggedness1();
  };

  static OneMax_Ruggedness1 * createInstance(int instance_id, int dimension) {
    return new OneMax_Ruggedness1(instance_id, dimension);
  };
};

#endif