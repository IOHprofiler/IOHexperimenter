#ifndef _F_LINEAR_H
#define _F_LINEAR_H

#include "../IOHprofiler_problem.hpp"

class Linear : public IOHprofiler_problem<int> {
public:
  Linear() {

    IOHprofiler_set_problem_id(3);
    IOHprofiler_set_instance_id(1);
    IOHprofiler_set_problem_name("Linear");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
  }
  //~Linear();
  
  Linear(int instance_id, int dimension) {
    IOHprofiler_set_problem_id(3);
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Linear");
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
    double result = 0;
    for (int i = 0; i < n; ++i) {
        result += (double)x[i] * (double)(i+1);
    }
    y.push_back(result);
    return y;
  };
};

#endif