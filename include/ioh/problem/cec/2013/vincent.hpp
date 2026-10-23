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
        
        class Vincent final : public CEC2013
        {
        public:
            //! The registry entries (see registry_entries_of): the dimension and the instance are fixed
            static std::vector<RegistryEntry<Vincent>> registry_entries();

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
                CEC2013(problem_id, 1, n_variables, name, 0.25, 10.0, vincent::get_optima(n_variables), 0.19)
            {
            }
        };
    } // namespace cec2013


    inline std::vector<RegistryEntry<cec2013::Vincent>> cec2013::Vincent::registry_entries()
    {
        return {{1107, "Vincent1107", [](int, int) { return std::make_shared<cec2013::Vincent>(1107, "CEC2013Vincent2D", 2); }},
                {1109, "Vincent1109", [](int, int) { return std::make_shared<cec2013::Vincent>(1109, "CEC2013Vincent3D", 3); }}
        };
    }

} // namespace ioh::problem