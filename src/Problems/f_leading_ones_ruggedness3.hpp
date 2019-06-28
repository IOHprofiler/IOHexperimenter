#ifndef _F_LEADING_ONES_RUGGEDNESSTHREE_H
#define _F_LEADING_ONES_RUGGEDNESSTHREE_H

#include "../IOHprofiler_problem.hpp"
#include "common_used_functions/wmodels.hpp"

class LeadingOnes_Ruggedness3 : public IOHprofiler_problem<int> {
public:
   LeadingOnes_Ruggedness3() {
    IOHprofiler_set_problem_name("LeadingOnes_Ruggedness3");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
  }
  //~LeadingOnes_Ruggedness3();
  
  LeadingOnes_Ruggedness3(int instance_id, int dimension) {

    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("LeadingOnes_Ruggedness3");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
    Initilize_problem(dimension);
  }

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
    IOHprofiler_set_evaluate_double_info(ruggedness3(dimension));
  };

  std::vector<double> internal_evaluate(std::vector<int> x) {
    std::vector<double> y;
    int n = x.size();
    int result = 0;
    for(int i = 0; i != n; ++i) {
      if(x[i] == 1)
        result = i + 1;
      else
        break;
    }
    result = IOHprofiler_get_evaluate_double_info()[(int)(result+0.5)];
    y.push_back((double)result);
    return y;
  };

  static LeadingOnes_Ruggedness3 * createInstance() {
    return new LeadingOnes_Ruggedness3();
  };

  static LeadingOnes_Ruggedness3 * createInstance(int instance_id, int dimension) {
    return new LeadingOnes_Ruggedness3(instance_id, dimension);
  };
};

#endif