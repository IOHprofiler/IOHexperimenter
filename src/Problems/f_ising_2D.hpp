/// \file f_ising_2D.hpp
/// \brief cpp file for class f_ising_2D.
///
/// A detailed file description.
///
/// \author Naama Horesh and Furong Ye
/// \date 2019-06-27
#ifndef _F_ISING_TWOD_H
#define _F_ISING_TWOD_H

#include "../Template/IOHprofiler_problem.hpp"

class Ising_2D : public IOHprofiler_problem<int> {
public:
  Ising_2D() {
    IOHprofiler_set_problem_name("Ising_2D");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
  }

  Ising_2D(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Ising_2D");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
    Initilize_problem(dimension);
  }
  ~Ising_2D() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };

  int modulo_ising_2D(int x,int N) {
    return (x % N + N) %N;
  }

  double internal_evaluate(const std::vector<int> &x) {
    int n = x.size();
    int i,j,neig;
    int result= 0;
    int neighbors[4];
    int lattice_size = (int)sqrt((double)n);
    
    if(floor(sqrt((double)n))!=sqrt((double)n)) {
      IOH_error("Number of parameters in the Ising square problem must be a square number");
    }


    for(i = 0; i < lattice_size; ++i) {
      for (j = 0; j < lattice_size; ++j) {
        neighbors[0]  = x[modulo_ising_2D(i - 1, lattice_size) * lattice_size + j];
        neighbors[1]  = x[modulo_ising_2D(i + 1, lattice_size) * lattice_size + j];
        neighbors[2]  = x[i * lattice_size + modulo_ising_2D((j - 1) , lattice_size)];
        neighbors[3]  = x[i * lattice_size + modulo_ising_2D((j + 1) , lattice_size)];

        for (neig=0; neig<4; neig++) {
          result+= (x[i*lattice_size + j] * neighbors[neig]) + ((1- x[i * lattice_size + j])*(1- neighbors[neig]));
        }
      }
    }

    return (double)result;
  };

  static Ising_2D * createInstance() {
    return new Ising_2D();
  };

  static Ising_2D * createInstance(int instance_id, int dimension) {
    return new Ising_2D(instance_id, dimension);
  };
};

#endif