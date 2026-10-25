#pragma once

#include "cec_problem.hpp"


namespace ioh::problem
{
    namespace cec2013
    {

        class SixHumpCamelback final : public CEC2013
        {
        public:
            //! The registry entries (see registry_entries_of): the dimension and the instance are fixed
            static std::vector<RegistryEntry<SixHumpCamelback>> registry_entries();

        protected:
            double inner_evaluate(const std::vector<double> &x) override
            {
                const double x2 = std::pow(x[0], 2);
                const double x4 = std::pow(x[0], 4);
                const double y2 = std::pow(x[1], 2);
                const double expr1 = (4.0 - 2.1 * x2 + x4 / 3.0) * x2;
                const double expr2 = x[0] * x[1];
                const double expr3 = (4.0 * y2 - 4.0) * y2;
                return -1.0 * (expr1 + expr2 + expr3);
            }

        public:
            SixHumpCamelback(const int instance, const int n_variables) :
                CEC2013(1105, instance, n_variables, "CEC2013SixHumpCamelback", -1.9, 1.9, {
                    {{0.0898, -0.7126}, 1.03162842},
                    {{-0.0898, 0.7126}, 1.03162842},
                }, .5)
            {
            }
        };
    } // namespace cec2013


    inline std::vector<RegistryEntry<cec2013::SixHumpCamelback>> cec2013::SixHumpCamelback::registry_entries()
    {
        return {{1105, "SixHumpCamelback1105", [](int, int) { return std::make_shared<cec2013::SixHumpCamelback>(1, 2); }}};
    }

} // namespace ioh::problem