#pragma once

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

#include "ioh/problem/transformation.hpp"

/**
 * @namespace ioh::problem
 * Namespace where the various problem functions are defined.
 */
namespace ioh::problem
{
    /**
     * @fn double rastrigin(const std::vector<double>& z)
     * @brief Computes the Rastrigin function for a given input vector.
     * @param z Input vector.
     * @return Function value at input z.
     */
    inline double rastrigin(const std::vector<double> &z)
    {
        return std::accumulate(z.begin(), z.end(), 0.0, [](const double acc, const double value) {
            return acc + (value * value - 10.0 * std::cos(2.0 * IOH_PI * value) + 10.0);
        });
    }

    /**
     * @brief Calculates the Rosenbrock function value for the input vector.
     *
     * The Rosenbrock function is commonly used for optimization algorithms testing. It is defined over an n-dimensional
     * space. The function is computed as the sum of squares of sequential elements, with a particular transformation
     * applied to each element before the computation.
     *
     * @param z A constant reference to a vector of doubles, which represents the input to the function.
     * @return The computed Rosenbrock function value.
     */
    inline double rosenbrock(const std::vector<double> &z)
    {
        std::vector<double> z_copy(z);
        double result = 0.0;
        z_copy[0] += 1.0;
        for (size_t i = 0; i < z_copy.size() - 1; ++i)
        {
            z_copy[i + 1] += 1.0;
            const auto tmp1 = z_copy[i] * z_copy[i] - z_copy[i + 1];
            const auto tmp2 = z_copy[i] - 1.0;
            result += 100.0 * tmp1 * tmp1 + tmp2 * tmp2;
        }
        return result;
    }

    /**
     * @brief Computes the Levy function value given an input vector.
     *
     * The Levy function is well known in the optimization domain, defined with a combination of sine and cosine
     * operations along with polynomial expressions. The function calculates various terms separately which contribute
     * to the final Levy function value.
     *
     * @param z A constant reference to a vector of doubles representing the input to the function.
     * @return The calculated Levy function value based on the input vector.
     */
    inline double levy(const std::vector<double> &z)
    {
        std::vector<double> w(z.size());

        for (size_t i = 0; i < z.size(); ++i)
        {
            w[i] = 1.0 + (z[i] - 0.0) / 4.0;
        }

        const double term1 = std::pow(std::sin(IOH_PI * w[0]), 2);
        const double term3 = std::pow((w.back() - 1), 2) * (1 + std::pow(std::sin(2 * IOH_PI * w.back()), 2));
        double sum = 0.0;
        for (size_t i = 0; i < z.size() - 1; ++i)
        {
            sum += std::pow((w[i] - 1), 2) * (1 + 10 * std::pow(std::sin(IOH_PI * w[i] + 1), 2));
        }
        return term1 + sum + term3;
    }

    /**
     * @brief Calculates the Zakharov function value for a given input vector.
     *
     * The Zakharov function is used frequently in optimization problems. It involves calculating two summation terms
     * based on the input vector, which are then used to compute the final function value.
     *
     * @param x A constant reference to a vector of doubles representing the input to the function.
     * @return The calculated Zakharov function value for the given input vector.
     */
    inline double zakharov(const std::vector<double> &x)
    {
        const double sum1 = std::inner_product(x.begin(), x.end(), x.begin(), 0.0);
        size_t i = 0; // Declare i outside the lambda.
        const double sum2 = std::accumulate(x.begin(), x.end(), 0.0, [&i, &x](const double acc, const double &xi) {
            const auto result = acc + 0.5 * static_cast<double>(++i) * xi;
            if (i == x.size())
                i = 0;
            return result;
        });
        return sum1 + std::pow(sum2, 2) + std::pow(sum2, 4);
    }

    /**
     * @brief Computes the Schaffer F7 function value for the input vector.
     *
     * The Schaffer F7 function is a well-known benchmark function in the field of optimization. It is characterized by
     * a rugged landscape with many local minima, making it a challenging problem for optimization algorithms. This
     * function takes a vector input and computes the function value using a series of mathematical operations including
     * square roots, sine functions, and power functions.
     *
     * @param y Input vector containing the coordinates in the domain space.
     * @return The function value at the point specified by the input vector, which represents a point in the solution
     * space of the Schaffer F7 function.
     */
    inline double schaffer(const std::vector<double> &y)
    {
        std::vector<double> z(y.size());

        double result = 0;
        for (size_t i = 0; i < z.size() - 1; ++i)
        {
            const auto y_squared_sum = y[i] * y[i] + y[i + 1] * y[i + 1];

            z[i] = std::sqrt(y_squared_sum);

            const auto pow_val = std::pow(z[i], 0.2);

            const double tmp = std::sin(50.0 * pow_val);

            const auto tmp_squared = tmp * tmp;

            const auto z_rooted = std::sqrt(z[i]);

            const auto val_addition = z_rooted * tmp_squared;

            result += z_rooted + val_addition;
        }
        const int n = static_cast<int>(y.size()) - 1;
        result = result * result / n / n;
        return result;
    }

