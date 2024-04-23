#pragma once

#include "cec_problem.hpp"


namespace ioh::problem
{
    namespace cec2013
    {

        class SixHumpCamelback final : public CEC2013Problem<SixHumpCamelback>
        {
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
                CEC2013Problem(1105, instance, n_variables, "CEC2013SixHumpCamelback", -1.9, 1.9, {
                    {{0.0898, -0.7126}, 1.03162842},
                    {{-0.0898, 0.7126}, 1.03162842},
                }, .5)
            {
            }
        };
    } // namespace cec2013


    template <>
    inline InstanceBasedProblem::Constructors<cec2013::SixHumpCamelback, int, int>
    InstanceBasedProblem::load_instances<cec2013::SixHumpCamelback>(const std::optional<fs::path> &)
    {
        return {{[](int, int) { return cec2013::SixHumpCamelback(1, 2); }, 1105, std::nullopt}};
    }

} // namespace ioh::problem