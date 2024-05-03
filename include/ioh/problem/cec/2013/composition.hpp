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
                double maxi = 0.0;
                size_t max_index(0);

                for (size_t i = 0; i < n_optima; ++i)
                {
                    double sum = 0.0;
                    for (size_t j = 0; j < x.size(); ++j)
                        sum += (x[j] - optima[i].x[j]) * (x[j] - optima[i].x[j]);

                    weight[i] = exp(-sum / (2.0 * static_cast<double>(x.size()) * sigmas[i] * sigmas[i]));

                    if (weight[i] > maxi)
                    {
                        maxi = weight[i];
                        max_index = i;
                    }
                }

                double sum = 0.0;
                for (size_t i = 0; i < n_optima; ++i)
                {
                    if (i != max_index)
                        weight[i] *= (1.0 - pow(maxi, 10.0));
                    sum += weight[i];
                }
                for (size_t i = 0; i < n_optima; ++i)
                    weight[i] = sum == 0.0 ? 1.0 / static_cast<double>(n_optima) : weight[i] / sum;
            }

            [[nodiscard]] std::vector<double> transform_to_z(const std::vector<double> &x, const size_t index,
                                                             const bool shift = true) const
            {
                auto z = x;
                auto tmp = x;

                for (size_t i = 0; i < z.size(); ++i)
                {
                    const double xi = x[i] - (optima[index].x[i] * shift);
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

        protected:
            double inner_evaluate(const std::vector<double> &x) override
            {
                calculate_weights(x);

                double result = 0;
                for (size_t i = 0; i < n_optima; i++)
                {
                    const auto z = transform_to_z(x, i);
                    const auto fi = functions[i](z);

                    result += weight[i] * (2000.0 * fi / f_max[i]);
                }
                return -result;
            }

        public:
            CompositionFunction(const int problem_id, const std::string &name, const int n_variables,
                                const std::vector<Solution<double, SingleObjective>> &opts,
                                const std::vector<std::vector<std::vector<double>>> &transformations,
                                const std::vector<RealFunction> &functions, const std::vector<double> &sigmas,
                                const std::vector<double> &lambdas) :
                CEC2013Problem(problem_id, 1, n_variables, name, -5.0, 5.0, opts),
                transformations(transformations), functions(functions), sigmas(sigmas), lambdas(lambdas),
                weight(opts.size(), 0.0), f_max(n_optima)
            {
                const auto x5 = std::vector<double>(meta_data().n_variables, 5.);
                for (size_t i = 0; i < n_optima; ++i)
                {
                    const auto z = transform_to_z(x5, i, false);
                    f_max[i] = functions[i](z);
                }
            }

            std::vector<std::vector<std::vector<double>>> transformations;
            std::vector<RealFunction> functions;
            std::vector<double> sigmas;
            std::vector<double> lambdas;
            std::vector<double> weight;
            std::vector<double> f_max;
        };
    } // namespace cec2013

    inline std::vector<Solution<double, SingleObjective>> select_optima(std::vector<std::vector<double>> optima,
                                                                        const size_t n_optima, const size_t dimension)
    {
        std::vector<Solution<double, SingleObjective>> result;
        optima.resize(n_optima);
        for (auto &opt : optima)
        {
            opt.resize(dimension);
            result.emplace_back(opt, 0.0);
        }
        return result;
    }

    inline std::vector<std::vector<std::vector<double>>> get_identity_matrices(const size_t n_optima,
                                                                               const size_t dimension)
    {
        std::vector<std::vector<std::vector<double>>> eyes(n_optima);
        for (size_t i = 0; i < n_optima; i++)
            eyes[i] = common::eye(dimension);
        return eyes;
    }

    inline std::vector<std::vector<std::vector<double>>>
    load_transformations(const size_t cf_id, const size_t dimension, const size_t n_optima)
    {
        if (cf_id < 3)
            return get_identity_matrices(n_optima, dimension);

        const auto file_name = common::file::utils::find_static_file(
            fmt::format("{}CF{}_M_D{}.dat", cec2013::DATA_FOLDER, cf_id, dimension));

        const auto data = common::file::as_numeric_vector<double>(file_name);

        const size_t matrix_size = dimension * dimension;

        std::vector<std::vector<std::vector<double>>> transformations;
        for (size_t i = 0; i < data.size() / matrix_size; i++)
        {
            const size_t start = i * matrix_size;
            const size_t end = start + matrix_size;
            transformations.push_back(common::to_matrix(std::vector<double>(data.begin() + start, data.begin() + end)));
        }
        return transformations;
    }

    inline InstanceBasedProblem::ConstructorTuple<cec2013::CompositionFunction, int, int>
    load_cf(const int problem_id, const int cf_id, const size_t dimension,
            const std::vector<std::vector<double>> &all_optima, const std::vector<double> &sigmas,
            const std::vector<double> &lambdas, const std::vector<RealFunction> &functions

    )
    {
        const size_t n_optima = sigmas.size();
        return {[=](int, int) {
                    const auto transformations = load_transformations(cf_id, dimension, n_optima);
                    const auto optima = select_optima(all_optima, n_optima, dimension);

                    return cec2013::CompositionFunction(problem_id, fmt::format("CEC2013CF{}D{}", cf_id, dimension),
                                                        static_cast<int>(dimension), optima, transformations, functions,
                                                        sigmas, lambdas);
                },
                problem_id, std::nullopt};
    }


    template <>
    inline InstanceBasedProblem::Constructors<cec2013::CompositionFunction, int, int>
    InstanceBasedProblem::load_instances<cec2013::CompositionFunction>(const std::optional<fs::path> &)
    {
        const static auto optima_file =
            common::file::utils::find_static_file(fmt::format("{}optima.dat", cec2013::DATA_FOLDER));

        // if (!fs::is_regular_file(optima_file))
            // return {};

        const auto optima = common::to_matrix(common::file::as_numeric_vector<double>(optima_file), 10, 100);

        const std::vector<double> cf1_lambdas = {1.0, 1.0, 8.0, 8.0, 1.0 / 5.0, 1.0 / 5.0};
        const std::vector<RealFunction> cf1_functions{griewank, griewank, weierstrass, weierstrass, sphere, sphere};

        const std::vector<double> cf2_lambdas = {1.0, 1.0, 10.0, 10.0, 1.0 / 10.0, 1.0 / 10.0, 1.0 / 7.0, 1.0 / 7.0};
        const std::vector<RealFunction> cf2_functions{rastrigin, rastrigin, weierstrass, weierstrass,
                                                      griewank,  griewank,  sphere,      sphere};

        const std::vector<double> cf3_sigmas = {1.0, 1.0, 2.0, 2.0, 2.0, 2.0};
        const std::vector<double> cf3_lambdas = {1.0 / 4.0, 1.0 / 10.0, 2.0, 1.0, 2.0, 5.0};
        const std::vector<RealFunction> cf3_functions = {fef8_f2,     fef8_f2,  weierstrass,
                                                         weierstrass, griewank, griewank};

        const std::vector<double> cf4_sigmas = {1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 2.0};
        const std::vector<double> cf4_lambdas = {4.0, 1.0, 4.0, 1.0, 1.0 / 10.0, 1.0 / 5.0, 1.0 / 10.0, 1.0 / 40.0};
        const std::vector<RealFunction> cf4_functions = {rastrigin,   rastrigin,   fef8_f2,  fef8_f2,
                                                         weierstrass, weierstrass, griewank, griewank};


        return {
            load_cf(1111, 1, 2, optima, std::vector<double>(6, 1.0), cf1_lambdas, cf1_functions),
            load_cf(1112, 2, 2, optima, std::vector<double>(8, 1.0), cf2_lambdas, cf2_functions),
            load_cf(1113, 3, 2, optima, cf3_sigmas, cf3_lambdas, cf3_functions),
            load_cf(1114, 3, 3, optima, cf3_sigmas, cf3_lambdas, cf3_functions),
            load_cf(1115, 4, 3, optima, cf4_sigmas, cf4_lambdas, cf4_functions),
            load_cf(1116, 3, 5, optima, cf3_sigmas, cf3_lambdas, cf3_functions),
            load_cf(1117, 4, 5, optima, cf4_sigmas, cf4_lambdas, cf4_functions),
            load_cf(1118, 3, 10, optima, cf3_sigmas, cf3_lambdas, cf3_functions),
            load_cf(1119, 4, 10, optima, cf4_sigmas, cf4_lambdas, cf4_functions),
            load_cf(1120, 4, 20, optima, cf4_sigmas, cf4_lambdas, cf4_functions),
        };
    }
} // namespace ioh::problem