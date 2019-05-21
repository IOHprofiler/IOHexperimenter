#ifndef _F_ONE_MAX_DUMMYTWO_H
#define _F_ONE_MAX_DUMMYTWO_H
// This file implements a OneMax problem with reduction of dummy variables.
// The reduction rate is chosen as 0.9.


#include "../IOHprofiler_problem.hpp"
#include "common_used_functions/wmodels.hpp"

class OneMax_Dummy2 : public IOHprofiler_problem<int> {
public:
  OneMax_Dummy2() {

    IOHprofiler_set_problem_id(5);
    IOHprofiler_set_instance_id(1);
    IOHprofiler_set_problem_name("OneMax_Dummy2");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
  }
  //~OneMax_Dummy1();
  
  OneMax_Dummy2(int instance_id, int dimension) {
    IOHprofiler_set_problem_id(5);
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("OneMax_Dummy2");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    Initilize_problem(dimension);
  }

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
    IOHprofiler_set_evaluate_int_info(dummy(dimension,0.9,10000));
    IOHprofiler_set_optimal(floor((double)(dimension * 0.9)));
  };

  std::vector<double> internal_evaluate(std::vector<int> x) {
    std::vector<double> y;
    int n = IOHprofiler_get_evaluate_int_info().size();
    int result = 0;
    for(int i = 0; i != n; ++i) {
      result += x[IOHprofiler_get_evaluate_int_info()[i]];
    }
    y.push_back(result);
    return y;
  };
};

#endif