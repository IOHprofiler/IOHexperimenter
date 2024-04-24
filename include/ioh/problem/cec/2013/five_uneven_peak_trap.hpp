#pragma once

#include "cec_problem.hpp"


namespace ioh::problem
{
    namespace cec2013
    {

        class FivePeaks final : public CEC2013Problem<FivePeaks>
        {
        protected:
            double inner_evaluate(const std::vector<double> &z) override
            {
                const double x = z[0];
                if (x >= 0 and x < 2.50)
                    return 80 * (2.5 - x);
                if (x >= 2.5 and x < 5)
                    return 64 * (x - 2.5);
                if (x >= 5.0 and x < 7.5)
                    return 64 * (7.5 - x);
                if (x >= 7.5 and x < 12.5)
                    return 28 * (x - 7.5);
                if (x >= 12.5 and x < 17.5)
                    return 28 * (17.5 - x);
                if (x >= 17.5 and x < 22.5)
                    return 32 * (x - 17.5);
                if (x >= 22.5 and x < 27.5)
                    return 32 * (27.5 - x);
                if (x >= 27.5 and x <= 30)
                    return 80 * (x - 27.5);
                return std::numeric_limits<double>::signaling_NaN();
            }

        public:
            FivePeaks(const int instance, const int n_variables) :
                CEC2013Problem(1101, instance, n_variables, "CEC2013FivePeaks", 0, 30, {
                    {{0.}, 200.0},
                    {{30}, 200.0},
                })
            {
            }
        };
    } // namespace cec2013


    template <>
    inline InstanceBasedProblem::Constructors<cec2013::FivePeaks, int, int>
    InstanceBasedProblem::load_instances<cec2013::FivePeaks>(const std::optional<fs::path> &)
    {
        return {{[](int, int) { return cec2013::FivePeaks(1, 1); }, 1101, std::nullopt}};
    }


} // namespace ioh::problem