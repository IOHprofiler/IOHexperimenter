#pragma once

#include "cec_problem.hpp"


namespace ioh::problem
{
    namespace cec2013
    {

        class Himmelblau final : public CEC2013Problem<Himmelblau>
        {
        protected:
            double evaluate(const std::vector<double> &x) override
            {
                return 200 - std::pow(std::pow(x[0], 2) + x[1] - 11, 2) - std::pow(x[0] + std::pow(x[1], 2) - 7, 2);

            }

        public:
            Himmelblau(const int instance, const int n_variables) :
                CEC2013Problem(1104, instance, n_variables, "CEC2013Himmelblau", -6, 6, 4)
            {
                optimum_.x = {3.0, 2.0};
                optimum_.y = {200.0};
            }
        };
    } // namespace cec2013


    template <>
    inline InstanceBasedProblem::Constructors<cec2013::Himmelblau, int, int>
    InstanceBasedProblem::load_instances<cec2013::Himmelblau>(const std::optional<fs::path> &definitions_file)
    {
        return {{[](int, int) { return cec2013::Himmelblau(1, 2); }, 1104, std::nullopt}};
    }

} // namespace ioh::problem