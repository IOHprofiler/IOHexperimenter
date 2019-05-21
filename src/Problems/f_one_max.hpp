#ifndef _F_ONE_MAX_H
#define _F_ONE_MAX_H

#include "../IOHprofiler_problem.hpp"

class OneMax : public IOHprofiler_problem<int> {
public:
  OneMax() {

    IOHprofiler_set_problem_id(1);
    IOHprofiler_set_instance_id(1);
    IOHprofiler_set_problem_name("OneMax");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
  }
  //~OneMax();
  
  OneMax(int instance_id, int dimension) {
    IOHprofiler_set_problem_id(1);
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("OneMax");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);

    Initilize_problem(dimension);
  }

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
    IOHprofiler_set_optimal((double)dimension);
  };

  std::vector<double> internal_evaluate(std::vector<int> x) {
    std::vector<double> y;
    int n = x.size();
    int result = 0;
    for(int i = 0; i != n; ++i) {
      result += x[i];
    }
    y.push_back((double)result);
    return y;
  };
};

#endif