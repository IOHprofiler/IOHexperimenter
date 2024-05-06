#pragma once

#include "ioh/problem/single.hpp"


namespace ioh::problem::funnel
{
    namespace functions
    {
        inline double shifted_sum(const std::vector<double> &x, const double u)
        {
            return std::accumulate(x.begin(), x.end(), 0.0,
                                   [u](const double s, const double xi) { return s + pow(xi - u, 2.0); });
        }

        inline double double_sphere(const std::vector<double> &x, const double u1, const double u2, const double s,
                                    const double d)
        {
            return std::min(shifted_sum(x, u1), (d * static_cast<double>(x.size())) + s * shifted_sum(x, u2));
        }
    } // namespace functions

    class DoubleFunnel : public RealSingleObjective
    {
        double d_;
        double s_;
        double u1_;
        double u2_;

    public:
        /*
         * @brief Construct a new DoubleSphere object
         * @param n_variables the dimension of the problem
         * @param name the name of the problem
         * @param d the depth of the suboptimal basin (higher values decrease the height)
         * @param s the size of the suboptimal basin (smaller values increase the size of the suboptimal basin)
         */
        DoubleFunnel(const int n_variables, const std::string &name, const double d = 0.0, const double s = 1.0) :
            RealSingleObjective(MetaData(0, 1, name, n_variables), Bounds<double>(n_variables, -5, 5), {},
                                {std::vector<double>(n_variables, 2.5), 0.0}),
            d_(d), s_(s), u1_(2.5), u2_(-std::sqrt((std::pow(2.5, 2.0) - d) / s))
        {
        }

        [[nodiscard]] double d() const { return d_; }
        [[nodiscard]] double s() const { return s_; }
        [[nodiscard]] double u1() const { return u1_; }
        [[nodiscard]] double u2() const { return u2_; }
    };

} // namespace ioh::problem::funnel