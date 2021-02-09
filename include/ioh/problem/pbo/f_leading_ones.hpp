#pragma once
#include "pbo_base.hpp"

namespace ioh {
    namespace problem {
        namespace pbo {
            class LeadingOnes : public pbo_base {
            public:
                /**
                 * \brief Construct a new LeadingOnes object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
                 * 
                 * \param instance_id The instance number of a problem, which controls the transformation
                 * performed on the original problem.
                 * \param dimension The dimensionality of the problem to created, 4 by default.
                 **/
                LeadingOnes(int instance_id = IOH_DEFAULT_INSTANCE,
                            int dimension = IOH_DEFAULT_DIMENSION)
                    : pbo_base(2, "LeadingOnes", instance_id) {
                    set_best_variables(1);
                    set_number_of_variables(dimension);
                }

                double internal_evaluate(const std::vector<int> &x) override {
                    const auto n = x.size();
                    auto result = 0;
                    for (auto i = 0; i != n; ++i) {
                        if (x[i] == 1)
                            result = i + 1;
                        else
                            break;
                    }
                    return static_cast<double>(result);
                }

                static LeadingOnes *create(
                    int instance_id = IOH_DEFAULT_INSTANCE,
                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new LeadingOnes(instance_id, dimension);
                }
            };
        }
    }
}
