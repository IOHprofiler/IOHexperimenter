/// \file f_Cancatenate_Trap.hpp
/// \brief cpp file for class f_Cancatenate_Trap.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-12-01
#ifndef _F_CANCATENATE_TRAP_H
#define _F_CANCATENATE_TRAP_H

#include "../Template/IOHprofiler_problem.hpp"

class Cancatenate_Trap : public IOHprofiler_problem<int> {
public:

  Cancatenate_Trap(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Cancatenate_Trap");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
    Initilize_problem(dimension);
  }
  
  ~Cancatenate_Trap() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
    IOHprofiler_set_optimal((double)dimension);
  };
  int k = 5;

  double internal_evaluate(const std::vector<int> &x) {
    int n = x.size();
    double result = 0.0;
    double block_result;

    int m = n / k;
    for (int i = 1; i <= m; ++i) {
      block_result = 0.0;
      for (int j = i*k - k; j != i*k; ++j) {
        block_result += x[j];
      }
      if (block_result == k) {
        result += 1;
      } else {
        result += ((double)(k - 1 - block_result) / (double) k );
      }
    }
    int remain_k = n - m * k;
    if (remain_k != 0 ) {
      block_result = 0.0;
      for (int j = m * k - 1; j != n; ++j) {
        block_result += x[j];
      }
      if (block_result == remain_k) {
        result += 1;
      } else {
        result += ((double)(remain_k - 1 - block_result) / (double) remain_k );
      }
    }

    return result;
  };

  static Cancatenate_Trap * createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
    return new Cancatenate_Trap(instance_id, dimension);
  };
};

#endif