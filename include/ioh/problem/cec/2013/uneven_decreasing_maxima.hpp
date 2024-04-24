#pragma once

#include "cec_problem.hpp"


namespace ioh::problem
{
    namespace cec2013
    {

        class UnevenEqualMaxima final : public CEC2013Problem<UnevenEqualMaxima>
        {
        protected:
            double inner_evaluate(const std::vector<double> &z) override
            {
                return std::exp(-2.0 * std::log(2) * std::pow((z[0] - 0.08) / 0.854, 2)) *
                    std::pow(std::sin(5 * IOH_PI * (std::pow(z[0], 0.75) - 0.05)), 6);
            }

        public:
            UnevenEqualMaxima(const int instance, const int n_variables) :
                CEC2013Problem(1103, instance, n_variables, "CEC2013UnevenEqualMaxima", 0, 1, {
                    {{0.07969978}, 0.999999828},
                })
            {
            }

        };
    } // namespace cec2013


    template <>
    inline InstanceBasedProblem::Constructors<cec2013::UnevenEqualMaxima, int, int>
    InstanceBasedProblem::load_instances<cec2013::UnevenEqualMaxima>(const std::optional<fs::path> &)
    {
        return {{[](int, int) { return cec2013::UnevenEqualMaxima(1, 1); }, 1103, std::nullopt}};
    }

} // namespace ioh::problem