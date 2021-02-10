#pragma once
#include "pbo_base.hpp"
#include "ioh/problem/utils.hpp"

namespace ioh {
    namespace problem {
        namespace pbo {
            class LeadingOnes_Neutrality : public pbo_base {
            public:
                /**
                 * \brief Construct a new LeadingOnes_Neutrality object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
                 * 
                 * \param instance_id The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param dimension The dimensionality of the problem to created, 4 by default.
                 **/
                LeadingOnes_Neutrality(int instance_id = IOH_DEFAULT_INSTANCE,
                                       int dimension = IOH_DEFAULT_DIMENSION)
                    : pbo_base(13, "LeadingOnes_Neutrality", instance_id, dimension) {
                    set_best_variables(1);
                    set_number_of_variables(dimension);
                }

                double internal_evaluate(const std::vector<int> &x) override {
                    auto new_variables = utils::neutrality(x, 3);
                    auto result = 0.0;
                    auto n = static_cast<int>(new_variables.size());
                    for (auto i = 0; i != n; ++i) {
                        if (new_variables[i] == 1) {
                            result = i + 1;
                        } else {
                            break;
                        }
                    }
                    return static_cast<double>(result);
                }

                static LeadingOnes_Neutrality *create(
                    int instance_id = IOH_DEFAULT_INSTANCE,
                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new LeadingOnes_Neutrality(instance_id, dimension);
                }
            };
        }
    }
}
