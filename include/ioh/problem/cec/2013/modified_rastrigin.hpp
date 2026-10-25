#pragma once

#include "cec_problem.hpp"


namespace ioh::problem
{
    namespace cec2013
    {

        class ModifiedRastrigin final : public CEC2013
        {
        public:
            //! The registry entries (see registry_entries_of): the dimension and the instance are fixed
            static std::vector<RegistryEntry<ModifiedRastrigin>> registry_entries();

        protected:
            double inner_evaluate(const std::vector<double> &x) override
            {
                return -((10.0 + 9.0 * std::cos(2.0 * IOH_PI * 3.0 * x[0])) +
                         (10.0 + 9.0 * std::cos(2.0 * IOH_PI * 4.0 * x[1])));
            }

        public:
            ModifiedRastrigin(const int instance, const int n_variables) :
                CEC2013(1110, instance, n_variables, "CEC2013ModifiedRastrigin", 0, 1, {
                    {{(1.0 / 3.0) - (1.0 / 6.0), 0.125}, -2.0},
                    {{(1.0 / 3.0) - (1.0 / 6.0), 0.375}, -2.0},
                    {{(1.0 / 3.0) - (1.0 / 6.0), 0.625}, -2.0},
                    {{(1.0 / 3.0) - (1.0 / 6.0), 0.875}, -2.0},
                    {{(2.0 / 3.0) - (1.0 / 6.0), 0.125}, -2.0},
                    {{(2.0 / 3.0) - (1.0 / 6.0), 0.375}, -2.0},
                    {{(2.0 / 3.0) - (1.0 / 6.0), 0.625}, -2.0},
                    {{(2.0 / 3.0) - (1.0 / 6.0), 0.875}, -2.0},
                    {{(3.0 / 3.0) - (1.0 / 6.0), 0.125}, -2.0},
                    {{(3.0 / 3.0) - (1.0 / 6.0), 0.375}, -2.0},
                    {{(3.0 / 3.0) - (1.0 / 6.0), 0.625}, -2.0},
                    {{(3.0 / 3.0) - (1.0 / 6.0), 0.875}, -2.0},
                })
            {
            }
        };
    } // namespace cec2013


    inline std::vector<RegistryEntry<cec2013::ModifiedRastrigin>> cec2013::ModifiedRastrigin::registry_entries()
    {
        return {{1110, "ModifiedRastrigin1110", [](int, int) { return std::make_shared<cec2013::ModifiedRastrigin>(1, 2); }}};
    }

} // namespace ioh::problem