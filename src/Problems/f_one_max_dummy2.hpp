/// \file f_one_max_dummy2.hpp
/// \brief cpp file for class f_one_max_dummy2.
///
/// This file implements a OneMax problem with reduction of dummy variables.
/// The reduction rate is chosen as 0.9.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _F_ONE_MAX_DUMMYTWO_H
#define _F_ONE_MAX_DUMMYTWO_H

#include "../Template/IOHprofiler_problem.hpp"
#include "common_used_functions/wmodels.hpp"

class OneMax_Dummy2 : public IOHprofiler_problem<int> {
public:
  OneMax_Dummy2() {
    IOHprofiler_set_problem_name("OneMax_Dummy2");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
  }
  
  OneMax_Dummy2(int instance_id, int dimension) {

    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("OneMax_Dummy2");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
    Initilize_problem(dimension);
  }

  ~OneMax_Dummy2() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
    IOHprofiler_set_optimal(floor((double)(dimension * 0.9)));
  };

  
  std::vector<int> info;
  void prepare_problem() {
    info = dummy(IOHprofiler_get_number_of_variables(),0.9,10000);
  }

  double internal_evaluate(const std::vector<int> &x) {
    int n = this->info.size();
    int result = 0;
    for (int i = 0; i != n; ++i) {
      result += x[this->info[i]];
    }
    return (double)result;
  };

  static OneMax_Dummy2 * createInstance() {
    return new OneMax_Dummy2();
  };

  static OneMax_Dummy2 * createInstance(int instance_id, int dimension) {
    return new OneMax_Dummy2(instance_id, dimension);
  };
};

#endif