/// \file f_ising_ring.hpp
/// \brief cpp file for class Ising_Ring.
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
            class Ising_Ring : public pbo_base
            {
            public:
                Ising_Ring(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
                    : pbo_base("Ising_Ring", instance_id)
                {
                    set_best_variables(1);
                	set_number_of_variables(dimension);
                }
                int modulo_ising_ring(int x, int N) {
                    return (x % N + N) % N;
                }

                double internal_evaluate(const std::vector<int>& x) override {
	                auto result = 0;
	                auto n = x.size();

                    int neig;
                    for (auto i = 0; i < n; ++i) {
                        neig = x[modulo_ising_ring((i - 1), static_cast<int>(n))];
                        result += (x[i] * neig) - ((1 - x[i]) * (1 - neig));
                    }
                    return (double)result;
                }

                static Ising_Ring* createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
                    return new Ising_Ring(instance_id, dimension);
                }
            	
            };
        }
    }
}