    /**
     * @brief Computes the HGBat function value for the input vector.
     *
     * The HGBat function is a complex, high-dimensional optimization problem characterized by a bat-shaped landscape.
     * It is used to evaluate the performance of optimization algorithms. This function performs a series of
     * mathematical operations including power and square root functions on the input vector to compute the function
     * value.
     *
     * @param z Input vector containing the coordinates in the domain space.
     * @return The function value at the point specified by the input vector, representing a point in the solution space
     * of the HGBat function.
     */
    inline double hgbat(const std::vector<double> &z)
    {
        std::vector<double> z_copy(z);

        std::transform(z_copy.begin(), z_copy.end(), z_copy.begin(), [](const double value) { return value - 1.0; });

        constexpr double alpha = 1.0 / 4.0;
        const double r2 = std::inner_product(z_copy.begin(), z_copy.end(), z_copy.begin(), 0.0);
        const double sum_z = std::accumulate(z_copy.begin(), z_copy.end(), 0.0);
        const double term1 = std::pow(r2, 2.0);
        const double term2 = std::pow(sum_z, 2.0);
        const double term3 = std::pow(std::abs(term1 - term2), 2 * alpha);
        const double term4 = (0.5 * r2 + sum_z) / static_cast<double>(z_copy.size());
        const double result = term3 + term4 + 0.5;

        return result;
    }

    /**
     * @brief Computes the Bent Cigar function value for the input vector.
     *
     * The Bent Cigar function is a unimodal function used in optimization problems. It is characterized by a narrow,
     * curved valley. The function is known for its steepness and is used to test the performance of optimization
     * algorithms in locating the global minimum amidst steep gradients.
     *
     * @param z Input vector containing the coordinates in the domain space.
     * @return The function value at the point specified by the input vector, representing a point in the solution space
     * of the Bent Cigar function.
     */
    inline double bent_cigar(const std::vector<double> &z)
    {
        static constexpr auto condition = 1.0e6;
        double result = z[0] * z[0];
        for (size_t i = 1; i < z.size(); ++i)
        {
            result += condition * z[i] * z[i];
        }
        return result;
    }

    /**
     * @brief Computes the Katsuura function value for the input vector.
     *
     * The Katsuura function is a non-separable, scalable function used in optimization problems. It is characterized by
     * a large number of local minima and a single global minimum, making it a challenging problem for optimization
     * algorithms. This function performs a series of mathematical operations including power and trigonometric
     * functions on the input vector to compute the function value.
     *
     * @param z Input vector containing the coordinates in the domain space.
     * @return The function value at the point specified by the input vector, representing a point in the solution space
     * of the Katsuura function.
     */
    inline double katsuura(const std::vector<double> &z)
    {
        double tmp3 = std::pow(static_cast<double>(z.size()), 1.2);

        double &&result =
            std::accumulate(z.cbegin(), z.cend(), 1.0, [index = 0, tmp3](double acc, double z_val) mutable {
                double temp = 0.0;
                for (int j = 1; j <= 32; ++j)
                {
                    const double tmp1 = std::pow(2.0, j);
                    const double tmp2 = tmp1 * z_val;
                    temp += std::fabs(tmp2 - std::floor(tmp2 + 0.5)) / tmp1;
                }
                return acc * std::pow(1.0 + (++index) * temp, 10.0 / tmp3);
            });

        const int n = static_cast<int>(z.size());
        const double tmp1 = 10.0 / n / n;
        result = result * tmp1 - tmp1;
        return result;
    }

