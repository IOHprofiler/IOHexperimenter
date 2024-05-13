#pragma once

#include "double_funnel.hpp"

namespace ioh::problem::funnel
{
    class DoubleRastrigin final : public DoubleFunnel
    {
    protected:
        double evaluate(const std::vector<double> &x) override
        {
            double f_rastrigin = 0.0;
            for (const double xi : x)
                f_rastrigin += 1 - std::cos(2.0 * IOH_PI * (xi - u1()));

            return functions::double_sphere(x, u1(), u2(), s(), d()) + (10 * f_rastrigin);
        }

    public:
        /*
         * @brief Construct a new DoubleSphere object
         * @param n_variables the dimension of the problem
         * @param d the depth of the suboptimal basin (higher values decrease the height)
         * @param s the size of the suboptimal basin (smaller values increase the size of the suboptimal basin)
         */
        DoubleRastrigin(const int n_variables, const double d = 0.0, const double s = 1.0) :
            DoubleFunnel(n_variables, "DoubleRastrigin", d, s)
        {
        }
    };
} // namespace ioh::problem::funnel
