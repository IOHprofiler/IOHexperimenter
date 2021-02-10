#pragma once
#include "pbo_base.hpp"

namespace ioh {
    namespace problem {
        namespace pbo {

            class Ising_Ring : public pbo_base {
            public:
                /**
                       * \brief Construct a new Ising_Ring object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
                       * 
                       * \param instance_id The instance number of a problem, which controls the transformation
                       * performed on the original problem.
                       * \param dimension The dimensionality of the problem to created, 4 by default.
                       **/
                Ising_Ring(int instance_id = IOH_DEFAULT_INSTANCE,
                           int dimension = IOH_DEFAULT_DIMENSION)
                    : pbo_base(19, "Ising_Ring", instance_id, dimension) {
                    set_best_variables(1);
                    set_number_of_variables(dimension);

                }

                static int modulo_ising_ring(int x, int N) {
                    return (x % N + N) % N;
                }

                double internal_evaluate(const std::vector<int> &x) override {
                    auto result = 0.0;
                    for (auto i = 0; i < n_; ++i) {
                        const auto neig = x[modulo_ising_ring(i - 1, n_)];
                        result += x[i] * neig + (1 - x[i]) * (1 - neig);
                    }
                    return static_cast<double>(result);
                }

                static Ising_Ring *create(
                    int instance_id = IOH_DEFAULT_INSTANCE,
                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new Ising_Ring(instance_id, dimension);
                }
            };
        }
    }
}
