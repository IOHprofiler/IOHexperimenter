#pragma once

#include "ioh/problem/utils.hpp"

namespace ioh::problem::transformation::objective
{
    /**
     * \brief Shift a double y with a given offset
     * \param y the raw value
     * \param offset the offset to shift by
     * \return the shifted objective value
     */
    inline double shift(const double y, const double offset) { return y + offset; }

    /**
     * \brief Scale a double y with a given offset
     * \param y the raw value
     * \param offset the offset to scale by
     * \return the scaled objective value
     */
    inline double scale(const double y, const double offset) { return y * offset; }

    /**
     * \brief power a double y with a given exponent
     * \param y the raw value
     * \param exponent the exponent
     * \return y^exponent
     */
    inline double power(const double y, const double exponent) { return pow(y, exponent); }

    /**
    * \brief oscillates the value for y
    * \param y the raw y value
    * \param factor the factor of oscillation
    * \return the transformed y value
    */
    inline double oscillate(const double y, const double factor = 0.1)
    {
        if (y != 0.0)
        {
            const auto log_y = log(fabs(y)) / factor;
            if (y > 0)
                return pow(exp(log_y + 0.49 * (sin(log_y) + sin(0.79 * log_y))), factor);
            return -pow(exp(log_y + 0.49 * (sin(0.55 * log_y) + sin(0.31 * log_y))), factor);
        }
        return y;
    }

    using Transformation = std::function<double(double, double)>;

    /**
     * \brief applies a given transformation method t(double y, double a) with a random number for a.
     * \param t A transformation method
     * \param y the raw y value
     * \param seed the seed for the uniform random number generator
     * \param lb the lower bound for the random number
     * \param ub the upper bound for the random number
     * \return the transformed y value
     */
    inline double uniform(const Transformation &t, const double y, const int seed, const double lb, const double ub)
    {
        auto scalar = std::vector<double>(1);
        common::Random::uniform(1, seed, scalar);
        scalar.at(0) = scalar.at(0) * 1e4 / 1e4 * ub - lb;
        return t(y, scalar.at(0));
    }

    /**
     * \brief penalizes the objective value for x when it is out of bounds, weighed by a constant factor
     * \param x the object in the search space
     * \param lb the lower bound
     * \param ub the upper bound
     * \param factor the weight
     * \param y the raw y value
     * \return the penalized y value
     */
    inline double penalize(const std::vector<double> &x, const double lb, const double ub,
                           const double factor, const double y)
    {
        auto penalty = 0.0;
        for (const auto xi : x)
        {
            const auto c1 = xi - ub;
            const auto c2 = lb - xi;
            if (c1 > 0.0)
                penalty += c1 * c1;
            else if (c2 > 0.0)
                penalty += c2 * c2;
        }
        return y + (factor * penalty);
    }

    /**
    * \brief penalizes the objective value for x when it is out of bounds, weighed by a constant factor
    * \param x the object in the search space
    * \param constraint the bounds of the problem
    * \param factor the weight
    * \param y the raw y value
    * \return the penalized y value
    */
    template <typename T>
    double penalize(const std::vector<double> &x, const Constraint<T> constraint, const double factor,
                    const double y)
    {
        return penalize(x, static_cast<double>(constraint.lb.at(0)), static_cast<double>(constraint.ub.at(0)), factor, y);
    }

} 
