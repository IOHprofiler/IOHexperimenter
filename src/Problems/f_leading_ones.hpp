/// \file f_leading_ones.hpp
/// \brief cpp file for class f_leading_ones.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _F_LEADING_ONES_H
#define _F_LEADING_ONES_H


#include "../Template/IOHprofiler_problem.hpp"

class LeadingOnes : public IOHprofiler_problem<int> {
public:
   LeadingOnes() {
    IOHprofiler_set_problem_name("LeadingOnes");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
  }
  LeadingOnes(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("LeadingOnes");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
    Initilize_problem(dimension);
  }
  
  ~LeadingOnes() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
    IOHprofiler_set_optimal((double)dimension);
  };

  std::vector<double> internal_evaluate(std::vector<int> x) {
    std::vector<double> y;
    int n = x.size();
    int result = 0;
    for (int i = 0; i != n; ++i) {
      if(x[i] == 1)
        result = i + 1;
      else
        break;
    }
    y.push_back((double)result);
    return y;
  };

  static LeadingOnes * createInstance() {
    return new LeadingOnes();
  };

  static LeadingOnes * createInstance(int instance_id, int dimension) {
    return new LeadingOnes(instance_id, dimension);
  };
};

#endif