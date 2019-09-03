/// \file f_ising_triangle.hpp
/// \brief cpp file for class f_ising_triangle.
///
/// A detailed file description.
///
/// \author Naama Horesh and Furong Ye
/// \date 2019-06-27
#ifndef _F_ISING_TRIANGLE_H
#define _F_ISING_TRIANGLE_H

#include "../Template/IOHprofiler_problem.hpp"

class Ising_Triangle : public IOHprofiler_problem<int> {
public:
  Ising_Triangle() {
    IOHprofiler_set_problem_name("Ising_Triangle");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
  }
  Ising_Triangle(int instance_id, int dimension) {

    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Ising_Triangle");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
    Initilize_problem(dimension);
  }
  ~Ising_Triangle() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };
  
  int modulo_ising_triangle(int x,int N) {
    return (x % N + N) %N;
  }

  double internal_evaluate(const std::vector<int> &x) {
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
    return (double)result;
  };

  static Ising_Triangle * createInstance() {
    return new Ising_Triangle();
  };

  static Ising_Triangle * createInstance(int instance_id, int dimension) {
    return new Ising_Triangle(instance_id, dimension);
  };
};

#endif