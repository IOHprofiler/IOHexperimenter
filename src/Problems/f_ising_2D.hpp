#ifndef _F_ISING_TWOD_H
#define _F_ISING_TWOD_H

#include "../IOHprofiler_problem.hpp"

class Ising_2D : public IOHprofiler_problem<int> {
public:
  Ising_2D() {

    IOHprofiler_set_problem_id(20);
    IOHprofiler_set_instance_id(1);
    IOHprofiler_set_problem_name("Ising_2D");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
  }
  //~Ising_2D();
  
  Ising_2D(int instance_id, int dimension) {
    IOHprofiler_set_problem_id(20);
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Ising_2D");
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

  int modulo_ising_2D(int x,int N) {
    return (x % N + N) %N;
  }

  void internal_evaluate(std::vector<int> x,std::vector<double> &y) {
    y.clear();
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

    y.push_back((double)result);
  };
};

#endif