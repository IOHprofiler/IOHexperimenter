#pragma once

#include "cec_problem.hpp"


namespace ioh::problem
{
    namespace cec2013
    {

        class Himmelblau final : public CEC2013
        {
        public:
            //! The registry entries (see registry_entries_of): the dimension and the instance are fixed
            static std::vector<RegistryEntry<Himmelblau>> registry_entries();

        protected:
            double inner_evaluate(const std::vector<double> &x) override
            {
                return 200 - std::pow(std::pow(x[0], 2) + x[1] - 11, 2) - std::pow(x[0] + std::pow(x[1], 2) - 7, 2);

            }

        public:
            Himmelblau(const int instance, const int n_variables) :
                CEC2013(1104, instance, n_variables, "CEC2013Himmelblau", -6, 6, {
                    {{3, 2}, 200.0},
                    {{-2.805118, 3.131312}, 200.0},
                    {{-3.779310, -3.283186}, 200.0},
                    {{3.584428, -1.848126}, 200.0},
                })
            {
            }
        };
    } // namespace cec2013


    inline std::vector<RegistryEntry<cec2013::Himmelblau>> cec2013::Himmelblau::registry_entries()
    {
        return {{1104, "Himmelblau1104", [](int, int) { return std::make_shared<cec2013::Himmelblau>(1, 2); }}};
    }

} // namespace ioh::problem