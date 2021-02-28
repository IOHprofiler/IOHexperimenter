#pragma once
#include "pbo_base.hpp"

namespace ioh {
    namespace problem {
        namespace pbo {
            class Concatenated_Trap : public pbo_base {
                int k = 5;
            public:
                /**
               * \brief Construct a new Concatenated_Trap object. Definition refers to https://doi.org/10.1007/978-3-030-58115-2_49 
               * 
               * \param instance_id The instance number of a problem, which controls the transformation
               * performed on the original problem.
               * \param dimension The dimensionality of the problem to created, 4 by default.
               **/
                Concatenated_Trap(int instance_id = IOH_DEFAULT_INSTANCE,
                                  int dimension = IOH_DEFAULT_DIMENSION)
                    : pbo_base(24, "Concatenated_Trap", instance_id, dimension) {
                    set_best_variables(1);
                    set_number_of_variables(dimension);
                    set_optimal(static_cast<double>(dimension));
                }

                double internal_evaluate(const std::vector<int> &x) override {
                    auto result = 0.0;
                    double block_result;

                    const auto m = n_ / k;
                    for (auto i = 1; i <= m; ++i) {
                        block_result = 0.0;
                        for (auto j = i * k - k; j != i * k; ++j) {
                            block_result += x[j];
                        }
                        if (block_result == k) {
                            result += 1;
                        } else {
                            result += (static_cast<double>(k - 1) - block_result
                            ) / static_cast<
                                double>(k);
                        }
                    }
                    const auto remain_k = n_ - m * k;
                    if (remain_k != 0) {
                        block_result = 0.0;
                        // TODO: check this, only with braces this works
                        for (auto j = m * (k - 1); j != n_; ++j) {
                            block_result += x[j];
                        }
                        if (block_result == remain_k) {
                            result += 1;
                        } else {
                            result += static_cast<double>(
                                    remain_k - 1 - block_result) / static_cast
                                <double>(remain_k);
                        }
                    }

                    return result;
                }

                static Concatenated_Trap *create(
                    int instance_id = IOH_DEFAULT_INSTANCE,
                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new Concatenated_Trap(instance_id, dimension);
                }
            };
        }
    }
}
