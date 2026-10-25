#pragma once

#include "cec_problem.hpp"


namespace ioh::problem
{
    namespace cec2013
    {
        class EqualMaxima final : public CEC2013
        {
        public:
            //! The registry entries (see registry_entries_of): the dimension and the instance are fixed
            static std::vector<RegistryEntry<EqualMaxima>> registry_entries();

        protected:
            double inner_evaluate(const std::vector<double> &z) override
            {
                return std::pow(std::sin(5.0 * IOH_PI * z[0]), 6);
            }

        public:
            EqualMaxima(const int instance, const int n_variables) :
                CEC2013(1102, instance, n_variables, "CEC2013EqualMaxima", 0, 1, {
                    {{0.1}, 1.0},
                    {{0.3}, 1.0},
                    {{0.5}, 1.0},
                    {{0.7}, 1.0},
                    {{0.9}, 1.0},
                })
            {
            }
        };
    } // namespace cec2013


    inline std::vector<RegistryEntry<cec2013::EqualMaxima>> cec2013::EqualMaxima::registry_entries()
    {
        return {{1102, "EqualMaxima1102", [](int, int) { return std::make_shared<cec2013::EqualMaxima>(1, 1); }}};
    }

} // namespace ioh::problem