    /**
     * @brief Computes the Ackley function value for the input vector.
     *
     * The Ackley function is a widely used benchmark function in optimization, characterized by a nearly flat outer
     * region and a large hole at the centre. The function is known for its complex oscillating behavior, which is used
     * to evaluate the performance of optimization algorithms in navigating complex landscapes.
     *
     * @param z Input vector containing the coordinates in the domain space.
     * @return The function value at the point specified by the input vector, representing a point in the solution space
     * of the Ackley function.
     */
    inline double ackley(const std::vector<double> &z)
    {

        static const double e = std::exp(1.0);
        const double sum1 = z.empty()
            ? 0.0
            : -0.2 * std::sqrt(std::inner_product(z.begin(), z.end(), z.begin(), 0.0) / static_cast<double>(z.size()));

        const double sum2 = z.empty()
            ? 0.0
            : std::accumulate(z.begin(), z.end(), 0.0,
                              [](const double acc, const double value) { return acc + std::cos(2.0 * IOH_PI * value); }) /
                static_cast<double>(z.size());

        const double result = e - 20.0 * std::exp(sum1) - std::exp(sum2) + 20.0;
        return result;
    }

    /**
     * @brief Computes the Schwefel function value for the input vector.
     *
     * The Schwefel function is a complex, non-linear, multimodal function characterized by a large number of local
     * minima, making it a challenging problem for optimization algorithms. This function performs a series of
     * mathematical operations including sine functions and power functions on the input vector to compute the function
     * value.
     *
     * @param z Input vector containing the coordinates in the domain space.
     * @return The function value at the point specified by the input vector, representing a point in the solution space
     * of the Schwefel function.
     */
    inline double schwefel(const std::vector<double> &z)
    {
        std::vector<double> z_copy(z);

        double result = 0.0;
        const int n = static_cast<int>(z.size());
        for (double &i : z_copy)
        {
            i += 4.209687462275036e+002;
            double tmp;

            if (i > 500)
            {
                result -=
                    (500.0 - std::fmod(i, 500)) * std::sin(std::pow(500.0 - std::fmod(i, 500), 0.5));
                tmp = (i - 500.0) / 100;
                result += tmp * tmp / n;
            }
            else if (i < -500)
            {
                result -= (-500.0 + std::fmod(std::fabs(i), 500)) *
                    std::sin(std::pow(500.0 - std::fmod(std::fabs(i), 500), 0.5));
                tmp = (i + 500.0) / 100;
                result += tmp * tmp / n;
            }
            else
            {
                result -= i * std::sin(std::pow(std::fabs(i), 0.5));
            }
        }
        result += 4.189828872724338e+002 * n;
        return result;
    }

    /**
     * @brief Computes the HappyCat function value for the input vector.
     *
     * The HappyCat function is a non-convex function used in optimization problems. It has a bowl-shaped surface with a
     * global minimum. This implementation calculates the HappyCat function value given a vector of input values.
     *
     * @param z Input vector containing the coordinates in the domain space.
     * @return The function value at the point specified by the input vector.
     */
    inline double happycat(const std::vector<double> &z)
    {
        constexpr double alpha = 1.0 / 8.0;
        const int n = static_cast<int>(z.size());
        double r2 = 0.0;
        double sum_z = 0.0;

        std::vector<double> z_copy(z);
        for (double &i : z_copy)
        {
            i -= 1.0;
            r2 += i * i;
            sum_z += i;
        }

        return std::pow(std::fabs(r2 - n), 2 * alpha) + (0.5 * r2 + sum_z) / n + 0.5;
    }

    /**
     * @brief Computes the Griewank-Rosenbrock function value for the input vector.
     *
     * The Griewank-Rosenbrock function is a complex, multimodal function often used in optimization problem testing. It
     * combines characteristics of both the Griewank and Rosenbrock functions. This function evaluates the
     * Griewank-Rosenbrock function at a point in the domain space specified by the input vector.
     *
     * @param z Input vector containing the coordinates in the domain space.
     * @return The function value at the point specified by the input vector.
     */
    inline double griewank_rosenbrock(const std::vector<double> &z)
    {
        std::vector<double> z_copy(z);

        z_copy[0] += 1.0;
        double f = 0.0;

        for (size_t i = 0; i < z.size() - 1; ++i)
        {
            z_copy[i + 1] += 1.0;
            const double tmp1 = z_copy[i] * z_copy[i] - z_copy[i + 1];
            const double tmp2 = z_copy[i] - 1.0;
            const double temp = 100.0 * tmp1 * tmp1 + tmp2 * tmp2;
            f += (temp * temp) / 4000.0 - std::cos(temp) + 1.0;
        }

        const double tmp1 = z_copy.back() * z_copy.back() - z_copy[0];
        const double tmp2 = z_copy.back() - 1.0;
        const double temp = 100.0 * tmp1 * tmp1 + tmp2 * tmp2;
        f += (temp * temp) / 4000.0 - std::cos(temp) + 1.0;
        return f;
    }

