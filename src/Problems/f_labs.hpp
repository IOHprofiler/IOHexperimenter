/// \file f_labs.hpp
/// \brief cpp file for class f_labs.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _F_LABS_H
#define _F_LABS_H

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

  double correlation(const std::vector<int> x, const int n, int k)
  {
    int x1,x2;
    double result;
    result = 0.0;
    for (int i = 0 ; i < n - k; ++i) {
        if (x[i] == 0) {
          x1 = -1;
        } else {
          x1 = 1;
        }
        if (x[i + k] == 0) {
          x2 = -1;
        } else {
          x2 = 1;
        }
        result += x1 * x2;
    }
    return result;
  }

  std::vector<double> internal_evaluate(std::vector<int> x) {
    std::vector<double> y;
    int n = x.size();
    double result = 0.0, cor;
    for (int k = 1; k != n; ++k) {
      cor = correlation(x,n,k);
      result += cor * cor;
    }
    result = (double)(n*n)/2.0/result;
    y.push_back(result);
    return y;
  };

  static LABS * createInstance() {
    return new LABS();
  };

  static LABS * createInstance(int instance_id, int dimension) {
    return new LABS(instance_id, dimension);
  };
};

#endif