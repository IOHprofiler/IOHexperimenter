#pragma once

#include "cec_problem.hpp"


namespace ioh::problem
{
    namespace cec2013
    {
        class Vincent final : public CEC2013Problem<Vincent>
        {
        protected:
            double evaluate(const std::vector<double> &x) override
            {
                double result = 0;

                for (const auto &xi : x)
                    result += std::sin(10 * std::log(xi)) / static_cast<double>(x.size());

                return result;
            }

        public:
            Vincent(const int problem_id, const std::string &name, const int n_variables) :
                CEC2013Problem(problem_id, 1, n_variables, name, 0.25, 10.0, static_cast<int>(std::pow(6, n_variables)))
            {
                optimum_.x = std::vector<double>(n_variables, 1.0/3.0);
                optimum_.y = evaluate(optimum_.x);
            }
        };
    } // namespace cec2013


    template <>
    inline InstanceBasedProblem::Constructors<cec2013::Vincent, int, int>
    InstanceBasedProblem::load_instances<cec2013::Vincent>(const std::optional<fs::path> &)
    {
        return {{[](int, int) { return cec2013::Vincent(1107, "CEC2013Vincent2D", 2); }, 1107, std::nullopt},
                {[](int, int) { return cec2013::Vincent(1109, "CEC2013Vincent3D", 3); }, 1109, std::nullopt}
        };
    }

} // namespace ioh::problem