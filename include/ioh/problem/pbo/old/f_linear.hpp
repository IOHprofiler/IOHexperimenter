#pragma once
#include "pbo_base.hpp"

namespace ioh {
    namespace problem {
        namespace pbo {
            class Linear : public pbo_base {
            public:
                std::vector<double> info;

                /**
                 * \brief Construct a new Linear object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
                 * 
                 * \param instance_id The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param dimension The dimensionality of the problem to created, 4 by default.
                 **/
                Linear(int instance_id = IOH_DEFAULT_INSTANCE,
                       int dimension = IOH_DEFAULT_DIMENSION)
                    : pbo_base(3, "Linear", instance_id, dimension) {
                    set_best_variables(1);
                    set_number_of_variables(dimension);
                }

                double internal_evaluate(const std::vector<int> &x) override {
                    double result = 0.0;
                    for (auto i = 0; i < n_; ++i) {
                        result += static_cast<double>(x[i]) * static_cast<double>(i + 1);
                    }
                    return result;
                }

                static Linear *create(int instance_id = IOH_DEFAULT_INSTANCE,
                                      int dimension = IOH_DEFAULT_DIMENSION) {
                    return new Linear(instance_id, dimension);
                }
            };
        }
    }
}
