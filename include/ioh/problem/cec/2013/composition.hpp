#pragma once

#include "cec_problem.hpp"


namespace ioh::problem
{
    namespace cec2013
    {
        const inline std::string DATA_FOLDER = "cec_transformations/2013/";

        class CompositionFunction final : public CEC2013Problem<CompositionFunction>
        {
            void calculate_weights(const std::vector<double> &x)
            {
                double sum, maxi(-std::numeric_limits<double>::max());

                size_t max_index(0);

                for (size_t i = 0; i < n_optima; ++i)
                {
                    sum = 0.0;
                    for (size_t j = 0; j < x.size(); ++j)
                    {
                        sum += (x[j] - optima[i][j]) * (x[j] - optima[i][j]);
                    }
                    weight_[i] = exp(-sum / (2.0 * static_cast<double>(x.size()) * sigmas[i] * sigmas[i]));
                    if (i == 0)
                    {
                        maxi = weight_[i];
                    }
                    if (weight_[i] > maxi)
                    {
                        maxi = weight_[i];
                        max_index = i;
                    }
                }
                sum = 0.0;
                for (int i = 0; i < n_optima; ++i)
                {
                    // if (weight_[i] != maxi) {
                    if (i != max_index)
                    {
                        weight_[i] *= (1.0 - pow(maxi, 10.0));
                    }
                    sum += weight_[i];
                }
                for (int i = 0; i < n_optima; ++i)
                {
                    if (sum == 0.0)
                    {
                        weight_[i] = 1.0 / static_cast<double>(n_optima);
                    }
                    else
                    {
                        weight_[i] /= sum;
                    }
                }
            }

            std::vector<double> transform_to_z(const std::vector<double> &x, const size_t index, const bool shift = true) const
            {
                auto z = x;
                auto tmp = x;

                for (size_t i = 0; i < z.size(); ++i)
                {
                    const double xi = x[i] - (optima[index][i] * shift);
                    tmp[i] = xi / lambdas[index];
                }
                for (size_t i = 0; i < z.size(); ++i)
                {
                    z[i] = 0;
                    for (size_t j = 0; j < z.size(); ++j)
                    {
                        z[i] += transformations[index][j][i] * tmp[j];
                    }
                }
                return z;
            }

            std::vector<double> calculate_fmaxi()
            {
                const auto x5 = std::vector<double>(meta_data().n_variables, 5.);
                auto fmaxi = std::vector<double>(n_optima);

                for (size_t i = 0; i < n_optima; ++i)
                {
                    const auto z = transform_to_z(x5, i, false);
                    fmaxi[i] = functions[i](z);
                }
                return fmaxi;
            }

        protected:
            double evaluate(const std::vector<double> &x) override
            {
                static constexpr double C_ = 2000.0;
                calculate_weights(x);
                const auto fmax_i = calculate_fmaxi();


                double result = 0;
                for (size_t i = 0; i < n_optima; i++)
                {
                    const auto z = transform_to_z(x, i);
                    const auto fi = functions[i](z);

                    result += weight_[i] * (C_ * fi / fmax_i[i] + biases[i]);
                }

                return -result;
            }

        public:
            CompositionFunction(const int problem_id, const std::string &name, const int n_variables,
                                const std::vector<std::vector<double>> &optima,
                                const std::vector<std::vector<std::vector<double>>> &transformations,
                                const std::vector<RealFunction> &functions, const std::vector<double> &sigmas,
                                const std::vector<double> &biases, const std::vector<double> &weights,
                                const std::vector<double> &lambdas) :
                CEC2013Problem(problem_id, 1, n_variables, name, -5.0, 5.0, optima.size()),
                optima(optima), transformations(transformations), functions(functions), sigmas(sigmas), biases(biases),
                weights(weights), lambdas(lambdas), weight_(optima.size(), 0.0)
            {
            }

            std::vector<std::vector<double>> optima;
            std::vector<std::vector<std::vector<double>>> transformations;
            std::vector<RealFunction> functions;
            std::vector<double> sigmas;
            std::vector<double> biases;
            std::vector<double> weights;
            std::vector<double> lambdas;
            std::vector<double> weight_;
        };
    } // namespace cec2013


    template <>
    inline InstanceBasedProblem::Constructors<cec2013::CompositionFunction, int, int>
    InstanceBasedProblem::load_instances<cec2013::CompositionFunction>(const std::optional<fs::path> &definitions_file)
    {
        const static auto optima_file =
            common::file::utils::find_static_file(fmt::format("{}optima.dat", cec2013::DATA_FOLDER));

        auto optima = common::to_matrix(common::file::as_numeric_vector<double>(optima_file), 10, 100);
        optima.resize(6);
        for (auto &opt : optima)
            opt.resize(2);

        std::vector<std::vector<std::vector<double>>> transformations(6);
        for (size_t i = 0; i < 6; i++)
            transformations[i] = common::eye(2);

        return {{[optima, transformations](int, int) {
                     return cec2013::CompositionFunction(1111, "CEC2013CF1", 2, optima, transformations,
                                                         {griewank, griewank, weierstrass, weierstrass, sphere, sphere},
                                                         std::vector<double>(6, 1.0), std::vector<double>(6, 0.0),
                                                         std::vector<double>(6, 0.0),
                                                         {1.0, 1.0, 8.0, 8.0, 1.0 / 5.0, 1.0 / 5.0});
                 },
                 1111, std::nullopt}};
    }

} // namespace ioh::problem