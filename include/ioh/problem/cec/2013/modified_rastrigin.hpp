#pragma once

#include "cec_problem.hpp"


namespace ioh::problem
{
    namespace cec2013
    {

        class ModifiedRastrigin final : public CEC2013Problem<ModifiedRastrigin>
        {
        protected:
            double evaluate(const std::vector<double> &x) override
            {
                return -((10.0 + 9.0 * std::cos(2.0 * IOH_PI * 3.0 * x[0])) +
                         (10.0 + 9.0 * std::cos(2.0 * IOH_PI * 4.0 * x[1])));
            }

        public:
            ModifiedRastrigin(const int instance, const int n_variables) :
                CEC2013Problem(1110, instance, n_variables, "CEC2013ModifiedRastrigin", 0, 1, 12)
            {
                optimum_.x = {0.5, 0.375};
                optimum_.y = {-2.0};
            }
        };
    } // namespace cec2013


    template <>
    inline InstanceBasedProblem::Constructors<cec2013::ModifiedRastrigin, int, int>
    InstanceBasedProblem::load_instances<cec2013::ModifiedRastrigin>(const std::optional<fs::path> &definitions_file)
    {
        return {{[](int, int) { return cec2013::ModifiedRastrigin(1, 2); }, 1110, std::nullopt}};
    }

} // namespace ioh::problem