/// \file f_leading_ones_dummy1.hpp
/// \brief cpp file for class f_leading_ones_dummy1.
///
/// This file implements a LeadingOnes problem with reduction of dummy variables.
/// The reduction rate is chosen as 0.5.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _F_LEADING_ONES_DUMMYONE_H
#define _F_LEADING_ONES_DUMMYONE_H

#include "IOHprofiler_problem.h"
#include "wmodels.hpp"

class LeadingOnes_Dummy1 : public IOHprofiler_problem<int> {
public:
  LeadingOnes_Dummy1(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {

    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("LeadingOnes_Dummy1");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
    IOHprofiler_set_number_of_variables(dimension);
  }

  ~LeadingOnes_Dummy1() {}

  std::vector<int> info;
  void prepare_problem() {
    info = dummy(IOHprofiler_get_number_of_variables(),0.5,10000);
  }

  double internal_evaluate(const std::vector<int> &x) {
    int n = this->info.size();
    int result = 0;
    for (int i = 0; i != n; ++i) {
      if (x[this->info[i]] == 1) {
        result = i + 1;
      } else {
        break;
      }
    }
    return (double)result;
  }

  static LeadingOnes_Dummy1 * createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
    return new LeadingOnes_Dummy1(instance_id, dimension);
  }
};

#endif
