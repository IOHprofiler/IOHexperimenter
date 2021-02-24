#pragma once
#include "bbob_base.hpp"


namespace ioh {
    namespace problem {
        namespace bbob {
            class Step_Ellipsoid : public bbob_base {
                std::vector<double> datax_;
                std::vector<double> dataxx_;

            public:
                Step_Ellipsoid(int instance_id = IOH_DEFAULT_INSTANCE,
                               int dimension = IOH_DEFAULT_DIMENSION)
                    : bbob_base(7, "Step_Ellipsoid", instance_id, dimension),
                      datax_(dimension),
                      dataxx_(dimension) {
                    set_number_of_variables(dimension);
                }

                void prepare_problem() override {
                    transformation::coco::bbob2009_compute_xopt(
                        xopt_, rseed_, n_);
                    transformation::coco::bbob2009_compute_rotation(
                        rot1_, rseed_ + 1000000, n_);
                    transformation::coco::bbob2009_compute_rotation(
                        rot2_, rseed_, n_);
                    set_best_variables(xopt_);
                }


                double internal_evaluate(
                    const std::vector<double> &x) override {
                    using namespace transformation::coco;
                    static const double condition = 100;
                    static const auto alpha = 10.0;
                    int i, j;
                    auto penalty = 0.0;
                    std::vector<double> result(1);

                    for (i = 0; i < n_; ++i) {
                        const auto tmp = fabs(x[i]) - 5.0;
                        if (tmp > 0.0)
                            penalty += tmp * tmp;
                    }

                    for (i = 0; i < n_; ++i) {
                        datax_[i] = 0.0;
                        const auto c1 = sqrt(pow(condition / 10.,
                                                 static_cast<double>(i) /
                                                 static_cast<double>(n_ - 1
                                                 )));
                        for (j = 0; j < n_; ++j) {
                            datax_[i] += c1 * rot2_[i][j] * (x[j] - xopt_[j]);
                        }
                    }
                    const auto x1 = datax_[0];

                    for (i = 0; i < n_; ++i) {
                        if (fabs(datax_[i]) > 0.5)
                            /* TODO: Documentation: no fabs() in documentation */
                            datax_[i] = static_cast<double>(floor(
                                datax_[i] + 0.5));
                        else
                            datax_[i] = static_cast<double>(floor(
                                            alpha * datax_[i] + 0.5)) / alpha;
                    }

                    for (i = 0; i < n_; ++i) {
                        dataxx_[i] = 0.0;
                        for (j = 0; j < n_; ++j) {
                            dataxx_[i] += rot1_[i][j] * datax_[j];
                        }
                    }

                    /* Computation core */
                    result[0] = 0.0;
                    for (i = 0; i < n_; ++i) {
                        const auto exponent =
                            static_cast<double>(static_cast<long>(i)) / (
                                static_cast<double>(static_cast<long>(n_
                                    )
                                ) - 1.0);
                        result[0] += pow(condition, exponent) * dataxx_[i] *
                            dataxx_[i];
                    }
                    result[0] = 0.1 * (fabs(x1) * 1.0e-4 > result[0]
                                           ? fabs(x1) * 1.0e-4
                                           : result[0]) + penalty +
                                fopt_;

                    return result[0];
                }


                void objectives_transformation(const std::vector<double> &x,
                                               std::vector<double> &y,
                                               const int transformation_id,
                                               const int instance_id) override {
                    // Needs to override default beviour
                }

                static Step_Ellipsoid *create(
                    int instance_id = IOH_DEFAULT_INSTANCE,
                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new Step_Ellipsoid(instance_id, dimension);
                }
            };
        }
    }
}
