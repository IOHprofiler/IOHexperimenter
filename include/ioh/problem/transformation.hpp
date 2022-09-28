#pragma once

#include "ioh/problem/utils.hpp"
#include "ioh/problem/structures.hpp"

/* Transformation namespace */
namespace ioh::problem::transformation
{
    namespace objective
    {
        /**
         * \brief Exclusive or operation on two integers, which should either be 0 or 1
         * \param x1 first operand
         * \param x2 second operand
         * \return the result of xor
         */
        inline int exclusive_or(const int x1, const int x2) { return static_cast<int>(x1 != x2);}

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
            const auto scalar = common::random::pbo::uniform(1, seed, lb, ub).at(0);
            return t(y, scalar);
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
        inline double penalize(const std::vector<double> &x, const double lb, const double ub, const double factor,
                               const double y)
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

        ///**
        // * \brief penalizes the objective value for x when it is out of bounds, weighed by a constant factor
        // * \param x the object in the search space
        // * \param constraint the bounds of the problem
        // * \param factor the weight
        // * \param y the raw y value
        // * \return the penalized y value
        // */
        //template <typename T>
        //double penalize(const std::vector<double> &x, const BoxConstraint<T> constraint, const double factor,
        //                const double y)
        //{
        //    return penalize(x, static_cast<double>(constraint.lb.at(0)), static_cast<double>(constraint.ub.at(0)),
        //                    factor, y);
        //}
    }

    namespace variables
    {
        /**
         * \brief randomly flips a bit 
         * \param x raw variables
         * \param seed seed for the random flip
         */
        inline void random_flip(std::vector<int> &x, const int seed)
        {
            const auto n = static_cast<int>(x.size());
            const auto rx = common::random::pbo::uniform(n, seed);

            for (auto i = 0; i < n; ++i)
                x[i] = objective::exclusive_or(x.at(i), static_cast<int>(2.0 * floor(1e4 * rx.at(i)) / 1e4));
        }

        /**
         * \brief randomly reorder the elements from x
         * \param x raw variables
         * \param seed seed for the random flip
         */
        inline void random_reorder(std::vector<int> &x, const int seed)
        {
            std::vector<int> index(x.size());
            std::iota(index.begin(), index.end(), 0);

            const auto n = static_cast<int>(x.size());
            const auto rx = common::random::pbo::uniform(n, seed);
            const auto copy_x = x;

            for (auto i = 0; i != n; ++i)
            {
                const auto t = static_cast<int>(floor(rx.at(i) * n));
                const auto temp = index[0];
                index[0] = index[t];
                index[t] = temp;
            }
            for (auto i = 0; i < n; ++i)
                x[i] = copy_x.at(index[i]);
        }


        /**
         * \brief reset x from x_1 whose elements were randomly reordered from x
         * \param x_1 the reordered variables
         * \param seed seed for the random flip
         */
        inline std::vector<int> random_reorder_reset(const std::vector<int> &x_1, const int seed)
        {
            std::vector<int> x(x_1.size());
            std::vector<int> index(x_1.size());
            std::iota(index.begin(), index.end(), 0);

            const auto n = static_cast<int>(x_1.size());
            const auto rx = common::random::pbo::uniform(n, seed);

            for (auto i = 0; i != n; ++i)
            {
                const auto t = static_cast<int>(floor(rx.at(i) * n));
                const auto temp = index[0];
                index[0] = index[t];
                index[t] = temp;
            }
            for (auto i = 0; i < n; ++i)
                x[index[i]] = x_1.at(i);
            return x;
        }

        /**
         * \brief Affine transformation for x using matrix M and vector B
         * \param x raw variables
         * \param m transformation matrix
         * \param b transformation vector
         */
        inline void affine(std::vector<double> &x, const std::vector<std::vector<double>> &m,
                           const std::vector<double> &b)
        {
            auto temp_x = x;
            for (size_t i = 0; i < x.size(); ++i)
            {
                x[i] = b[i];
                for (size_t j = 0; j < x.size(); ++j)
                    x[i] += temp_x[j] * m[i][j];
            }
        }

        /**
         * \brief Asymmetric transformation scaled by beta
         * \param x raw variables
         * \param beta scale of the transformation
         */
        inline void asymmetric(std::vector<double> &x, const double beta)
        {
            const auto n_eff = static_cast<double>(x.size()) - 1.0;
            for (auto i = 0; i < static_cast<int>(x.size()); ++i)
                if (x[i] > 0.0)
                    x[i] = pow(x[i], 1.0 + beta * i / n_eff * sqrt(x[i]));
        }


        /**
         * \brief brs transformation on x
         * \param x raw variables
         */
        inline void brs(std::vector<double> &x)
        {
            const auto n_eff = static_cast<double>(x.size()) - 1.0;
            for (auto i = 0; i < static_cast<int>(x.size()); ++i)
            {
                auto factor = pow(sqrt(10.0), i / n_eff);
                if (x[i] > 0.0 && i % 2 == 0)
                    factor *= 10.0;
                x[i] = factor * x[i];
            }
        }

        /**
         * \brief conditioning transformation of x
         * \param x raw variables
         * \param alpha base of the transformation
         */
        inline void conditioning(std::vector<double> &x, const double alpha)
        {
            const auto n_eff = static_cast<double>(x.size()) - 1.0;
            for (auto i = 0; i < static_cast<int>(x.size()); ++i)
                x[i] = pow(alpha, 0.5 * i / n_eff) * x[i];
        }

        /**
         * \brief oscillate each variable in x
         * \param x raw variables
         * \param alpha  the factor of oscillation
         */
        inline void oscillate(std::vector<double> &x, const double alpha = 0.1)
        {
            for (auto &xi : x)
                xi = objective::oscillate(xi, alpha);
        }


        /**
         * \brief scale x by a scalar
         * \param x raw variables
         * \param scalar the factor to scale x by
         */
        inline void scale(std::vector<double> &x, const double scalar)
        {
            for (auto &xi : x)
                xi = scalar * xi;
        }

        /**
         * \brief subtract an offset from each xi
         * \param x raw variables
         * \param offset a vector of offsets for each xi
         */
        inline void subtract(std::vector<double> &x, const std::vector<double> &offset)
        {
            for (auto i = 0; i < static_cast<int>(x.size()); ++i)
                x[i] = x[i] - offset[i];
        }

        /**
         * \brief randomly reverse the sign for each xi
         * \param x raw variables
         * \param seed for generating the random vector
         */
        inline void random_sign_flip(std::vector<double> &x, const long seed)
        {
            const auto random_numbers = common::random::bbob2009::uniform(x.size(), seed);
            for (size_t i = 0; i < x.size(); ++i)
                if (random_numbers[i] < 0.5)
                    x[i] = -x[i];
        }

        /**
         * \brief transforms the raw variables using the distance to the optimum
         * \param x the raw variables
         * \param xopt the optimum
         */
        inline void z_hat(std::vector<double> &x, const std::vector<double> &xopt)
        {
            const auto temp_x = x;
            for (size_t i = 1; i < x.size(); ++i)
                x[i] = temp_x[i] + 0.25 * (temp_x[i - 1] - 2.0 * fabs(xopt[i - 1]));
        }
    }
}
