#pragma once
#include "pbo_base.hpp"
#include "ioh/problem/utils.hpp"

namespace ioh {
    namespace problem {
        namespace pbo {
            class OneMax_Epistasis : public pbo_base {
            public:
                /**
                       * \brief Construct a new OneMax_Epistasis object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
                       * 
                       * \param instance_id The instance number of a problem, which controls the transformation
                       * performed on the original problem.
                       * \param dimension The dimensionality of the problem to created, 4 by default.
                       **/
                OneMax_Epistasis(int instance_id = IOH_DEFAULT_INSTANCE,
                                 int dimension = IOH_DEFAULT_DIMENSION)
                    : pbo_base(7, "OneMax_Epistasis", instance_id) {
                    set_number_of_variables(dimension);
                }

                void customize_optimal() override {
                    set_optimal(get_number_of_variables());
                }

                double internal_evaluate(const std::vector<int> &x) override {
                    auto new_variables = utils::epistasis(x, 4);
                    const auto n = new_variables.size();
                    auto result = 0;
                    for (auto i = 0; i != n; ++i) {
                        result += new_variables[i];
                    }
                    return static_cast<double>(result);
                }

                static OneMax_Epistasis *create(
                    int instance_id = IOH_DEFAULT_INSTANCE,
                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new OneMax_Epistasis(instance_id, dimension);
                }
            };
        }
    }
}
