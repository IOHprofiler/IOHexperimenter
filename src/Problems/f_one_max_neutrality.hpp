#ifndef _F_ONE_MAX_NEUTRALITY_H
#define _F_ONE_MAX_NEUTRALITY_H

// This file implements a OneMax problem with neutrality transformation method from w-model.
// The parameter mu is chosen as 3.

#include "../IOHprofiler_problem.hpp"
#include "common_used_functions/wmodels.hpp"

class OneMax_Neutrality : public IOHprofiler_problem<int> {
public:
  OneMax_Neutrality() {
    IOHprofiler_set_problem_name("OneMax_Neutrality");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);  
  }
  //~OneMax_Neutrality();
  
  OneMax_Neutrality(int instance_id, int dimension) {

    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("OneMax_Neutrality");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
    Initilize_problem(dimension);
  }

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
    IOHprofiler_set_optimal((double)ceil(dimension/3));
  };

  std::vector<double> internal_evaluate(std::vector<int> x) {
    std::vector<double> y;
    std::vector<int> new_variables = neutrality(x,3);
    int n = new_variables.size();
    int result = 0;
    for(int i = 0; i != n; ++i) {
      result += new_variables[i];
    }
    y.push_back(result);
    return y;
  };

  static OneMax_Neutrality * createInstance() {
    return new OneMax_Neutrality();
  };

  static OneMax_Neutrality * createInstance(int instance_id, int dimension) {
    return new OneMax_Neutrality(instance_id, dimension);
  };
};

#endif