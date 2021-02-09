#pragma once
#include <algorithm>

#include "bbob_base.hpp"


namespace ioh {
    namespace problem {
        namespace bbob {
            class Gallagher21 : public bbob_base {
                typedef struct f_gallagher_permutation_t {
                    double value;
                    size_t index;
                } F_GALLAGHER_PERMUTATION_T;

                static bool f_gallagher_compare_doubles(
                    const f_gallagher_permutation_t &a,
                    const f_gallagher_permutation_t &b) {
                    return a.value < b.value;
                }

                const int number_of_peaks = 21;
                const double maxcondition = 1000.;

                std::vector<std::vector<double>> rotation;
                std::vector<std::vector<double>> arr_scales;
                std::vector<std::vector<double>> x_local;
                std::vector<double> peak_values;

            public:
                Gallagher21(int instance_id = IOH_DEFAULT_INSTANCE,
                            int dimension = IOH_DEFAULT_DIMENSION)
                    : bbob_base(22, "Gallagher21", instance_id, dimension),
                      arr_scales(
                          std::vector<std::vector<double>>(number_of_peaks,
                              std::vector<double>(n_))),
                      x_local(
                          std::vector<std::vector<double>>(
                              n_, std::vector<double>(number_of_peaks))),
                      peak_values(std::vector<double>(number_of_peaks, 10)) {
                    set_number_of_variables(dimension);
                }

                void prepare_problem() override {

                    const auto f1 = 1.1, f2 = 9.1, b_ = 9.8, c = 4.9;

                    std::vector<double> random_numbers;
                    std::vector<double> arr_condition(
                        number_of_peaks, maxcondition);
                    std::vector<f_gallagher_permutation_t> rperm(
                        number_of_peaks - 1);

                    /* Allocate temporary storage and space for the rotation matrices */
                    transformation::coco::bbob2009_compute_rotation(
                        rotation, rseed_, n_);
                    transformation::coco::bbob2009_unif(
                        random_numbers, number_of_peaks - 1,
                        rseed_);

                    for (size_t i = 0; i < number_of_peaks - 1; ++i)
                        rperm[i] = {random_numbers[i], i};

                    std::sort(rperm.begin(), rperm.end(),
                              f_gallagher_compare_doubles);

                    /* Random permutation */
                    for (auto i = 1; i < number_of_peaks; ++i) {
                        const auto rank =
                            rperm[i - 1].index / static_cast<double>(
                                number_of_peaks - 2);
                        arr_condition[i] = pow(maxcondition, rank);
                        peak_values[i] =
                            static_cast<double>(i - 1) / static_cast<double>(
                                number_of_peaks - 2) * (f2 - f1) + f1;
                    }

                    rperm = std::vector<f_gallagher_permutation_t>(n_);
                    for (auto i = 0; i < number_of_peaks; ++i) {
                        transformation::coco::bbob2009_unif(random_numbers, n_,
                            rseed_ + static_cast<long>(1000 * i));
                        for (size_t j = 0; j < n_; ++j)
                            rperm[j] = {random_numbers[j], j};

                        std::sort(rperm.begin(), rperm.end(),
                                  f_gallagher_compare_doubles);
                        for (auto j = 0; j < n_; ++j)
                            arr_scales[i][j] = pow(arr_condition[i],
                                                   rperm[j].index / static_cast<
                                                       double>(n_ - 1) -
                                                   0.5);
                    }

                    transformation::coco::bbob2009_unif(
                        random_numbers, n_ * number_of_peaks,
                        rseed_);
                    std::vector<double> best_variables(n_);
                    for (auto i = 0; i < n_; ++i) {
                        xopt_[i] = 0.8 * (b_ * random_numbers[i] - c);
                        best_variables[i] = 0.8 * (b_ * random_numbers[i] - c);
                        for (auto j = 0; j < number_of_peaks; ++j) {
                            x_local[i][j] = 0.;
                            for (auto k = 0; k < n_; ++k)
                                x_local[i][j] += rotation[i][k] * (
                                    b_ * random_numbers[j * n_ + k] - c
                                );
                            if (j == 0)
                                x_local[i][j] *= 0.8;
                        }
                    }
                    set_best_variables(best_variables);
                }

                double internal_evaluate(
                    const std::vector<double> &x) override {
                    std::vector<double> tmx(n_, 0);
                    const auto a = 0.1;
                    auto f = 0., f_add = 0., f_true = 0.;
                    const auto fac = -0.5 / static_cast<double>(n_);

                    /* Boundary handling */
                    for (auto i = 0; i < n_; ++i) {
                        const auto tmp = fabs(x[i]) - 5.;
                        if (tmp > 0.)
                            f_add += tmp * tmp;
                    }

                    for (auto i = 0; i < n_; i++)
                        for (auto j = 0; j < n_; ++j)
                            tmx[i] += rotation[i][j] * x[j];

                    /* Computation core*/
                    for (auto i = 0; i < number_of_peaks; ++i) {
                        auto tmp2 = 0.;
                        for (auto j = 0; j < n_; ++j)
                            tmp2 += arr_scales[i][j] * pow(
                                tmx[j] - x_local[j][i], 2.0);

                        tmp2 = peak_values[i] * exp(fac * tmp2);
                        f = f > tmp2 ? f : tmp2;
                    }

                    f = 10. - f;
                    if (f > 0) {
                        f_true = log(f) / a;
                        f_true = pow(
                            exp(f_true + 0.49 * (sin(f_true) + sin(
                                                     0.79 * f_true))), a);
                    } else if (f < 0) {
                        f_true = log(-f) / a;
                        f_true = -pow(
                            exp(f_true + 0.49 * (sin(0.55 * f_true) + sin(
                                                     0.31 * f_true))), a);
                    } else {
                        f_true = f;
                    }

                    f_true *= f_true;
                    f_true += f_add;

                    return f_true;
                }


                static Gallagher21 *create(
                    int instance_id = IOH_DEFAULT_INSTANCE,
                    int dimension = IOH_DEFAULT_DIMENSION) {
                    return new Gallagher21(instance_id, dimension);
                }
            };
        }
    }
}
