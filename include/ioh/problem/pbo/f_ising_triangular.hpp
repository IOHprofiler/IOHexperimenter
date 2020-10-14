/// \file f_ising_triangular.hpp
/// \brief cpp file for class Ising_Triangular.
///
/// A detailed file description.
///
/// \author Naama Horesh and Furong Ye
/// \date 2019-06-27
#pragma once
#include "pbo_base.hpp"

namespace ioh
{
    namespace problem
    {
        namespace pbo
        {
            class Ising_Triangular : public pbo_base
            {
            public:
                Ising_Triangular(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
                    : pbo_base("Ising_Triangular", instance_id)
                {
                    set_best_variables(1);
                	set_number_of_variables(dimension);
                }
            	
                static int modulo_ising_triangular(int x, int N) {
                    return (x % N + N) % N;
                }

                double internal_evaluate(const std::vector<int>& x) override {
                    int i, j, neig;
                    auto n = x.size();
                    auto result = 0;
                    int neighbors[3];
                    auto lattice_size = (int)sqrt((double)n);

                    for (i = 0; i < lattice_size; ++i) {
                        for (j = 0; j < lattice_size; ++j) {
                            neighbors[0] = x[modulo_ising_triangular((i + 1), lattice_size) * lattice_size + j];
                            neighbors[1] = x[i * lattice_size + modulo_ising_triangular((j + 1), lattice_size)];
                            neighbors[2] = x[modulo_ising_triangular((i + 1), lattice_size) * lattice_size + modulo_ising_triangular((j + 1), lattice_size)];

                            for (neig = 0; neig < 3; neig++) {
                                result += (x[i * lattice_size + j] * neighbors[neig]) - ((1 - x[i * lattice_size + j]) * (1 - neighbors[neig]));
                            }
                        }
                    }
                    return (double)result;
                }

                static Ising_Triangular* create(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
                    return new Ising_Triangular(instance_id, dimension);
                }
                
            };
        }
    }
}
