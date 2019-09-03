/// \file f_ising_1D.hpp
/// \brief cpp file for class f_ising_1D.
///
/// A detailed file description.
///
/// \author Naama Horesh and Furong Ye
/// \date 2019-06-27
#ifndef _F_ISING_ONED_H
#define _F_ISING_ONED_H

#include "../Template/IOHprofiler_problem.hpp"

class Ising_1D : public IOHprofiler_problem<int> {
public:
  Ising_1D() {
    IOHprofiler_set_problem_name("Ising_1D");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
  }
  Ising_1D(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Ising_1D");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
    Initilize_problem(dimension);
  }
  ~Ising_1D() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };

  int modulo_ising_1D(int x,int N) {
    return (x % N + N) %N;
  }

  double internal_evaluate(const std::vector<int> &x) {
    int result= 0, n = x.size();

    for (int i = 0; i < n; ++i) {
        int first_neig=x[modulo_ising_1D((i+1), n)];
        int second_neig=x[modulo_ising_1D((i -1), n)];

        result += (x[i] *first_neig) + ((1- x[i])*(1- first_neig));
        result += (x[i] *second_neig) + ((1- x[i])*(1- second_neig));
    }
    return (double)result;
  };
  
  static Ising_1D * createInstance() {
    return new Ising_1D();
  };

  static Ising_1D * createInstance(int instance_id, int dimension) {
    return new Ising_1D(instance_id, dimension);
  };
};

#endif