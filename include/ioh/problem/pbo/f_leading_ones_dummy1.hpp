#pragma once
#include "pbo_base.hpp"
#include "ioh/problem/utils.hpp"

namespace ioh {
    namespace problem {
        namespace pbo {
            class LeadingOnes_Dummy1 : public pbo_base {
            public:
                std::vector<int> info;
                /**
                       * \brief Construct a new LeadingOnes_Dummy1 object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
                       * 
                       * \param instance_id The instance number of a problem, which controls the transformation
                       * performed on the original problem.
                       * \param dimension The dimensionality of the problem to created, 4 by default.
                       **/
                LeadingOnes_Dummy1(int instance_id = IOH_DEFAULT_INSTANCE,
                                   int dimension = IOH_DEFAULT_DIMENSION)
                    : pbo_base(11, "LeadingOnes_Dummy1", instance_id) {
                    set_best_variables(1);
                    set_number_of_variables(dimension);
                }

                void prepare_problem() override {
                    info = utils::dummy(get_number_of_variables(), 0.5, 10000);
                }

                double internal_evaluate(const std::vector<int> &x) override {
                    const auto n = this->info.size();
                    auto result = 0;
                    for (auto i = 0; i != n; ++i) {
                        if (x[this->info[i]] == 1) {
                            result = i + 1;
                        } else {
                            break;
                        }
                    }
                    return static_cast<double>(result);
                }

                static LeadingOnes_Dummy1 *create(
                    int instance_id = IOH_DEFAULT_INSTANCE,
                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new LeadingOnes_Dummy1(instance_id, dimension);
                }
            };
        }
    }
}
