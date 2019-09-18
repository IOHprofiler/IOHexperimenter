/// \file f_labs.hpp
/// \brief cpp file for class f_labs.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _F_LABS_H
#define _F_LABS_H

#include <cstdint>
#include "../Template/IOHprofiler_problem.hpp"

class LABS : public IOHprofiler_problem<int> {
public:
  LABS() {
    IOHprofiler_set_problem_name("LABS");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
  }
  LABS(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("LABS");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    Initilize_problem(dimension);
  }
  
  ~LABS() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };

  int64_t correlation(const std::vector<int> x, const int n, int k) {
    int64_t result = 0;
    for (int i = 0 ; i < n - k; ++i) {
        // x[i] and x[i+k] different = -1, otherwise 1, this logically gives:
        //     result += 1 - 2*(x[i] != x[i+k]);
        // But we can take out the +1 and 2* out of the loop.
        result += x[i] != x[i+k];
    }
    return n - k - 2*result;
  }

  double internal_evaluate(const std::vector<int> &x) {
    size_t n = x.size();
    int64_t result = 0, cor;
    for (int k = 1; k != n; ++k) {
      cor = correlation(x,n,k);
      result += cor * cor;
    }
    return double(n*n)/2.0/result;
  };

  static LABS * createInstance() {
    return new LABS();
  };

  static LABS * createInstance(int instance_id, int dimension) {
    return new LABS(instance_id, dimension);
  };
};

#endif
