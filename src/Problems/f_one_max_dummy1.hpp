/// \file f_one_max_dummy1.hpp
/// \brief cpp file for class f_one_max_dummy1.
///
/// This file implements a OneMax problem with reduction of dummy variables.
/// The reduction rate is chosen as 0.5.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _F_ONE_MAX_DUMMYONE_H
#define _F_ONE_MAX_DUMMYONE_H

#include "../IOHprofiler_problem.hpp"
#include "common_used_functions/wmodels.hpp"

class OneMax_Dummy1 : public IOHprofiler_problem<int> {
public:
  OneMax_Dummy1() {
    IOHprofiler_set_problem_name("OneMax_Dummy1");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
  }
  
  OneMax_Dummy1(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("OneMax_Dummy1");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
    Initilize_problem(dimension);
  }

  ~OneMax_Dummy1() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
    IOHprofiler_set_evaluate_int_info(dummy(dimension,0.5,10000));
    IOHprofiler_set_optimal(floor((double)(dimension * 0.5)));
  };

  std::vector<double> internal_evaluate(std::vector<int> x) {
    std::vector<double> y;
    int n = IOHprofiler_get_evaluate_int_info().size();
    int result = 0;
    for (int i = 0; i != n; ++i) {
      result += x[IOHprofiler_get_evaluate_int_info()[i]];
    }
    y.push_back(result);
    return y;
  };

  static OneMax_Dummy1 * createInstance() {
    return new OneMax_Dummy1();
  };

  static OneMax_Dummy1 * createInstance(int instance_id, int dimension) {
    return new OneMax_Dummy1(instance_id, dimension);
  };
};

#endif