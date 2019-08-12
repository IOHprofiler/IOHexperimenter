/// \file f_one_max.hpp
/// \brief cpp file for class f_one_max.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _F_ONE_MAX_H
#define _F_ONE_MAX_H

#include "../Template/IOHprofiler_problem.hpp"

class OneMax : public IOHprofiler_problem<int> {
public:
  OneMax() {
    IOHprofiler_set_problem_name("OneMax");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
  }

  OneMax(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("OneMax");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
    Initilize_problem(dimension);
  }
  
  ~OneMax() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
    IOHprofiler_set_optimal((double)dimension);
  };

  double internal_evaluate(const std::vector<int> &x) {
  
    int n = x.size();
    int result = 0;
    for (int i = 0; i != n; ++i) {
      result += x[i];
    }
    return (double)result;
  };

  static OneMax * createInstance() {
    return new OneMax();
  };

  static OneMax * createInstance(int instance_id, int dimension) {
    return new OneMax(instance_id, dimension);
  };
};

#endif