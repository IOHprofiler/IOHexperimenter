#pragma once

#include "cec_problem.hpp"


namespace ioh::problem
{
    namespace cec2013
    {
        namespace vincent {
            inline std::vector<Solution<double, SingleObjective>> get_optima(const int dimension) 
            {
                const static std::array<double, 6> coords {
                    std::exp((1.0 / 20.0) * (IOH_PI + 4.0 * IOH_PI * -2)),
                    std::exp((1.0 / 20.0) * (IOH_PI + 4.0 * IOH_PI * -1)),
                    std::exp((1.0 / 20.0) * (IOH_PI + 4.0 * IOH_PI * -0)),
                    std::exp((1.0 / 20.0) * (IOH_PI + 4.0 * IOH_PI * 1)),
                    std::exp((1.0 / 20.0) * (IOH_PI + 4.0 * IOH_PI * 2)),
                    std::exp((1.0 / 20.0) * (IOH_PI + 4.0 * IOH_PI * 3)),
                };
                
                std::vector<Solution<double, SingleObjective>> result;
                if (dimension == 2)
                {
                    for (const auto ci : coords)
                        for (const auto cj : coords)
                            result.emplace_back(std::vector<double>({ci, cj}), 1.0);
                }
                else if (dimension == 3)
                {
                    for (const auto ci : coords)
                        for (const auto cj : coords)
                            for (const auto ck : coords)
                                result.emplace_back(std::vector<double>({ci, cj, ck}), 1.0);
                }
                return result;
            }
        }
        
        class Vincent final : public CEC2013Problem<Vincent>
        {
        protected:
            double inner_evaluate(const std::vector<double> &x) override
            {
                double result = 0;

                for (const auto &xi : x)
                    result += std::sin(10 * std::log(xi)) / static_cast<double>(x.size());

                return result;
            }

        public:
            Vincent(const int problem_id, const std::string &name, const int n_variables) :
                CEC2013Problem(problem_id, 1, n_variables, name, 0.25, 10.0, vincent::get_optima(n_variables), 0.19)
            {
            }
        };
    } // namespace cec2013


    template <>
    inline InstanceBasedProblem::Constructors<cec2013::Vincent, int, int>
    InstanceBasedProblem::load_instances<cec2013::Vincent>(const std::optional<fs::path> &)
    {
        return {{[](int, int) { return cec2013::Vincent(1107, "CEC2013Vincent2D", 2); }, 1107, std::nullopt},
                {[](int, int) { return cec2013::Vincent(1109, "CEC2013Vincent3D", 3); }, 1109, std::nullopt}
        };
    }

} // namespace ioh::problem