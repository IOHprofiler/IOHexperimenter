#pragma once
#include "pbo_base.hpp"

namespace ioh {
    namespace problem {
        namespace pbo {
            class LABS : public pbo_base {
            public:
                /**
                       * \brief Construct a new LABS object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
                       * 
                       * \param instance_id The instance number of a problem, which controls the transformation
                       * performed on the original problem.
                       * \param dimension The dimensionality of the problem to created, 4 by default.
                       **/
                LABS(int instance_id = IOH_DEFAULT_INSTANCE,
                     int dimension = IOH_DEFAULT_DIMENSION)
                    : pbo_base(18, "LABS", instance_id) {
                    set_number_of_variables(dimension);
                }

                static double correlation(const std::vector<int> x, const int n,
                                          int k) {
                    int x1, x2;
                    auto result = 0.0;
                    for (auto i = 0; i < n - k; ++i) {
                        if (x[i] == 0) {
                            x1 = -1;
                        } else {
                            x1 = 1;
                        }
                        if (x[i + k] == 0) {
                            x2 = -1;
                        } else {
                            x2 = 1;
                        }
                        result += x1 * x2;
                    }
                    return result;
                }

                double internal_evaluate(const std::vector<int> &x) override {
                    const auto n = x.size();
                    auto result = 0.0;
                    for (auto k = 1; k != n; ++k) {
                        const auto cor = correlation(x, static_cast<int>(n), k);
                        result += cor * cor;
                    }
                    result = static_cast<double>(n * n) / 2.0 / result;
                    return static_cast<double>(result);
                }

                static LABS *create(int instance_id = IOH_DEFAULT_INSTANCE,
                                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new LABS(instance_id, dimension);
                }
            };
        }
    }
}
