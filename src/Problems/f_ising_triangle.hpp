#ifndef _F_ISING_TRIANGLE_H
#define _F_ISING_TRIANGLE_H

#include "../IOHprofiler_problem.hpp"

class Ising_Triangle : public IOHprofiler_problem<int> {
public:
  Ising_Triangle() {

    IOHprofiler_set_problem_id(21);
    IOHprofiler_set_instance_id(1);
    IOHprofiler_set_problem_name("Ising_Triangle");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
  }
  //~Ising_Triangle();
  
  Ising_Triangle(int instance_id, int dimension) {
    IOHprofiler_set_problem_id(21);
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Ising_Triangle");
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
  
  int modulo_ising_triangle(int x,int N){
    return (x % N + N) %N;
  }

  void internal_evaluate(std::vector<int> x,std::vector<double> &y) {
    y.clear();
    int i,j,neig;
    int n = x.size();
    int result = 0;
    int neighbors[6];
    int lattice_size = (int)sqrt((double)n);
    
    for (i = 0; i < lattice_size; ++i) {
      for (j = 0; j < lattice_size; ++j) {
        neighbors[0] = x[modulo_ising_triangle((i - 1), lattice_size) * lattice_size + j] ;
                neighbors[1] = x[modulo_ising_triangle((i + 1), lattice_size) * lattice_size + j] ;
                neighbors[2] = x[i * lattice_size + modulo_ising_triangle((j - 1) , lattice_size)] ;
                neighbors[3] = x[i * lattice_size + modulo_ising_triangle((i + 1) , lattice_size)] ;
                neighbors[4] = x[modulo_ising_triangle((i - 1) , lattice_size) * lattice_size + modulo_ising_triangle((j - 1) , lattice_size)] ;
                neighbors[5] = x[modulo_ising_triangle((i + 1) , lattice_size) * lattice_size + modulo_ising_triangle((j + 1) , lattice_size)];

        for (neig=0; neig<6; neig++) {
          result+= (x[i * lattice_size + j] * neighbors[neig]) + ((1- x[i * lattice_size + j])*(1- neighbors[neig]));
        }
      }
    }
    y.push_back((double)result);
  };
};

#endif