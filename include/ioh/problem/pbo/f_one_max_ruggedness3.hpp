#pragma once
#include "pbo_base.hpp"
#include "ioh/problem/utils.hpp"

namespace ioh {
    namespace problem {
        namespace pbo {
            class OneMax_Ruggedness3 : public pbo_base {
            public:
                std::vector<double> info;
                /**
                       * \brief Construct a new OneMax_Ruggedness3 object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
                       * 
                       * \param instance_id The instance number of a problem, which controls the transformation
                       * performed on the original problem.
                       * \param dimension The dimensionality of the problem to created, 4 by default.
                       **/
                OneMax_Ruggedness3(int instance_id = IOH_DEFAULT_INSTANCE,
                                   int dimension = IOH_DEFAULT_DIMENSION)
                    : pbo_base(10, "OneMax_Ruggedness3", instance_id) {
                    set_best_variables(1);
                    set_number_of_variables(dimension);
                }

                void prepare_problem() override {
                    info = utils::ruggedness3(get_number_of_variables());
                }

                double internal_evaluate(const std::vector<int> &x) override {
                    const auto n = x.size();
                    auto result = 0;
                    for (auto i = 0; i != n; ++i) {
                        result += x[i];
                    }
                    return this->info[static_cast<int>(result + 0.5)];
                }

                static OneMax_Ruggedness3 *create(
                    int instance_id = IOH_DEFAULT_INSTANCE,
                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new OneMax_Ruggedness3(instance_id, dimension);
                }
            };
        }
    }
}