    /**
     * @brief Computes the Ellipse function value for the input vector.
     *
     * The Ellipse function is a scalable function used in optimization problems, characterized by its elliptic contours.
     * It is used to test the performance of optimization algorithms, especially in high-dimensional spaces. This
     * function calculates the Ellipse function value at a point specified by the input vector.
     *
     * @param z Input vector containing the coordinates in the domain space.
     * @return The function value at the point specified by the input vector.
     */
    inline double ellipse(const std::vector<double> &z)
    {

        const int nx = static_cast<int>(z.size());
        double result = 0.0;

        for (int i = 0; i < nx; ++i)
        {
            result += std::pow(10.0, 6.0 * i / (nx - 1)) * z[i] * z[i];
        }
        return result;
    }

    /**
     * @brief Computes the Discus function value for the input vector.
     *
     * The Discus function is a unimodal function used in optimization problems. It is characterized by a very steep
     * descent towards the global minimum, which is surrounded by a flat area. This function calculates the Discus
     * function value at a point specified by the input vector.
     *
     * @param z Input vector containing the coordinates in the domain space.
     * @return The function value at the point specified by the input vector.
     */
    inline double discus(const std::vector<double> &z)
    {
        static constexpr auto condition = 1.0e6;

        double value = condition * z[0] * z[0];
        for (size_t i = 1; i < z.size(); ++i)
        {
            value += z[i] * z[i];
        }
        return value;
    }

    /**
     * The Expanded Schaffer's F6 function, is a non-convex function characterized
     * by a large number of local minima, making it a challenging problem for optimization algorithms. This
     * implementation also includes transformations such as scaling and rotation to further complicate the optimization
     * landscape.
     *
     * @param x Input vector containing the coordinates in the domain space.
     * @param f Reference to a double where the function value will be stored.
     * @param os Transformation vector used in the function computation.
     * @param mr Rotation matrix used in the function computation.
     * @param s_flag Scaling flag used in the transformation.
     * @param r_flag Rotation flag used in the transformation.
     */
    inline void expanded_schaffer_f6(const std::vector<double> &x, double &f, const std::vector<double> &os,
                          const std::vector<std::vector<double>> &mr, const int s_flag, const int r_flag)
    {
        std::vector<double> z(x.size());
        std::vector<double> y(x.size());

        transformation::variables::scale_and_rotate(x, z, y, os, mr, 1.0, s_flag, r_flag);

        f = 0.0;

        for (size_t i = 0; i < x.size() - 1; i++)
        {
            double temp1 = sin(sqrt(z[i] * z[i] + z[i + 1] * z[i + 1]));
            temp1 *= temp1;

            const double temp2 = 1.0 + 0.001 * (z[i] * z[i] + z[i + 1] * z[i + 1]);
            f += 0.5 + (temp1 - 0.5) / (temp2 * temp2);
        }

        double temp1 = sin(sqrt(z.back() * z.back() + z.front() * z.front()));
        temp1 *= temp1;

        const double temp2 = 1.0 + 0.001 * (z.back() * z.back() + z.front() * z.front());
        f += 0.5 + (temp1 - 0.5) / (temp2 * temp2);
    }

    /**
     * @brief Computes the Griewank function value for the input vector.
     *
     * The Griewank function is a non-convex function often used in the evaluation of optimization algorithms. It is
     * known for its large search space and numerous local minima, which pose a challenge for optimization algorithms.
     * This implementation includes transformations such as scaling and rotation.
     *
     * @param x Input vector containing the coordinates in the domain space.
     * @param f Reference to a double where the function value will be stored.
     * @param os Transformation vector used in the function computation.
     * @param mr Rotation matrix used in the function computation.
     * @param s_flag Scaling flag used in the transformation.
     * @param r_flag Rotation flag used in the transformation.
     */
    inline void griewank(const std::vector<double> &x, double &f, const std::vector<double> &os,
                         const std::vector<std::vector<double>> &mr, const int s_flag, const int r_flag)
    {
        std::vector<double> z(x.size());
        std::vector<double> y(x.size());

        transformation::variables::scale_and_rotate(x, z, y, os, mr, 600.0 / 100.0, s_flag, r_flag);

        const double s = std::accumulate(z.begin(), z.end(), 0.0, [](const double sum, const double val) { return sum + val * val; });

        double p = 1.0;
        for (size_t i = 0; i < x.size(); i++)
        {
            p *= cos(z[i] / sqrt(1.0 + static_cast<int>(i)));
        }

        f = 1.0 + s / 4000.0 - p;
    }
 
} // namespace ioh::problem
