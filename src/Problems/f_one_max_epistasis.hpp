#ifndef _F_ONE_MAX_EPISTASIS_H
#define _F_ONE_MAX_EPISTASIS_H

// This file implements a OneMax problem with epistasis transformation method from w-model.
// The parameter v is chosen as 4.

#include "../IOHprofiler_problem.hpp"
#include "common_used_functions/wmodels.hpp"

class OneMax_Epistasis : public IOHprofiler_problem<int> {
public:
  OneMax_Epistasis() {

    IOHprofiler_set_problem_id(7);
    IOHprofiler_set_instance_id(1);
    IOHprofiler_set_problem_name("OneMax_Epistasis");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
  
  }
  //~OneMax_Epistasis();
  
  OneMax_Epistasis(int instance_id, int dimension) {
    IOHprofiler_set_problem_id(7);
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("OneMax_Epistasis");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);

    Initilize_problem(dimension);
  }

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
  };

  void internal_evaluate(std::vector<int> x,std::vector<double> &y) {
    y.clear();
    std::vector<int> new_variables = epistasis(x,4);
    int n = new_variables.size();
    int result = 0;
    for(int i = 0; i != n; ++i) {
      result += new_variables[i];
    }
    y.push_back(result);
  };
};

#endif