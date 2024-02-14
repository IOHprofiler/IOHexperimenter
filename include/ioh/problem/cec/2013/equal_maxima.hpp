#pragma once

#include "cec_problem.hpp"


namespace ioh::problem
{
    namespace cec2013
    {

        class EqualMaxima final : public CEC2013Problem<EqualMaxima>
        {
        protected:
            double evaluate(const std::vector<double> &z) override
            {
                return std::pow(std::sin(5.0 * IOH_PI * z[0]), 6);
            }

        public:
            EqualMaxima(const int instance, const int n_variables) :
                CEC2013Problem(1102, instance, n_variables, "CEC2013EqualMaxima", 0, 1, 5)
            {
                optimum_.x = {0.1};
                optimum_.y = {1.0};
            }
        };
    } // namespace cec2013


    template <>
    inline InstanceBasedProblem::Constructors<cec2013::EqualMaxima, int, int>
    InstanceBasedProblem::load_instances<cec2013::EqualMaxima>(const std::optional<fs::path> &)
    {
        return {{[](int, int) { return cec2013::EqualMaxima(1, 1); }, 1102, std::nullopt}};
    }

} // namespace ioh::problem