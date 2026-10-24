#pragma once

#include "cec_problem.hpp"


namespace ioh::problem
{
    namespace cec2013
    {
        namespace shubert
        {
            inline double shubert(const std::vector<double> &x)
            {
                double result = 1;
                for (const double &xi : x)
                {
                    double soma = 0.0;
                    for (int j = 1; j < 6; j++)
                        soma = soma + (j * std::cos((j + 1) * xi + j));
                    result *= soma;
                }
                return -result;
            }

            inline std::vector<Solution<double, SingleObjective>> get_optima(const int dimension)
            {
                constexpr std::array<double, 6> coords = {-7.08350641, -0.8003211,  5.48286421,
                                                                -7.70831374, -1.42512843, 4.85805688};
                

                std::vector<Solution<double, SingleObjective>> result;
                if (dimension == 2)
                {
                    for (const auto ci : coords)
                    {
                        for (const auto cj : coords)
                        {
                            const std::vector<double> x0 = {ci, cj};
                            const double y0 = shubert(x0);
                            if (y0 > 186.5)
                                result.emplace_back(x0, y0);
                        }
                    }
                }
                else if (dimension == 3)
                {
                    for (const auto ci : coords)
                    {
                        for (const auto cj : coords)
                        {
                            for (const auto ck : coords)
                            {
                                const std::vector<double> x0 = {ci, cj, ck};
                                const double y0 = shubert(x0);
                                if (y0 > 2709.0)
                                    result.emplace_back(x0, y0);
                            }
                        }
                    }
                }
                assert(static_cast<size_t>(dimension * std::pow(3, dimension)) == result.size());
                return result;
            }
        } // namespace shubert

        class Shubert final : public CEC2013
        {
        public:
            //! The registry entries (see registry_entries_of): the dimension and the instance are fixed
            static std::vector<RegistryEntry<Shubert>> registry_entries();

        protected:
            double inner_evaluate(const std::vector<double> &x) override { return shubert::shubert(x); }

        public:
            Shubert(const int problem_id, const std::string &name, const int n_variables, const std::vector<Solution<double, SingleObjective>>& opts) :
                CEC2013(problem_id, 1, n_variables, name, -10.0, 10.0, opts, 0.5)
            {
            }
        };
    } // namespace cec2013


    inline std::vector<RegistryEntry<cec2013::Shubert>> cec2013::Shubert::registry_entries()
    {
        return {{1106, "Shubert1106", [](int, int) { return std::make_shared<cec2013::Shubert>(1106, "CEC2013Shubert2D", 2, cec2013::shubert::get_optima(2)); }},
                {1108, "Shubert1108", [](int, int) { return std::make_shared<cec2013::Shubert>(1108, "CEC2013Shubert3D", 3, cec2013::shubert::get_optima(3)); }}};
    }

} // namespace ioh::problem