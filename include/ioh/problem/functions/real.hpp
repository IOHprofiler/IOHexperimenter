#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

#include "ioh/problem/transformation.hpp"

#define LOG_FILE_NAME "IOHexperimenter.log"

#define LOG(message)                                                           \
  do {                                                                         \
    std::ofstream debug_log(LOG_FILE_NAME, std::ios::app);                     \
    auto now = std::chrono::system_clock::now();                               \
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);          \
    debug_log << "["                                                           \
              << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") \
              << "] " << message << std::endl;                                 \
    debug_log.close();                                                         \
  } while (0)

/**
 * @namespace ioh::problem
 * Namespace where the various problem functions are defined.
 */
namespace ioh::problem {

/**
 * @var INF
 * Defines a very large number representing infinity.
 */
constexpr double INF = 1.0e99;

/**
 * @var EPS
 * Defines a small number used to prevent division by zero and other numerical instabilities.
 */
constexpr double EPS = 1.0e-14;

/**
 * @var E
 * The mathematical constant e.
 */
constexpr double E = 2.7182818284590452353602874713526625;

/**
 * @var PI
 * The mathematical constant pi.
 */
constexpr double PI = 3.1415926535897932384626433832795029;

/**
 * @fn double rastrigin(const std::vector<double>& z)
 * @brief Computes the Rastrigin function for a given input vector.
 * @param z Input vector.
 * @return Function value at input z.
 */
inline double rastrigin(const std::vector<double> &z) {
  double result =
      std::accumulate(z.begin(), z.end(), 0.0, [](double acc, double value) {
        return acc + (value * value - 10.0 * std::cos(2.0 * PI * value) + 10.0);
      });
  return result;
}

/**
 * @brief Calculates the Rosenbrock function value for the input vector.
 *
 * The Rosenbrock function is commonly used for optimization algorithms testing. It is defined over an n-dimensional space. The function is computed as the sum of squares of sequential elements, with a particular transformation applied to each element before the computation.
 *
 * @param z A constant reference to a vector of doubles, which represents the input to the function.
 * @return The computed Rosenbrock function value.
 */
inline double rosenbrock(const std::vector<double> &z) {
  std::vector<double> z_copy(z);
  double result = 0.0;
  z_copy[0] += 1.0;
  for (size_t i = 0; i < z_copy.size() - 1; ++i) {
    z_copy[i + 1] += 1.0;
    auto tmp1 = z_copy[i] * z_copy[i] - z_copy[i + 1];
    auto tmp2 = z_copy[i] - 1.0;
    result += 100.0 * tmp1 * tmp1 + tmp2 * tmp2;
  }
  return result;
}

/**
 * @brief Computes the Levy function value given an input vector.
 *
 * The Levy function is well known in the optimization domain, defined with a combination of sine and cosine operations along with polynomial expressions. The function calculates various terms separately which contribute to the final Levy function value.
 *
 * @param z A constant reference to a vector of doubles representing the input to the function.
 * @return The calculated Levy function value based on the input vector.
 */
inline double levy(const std::vector<double> &z) {
  std::vector<double> w(z.size());

  for (size_t i = 0; i < z.size(); ++i) {
    w[i] = 1.0 + (z[i] - 0.0) / 4.0;
  }

  double term1 = std::pow(std::sin(PI * w[0]), 2);
  double term3 = std::pow((w.back() - 1), 2) *
                 (1 + std::pow(std::sin(2 * PI * w.back()), 2));
  double sum = 0.0;

  for (size_t i = 0; i < z.size() - 1; ++i) {
    auto wi = w[i];
    auto newv =
        std::pow((wi - 1), 2) * (1 + 10 * std::pow(std::sin(PI * wi + 1), 2));
    sum += newv;
  }

  double &&result = term1 + sum + term3;
  return result;
}

/**
 * @brief Calculates the Zakharov function value for a given input vector.
 *
 * The Zakharov function is used frequently in optimization problems. It involves calculating two summation terms based on the input vector, which are then used to compute the final function value.
 *
 * @param x A constant reference to a vector of doubles representing the input to the function.
 * @return The calculated Zakharov function value for the given input vector.
 */
inline double zakharov(const std::vector<double> &x) {
  double sum1 = std::inner_product(x.begin(), x.end(), x.begin(), 0.0);
  size_t i = 0; // Declare i outside the lambda.
  double sum2 = std::accumulate(x.begin(), x.end(), 0.0,
                                [&i, &x](double accum, const double &xi) {
                                  auto result = accum + 0.5 * (++i) * xi;
                                  if (i == x.size())
                                    i = 0;
                                  return result;
                                });
  double &&result = sum1 + std::pow(sum2, 2) + std::pow(sum2, 4);
  return result;
}

/**
 * @brief Computes the Schaffer F7 function value for the input vector.
 *
 * The Schaffer F7 function is a well-known benchmark function in the field of optimization. It is characterized by a rugged landscape with many local minima, making it a challenging problem for optimization algorithms. This function takes a vector input and computes the function value using a series of mathematical operations including square roots, sine functions, and power functions.
 *
 * @param y Input vector containing the coordinates in the domain space.
 * @return The function value at the point specified by the input vector, which represents a point in the solution space of the Schaffer F7 function.
 */
inline double schaffer_F7(const std::vector<double> &y) {
  std::vector<double> z(y.size());

  double result = 0;
  for (size_t i = 0; i < z.size() - 1; ++i) {
    auto y_squared_sum = y[i] * y[i] + y[i + 1] * y[i + 1];

    z[i] = std::sqrt(y_squared_sum);

    auto pow_val = std::pow(z[i], 0.2);

    double tmp = std::sin(50.0 * pow_val);

    auto tmp_squared = tmp * tmp;

    auto z_rooted = std::sqrt(z[i]);

    auto val_addition = z_rooted * tmp_squared;

    result += z_rooted + val_addition;
  }

  result = result * result / (y.size() - 1) / (y.size() - 1);
  return result;
}

/**
 * @brief Computes the HGBat function value for the input vector.
 *
 * The HGBat function is a complex, high-dimensional optimization problem characterized by a bat-shaped landscape. It is used to evaluate the performance of optimization algorithms. This function performs a series of mathematical operations including power and square root functions on the input vector to compute the function value.
 *
 * @param z Input vector containing the coordinates in the domain space.
 * @return The function value at the point specified by the input vector, representing a point in the solution space of the HGBat function.
 */
inline double hgbat(const std::vector<double> &z) {
  std::vector<double> z_copy(z);

  std::transform(z_copy.begin(), z_copy.end(), z_copy.begin(),
                 [](double value) { return value - 1.0; });

  const double alpha = 1.0 / 4.0;
  const double r2 =
      std::inner_product(z_copy.begin(), z_copy.end(), z_copy.begin(), 0.0);
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
 * The Bent Cigar function is a unimodal function used in optimization problems. It is characterized by a narrow, curved valley. The function is known for its steepness and is used to test the performance of optimization algorithms in locating the global minimum amidst steep gradients.
 *
 * @param z Input vector containing the coordinates in the domain space.
 * @return The function value at the point specified by the input vector, representing a point in the solution space of the Bent Cigar function.
 */
inline double bent_cigar(const std::vector<double> &z)
{
  static const auto condition = 1.0e6;
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
 * The Katsuura function is a non-separable, scalable function used in optimization problems. It is characterized by a large number of local minima and a single global minimum, making it a challenging problem for optimization algorithms. This function performs a series of mathematical operations including power and trigonometric functions on the input vector to compute the function value.
 *
 * @param z Input vector containing the coordinates in the domain space.
 * @return The function value at the point specified by the input vector, representing a point in the solution space of the Katsuura function.
 */
inline double katsuura(const std::vector<double> &z) {
  double tmp3 = std::pow(static_cast<double>(z.size()), 1.2);

  double &&result = std::accumulate(
      z.cbegin(), z.cend(), 1.0,
      [index = 0, tmp3](double acc, double z_val) mutable {
        double temp = 0.0;
        for (int j = 1; j <= 32; ++j) {
          double tmp1 = std::pow(2.0, j);
          double tmp2 = tmp1 * z_val;
          temp += std::fabs(tmp2 - std::floor(tmp2 + 0.5)) / tmp1;
        }
        return acc * std::pow(1.0 + (++index) * temp, 10.0 / tmp3);
      });

  double tmp1 = 10.0 / z.size() / z.size();
  result = result * tmp1 - tmp1;
  return result;
}

/**
 * @brief Computes the Ackley function value for the input vector.
 *
 * The Ackley function is a widely used benchmark function in optimization, characterized by a nearly flat outer region and a large hole at the centre. The function is known for its complex oscillating behavior, which is used to evaluate the performance of optimization algorithms in navigating complex landscapes.
 *
 * @param z Input vector containing the coordinates in the domain space.
 * @return The function value at the point specified by the input vector, representing a point in the solution space of the Ackley function.
 */
inline double ackley(const std::vector<double> &z) {
  const double sum1 =
      z.empty() ? 0.0
                : -0.2 * std::sqrt(std::inner_product(z.begin(), z.end(),
                                                      z.begin(), 0.0) /
                                   static_cast<double>(z.size()));

  const double sum2 =
      z.empty() ? 0.0
                : std::accumulate(z.begin(), z.end(), 0.0,
                                  [](double acc, double value) {
                                    return acc + std::cos(2.0 * PI * value);
                                  }) /
                      static_cast<double>(z.size());

  double result = E - 20.0 * std::exp(sum1) - std::exp(sum2) + 20.0;
  return result;
}

/**
 * @brief Computes the Schwefel function value for the input vector.
 *
 * The Schwefel function is a complex, non-linear, multimodal function characterized by a large number of local minima, making it a challenging problem for optimization algorithms. This function performs a series of mathematical operations including sine functions and power functions on the input vector to compute the function value.
 *
 * @param z Input vector containing the coordinates in the domain space.
 * @return The function value at the point specified by the input vector, representing a point in the solution space of the Schwefel function.
 */
inline double schwefel(const std::vector<double> &z) {
  std::vector<double> z_copy(z);

  double result = 0.0;
  for (size_t i = 0; i < z_copy.size(); ++i) {
    z_copy[i] += 4.209687462275036e+002;
    double tmp;

    if (z_copy[i] > 500) {
      result -= (500.0 - std::fmod(z_copy[i], 500)) *
                std::sin(std::pow(500.0 - std::fmod(z_copy[i], 500), 0.5));
      tmp = (z_copy[i] - 500.0) / 100;
      result += tmp * tmp / z_copy.size();
    } else if (z_copy[i] < -500) {
      result -=
          (-500.0 + std::fmod(std::fabs(z_copy[i]), 500)) *
          std::sin(std::pow(500.0 - std::fmod(std::fabs(z_copy[i]), 500), 0.5));
      tmp = (z_copy[i] + 500.0) / 100;
      result += tmp * tmp / z_copy.size();
    } else {
      result -= z_copy[i] * std::sin(std::pow(std::fabs(z_copy[i]), 0.5));
    }
  }
  result += 4.189828872724338e+002 * z_copy.size();
  return result;
}

/**
 * @brief Computes the HappyCat function value for the input vector.
 *
 * The HappyCat function is a non-convex function used in optimization problems. It has a bowl-shaped surface with a global minimum.
 * This implementation calculates the HappyCat function value given a vector of input values.
 *
 * @param z Input vector containing the coordinates in the domain space.
 * @return The function value at the point specified by the input vector.
 */
inline double happycat(const std::vector<double> &z)
{
  const double alpha = 1.0 / 8.0;
  double r2 = 0.0;
  double sum_z = 0.0;

  std::vector<double> z_copy(z);
  for (size_t i = 0; i < z_copy.size(); ++i) {
    z_copy[i] -= 1.0;
    r2 += z_copy[i] * z_copy[i];
    sum_z += z_copy[i];
  }

  double f = std::pow(std::fabs(r2 - z_copy.size()), 2 * alpha) +
      (0.5 * r2 + sum_z) / z_copy.size() + 0.5;
  return f;
}

/**
 * @brief Computes the Griewank-Rosenbrock function value for the input vector.
 *
 * The Griewank-Rosenbrock function is a complex, multimodal function often used in optimization problem testing. It combines characteristics of both the Griewank and Rosenbrock functions.
 * This function evaluates the Griewank-Rosenbrock function at a point in the domain space specified by the input vector.
 *
 * @param z Input vector containing the coordinates in the domain space.
 * @return The function value at the point specified by the input vector.
 */
inline double grie_rosen(const std::vector<double> &z)
{
  std::vector<double> z_copy(z);

  z_copy[0] += 1.0;
  double f = 0.0;

  for (size_t i = 0; i < z.size() - 1; ++i)
  {
    z_copy[i + 1] += 1.0;
    double tmp1 = z_copy[i] * z_copy[i] - z_copy[i + 1];
    double tmp2 = z_copy[i] - 1.0;
    double temp = 100.0 * tmp1 * tmp1 + tmp2 * tmp2;
    f += (temp * temp) / 4000.0 - std::cos(temp) + 1.0;
  }

  double tmp1 = z_copy.back() * z_copy.back() - z_copy[0];
  double tmp2 = z_copy.back() - 1.0;
  double temp = 100.0 * tmp1 * tmp1 + tmp2 * tmp2;
  f += (temp * temp) / 4000.0 - std::cos(temp) + 1.0;
  return f;
}

/**
 * @brief Computes the Ellips function value for the input vector.
 *
 * The Ellips function is a scalable function used in optimization problems, characterized by its elliptic contours. It is used to test the performance of optimization algorithms, especially in high-dimensional spaces.
 * This function calculates the Ellips function value at a point specified by the input vector.
 *
 * @param z Input vector containing the coordinates in the domain space.
 * @return The function value at the point specified by the input vector.
 */
inline double ellips(const std::vector<double> &z)
{
  constexpr double BASE = 10.0;
  constexpr double EXPONENT_SCALE = 6.0;

  auto nx = z.size();
  double result = 0.0;

  for (size_t i = 0; i < nx; ++i) {
    result += std::pow(BASE, EXPONENT_SCALE * i / (nx - 1)) * z[i] * z[i];
  }
  return result;
}

/**
 * @brief Computes the Discus function value for the input vector.
 *
 * The Discus function is a unimodal function used in optimization problems. It is characterized by a very steep descent towards the global minimum, which is surrounded by a flat area.
 * This function calculates the Discus function value at a point specified by the input vector.
 *
 * @param z Input vector containing the coordinates in the domain space.
 * @return The function value at the point specified by the input vector.
 */
inline double discus(const std::vector<double> &z)
{
  static const auto condition = 1.0e6;
  auto nx = z.size();
  double value = condition * z[0] * z[0];
  for (size_t i = 1; i < nx; ++i)
  {
    value += z[i] * z[i];
  }
  return value;
}

/**
 * @brief Computes the Escaffer6 function value for the input vector.
 *
 * The Escaffer6 function, also known as the Expanded Scaffer's F6 function, is a non-convex function characterized by a large number of local minima, making it a challenging problem for optimization algorithms. This implementation also includes transformations such as scaling and rotation to further complicate the optimization landscape.
 *
 * @param x Input vector containing the coordinates in the domain space.
 * @param f Reference to a double where the function value will be stored.
 * @param Os Transformation vector used in the function computation.
 * @param Mr Rotation matrix used in the function computation.
 * @param s_flag Scaling flag used in the transformation.
 * @param r_flag Rotation flag used in the transformation.
 */
inline void escaffer6(const std::vector<double> &x, double &f,
                           const std::vector<double> &Os,
                           const std::vector<std::vector<double>> &Mr,
                           int s_flag, int r_flag) {
  std::vector<double> z(x.size());
  std::vector<double> y(x.size());

  ioh::problem::transformation::variables::scale_and_rotate(x, z, y, Os, Mr, 1.0, s_flag, r_flag);

  f = 0.0;

  for (size_t i = 0; i < x.size() - 1; i++) {
    double temp1 = sin(sqrt(z[i] * z[i] + z[i + 1] * z[i + 1]));
    temp1 *= temp1;

    double temp2 = 1.0 + 0.001 * (z[i] * z[i] + z[i + 1] * z[i + 1]);
    f += 0.5 + (temp1 - 0.5) / (temp2 * temp2);
  }

  double temp1 = sin(sqrt(z.back() * z.back() + z.front() * z.front()));
  temp1 *= temp1;

  double temp2 = 1.0 + 0.001 * (z.back() * z.back() + z.front() * z.front());
  f += 0.5 + (temp1 - 0.5) / (temp2 * temp2);
}

/**
 * @brief Computes the Griewank function value for the input vector.
 *
 * The Griewank function is a non-convex function often used in the evaluation of optimization algorithms. It is known for its large search space and numerous local minima, which pose a challenge for optimization algorithms. This implementation includes transformations such as scaling and rotation.
 *
 * @param x Input vector containing the coordinates in the domain space.
 * @param f Reference to a double where the function value will be stored.
 * @param Os Transformation vector used in the function computation.
 * @param Mr Rotation matrix used in the function computation.
 * @param s_flag Scaling flag used in the transformation.
 * @param r_flag Rotation flag used in the transformation.
 */
inline void griewank(const std::vector<double> &x, double &f,
                          const std::vector<double> &Os,
                          const std::vector<std::vector<double>> &Mr,
                          int s_flag, int r_flag) {
  std::vector<double> z(x.size());
  std::vector<double> y(x.size());

  ioh::problem::transformation::variables::scale_and_rotate(x, z, y, Os, Mr, 600.0 / 100.0, s_flag, r_flag);

  double s =
      std::accumulate(z.begin(), z.end(), 0.0,
                      [](double sum, double val) { return sum + val * val; });

  double p = 1.0;
  for (size_t i = 0; i < x.size(); i++) {
    p *= cos(z[i] / sqrt(1.0 + i));
  }

  f = 1.0 + s / 4000.0 - p;
}

/**
 * @brief Computes the hf02 function value for the input vector.
 *
 * The hf02 function is a hybrid function that combines several benchmark functions to create a complex, multimodal landscape. It is used to evaluate the performance of optimization algorithms in navigating complex, high-dimensional spaces.
 *
 * @param prepared_y Input vector containing the coordinates in the domain space.
 * @return The function value at the point specified by the input vector.
 */
inline double hf02(const std::vector<double> &prepared_y)
{
  int nx = prepared_y.size();
  int cf_num = 3;
  std::vector<double> fit(cf_num);
  std::vector<int> G(cf_num);
  std::vector<int> G_nx(cf_num);
  std::vector<double> Gp = {0.4, 0.4, 0.2};

  int tmp = 0;
  for (int i = 0; i < cf_num - 1; ++i) {
    G_nx[i] = std::ceil(Gp[i] * nx);
    tmp += G_nx[i];
  }
  G_nx[cf_num - 1] = nx - tmp;

  G[0] = 0;
  for (int i = 1; i < cf_num; ++i) {
    G[i] = G[i - 1] + G_nx[i - 1];
  }

  std::vector<double> bent_cigar_z(prepared_y.begin() + G[0], prepared_y.begin() + G[0] + G_nx[0]);
  fit[0] = bent_cigar(bent_cigar_z);

  std::vector<double> hgbat_z(prepared_y.begin() + G[1], prepared_y.begin() + G[1] + G_nx[1]);
  fit[1] = hgbat(hgbat_z);

  std::vector<double> rastrigin_z(prepared_y.begin() + G[2], prepared_y.begin() + G[2] + G_nx[2]);
  fit[2] = rastrigin(rastrigin_z);

  double &&f = 0.0;
  for (const auto &val : fit) {
    f += val;
  }
  return f;
}

/**
 * @brief Computes the hf10 function value for the input vector.
 *
 * The hf10 function is a hybrid function that combines several benchmark functions to create a complex, multimodal landscape. It is used to evaluate the performance of optimization algorithms in navigating complex, high-dimensional spaces. This function integrates various benchmark functions including hgbat, katsuura, ackley, rastrigin, schwefel, and schaffer_F7.
 *
 * @param prepared_y Input vector containing the coordinates in the domain space.
 * @return The function value at the point specified by the input vector.
 */
inline double hf10(const std::vector<double> &prepared_y) {
  const int cf_num = 6;
  std::vector<double> fit(cf_num, 0.0);
  std::vector<int> G_nx(cf_num);
  std::vector<int> G(cf_num);
  std::vector<double> Gp = {0.1, 0.2, 0.2, 0.2, 0.1, 0.2};
  int nx = prepared_y.size();

  int tmp = 0;
  for (int i = 0; i < cf_num - 1; i++) {
    G_nx[i] = static_cast<int>(std::ceil(Gp[i] * nx));
    tmp += G_nx[i];
  }
  G_nx[cf_num - 1] = nx - tmp;

  G[0] = 0;
  for (int i = 1; i < cf_num; i++) {
    G[i] = G[i - 1] + G_nx[i - 1];
  }

  std::vector<double> hgbat_z(prepared_y.begin() + G[0], prepared_y.begin() + G[0] + G_nx[0]);
  fit[0] = hgbat(hgbat_z);

  std::vector<double> katsuura_z(prepared_y.begin() + G[1], prepared_y.begin() + G[1] + G_nx[1]);
  fit[1] = katsuura(katsuura_z);

  std::vector<double> ackley_z(prepared_y.begin() + G[2], prepared_y.begin() + G[2] + G_nx[2]);
  fit[2] = ackley(ackley_z);

  std::vector<double> rastrigin_z(prepared_y.begin() + G[3], prepared_y.begin() + G[3] + G_nx[3]);
  fit[3] = rastrigin(rastrigin_z);

  std::vector<double> schwefel_z(prepared_y.begin() + G[4], prepared_y.begin() + G[4] + G_nx[4]);
  fit[4] = schwefel(schwefel_z);

  std::vector<double> schaffer_y(prepared_y.begin() + G[5], prepared_y.begin() + G[5] + G_nx[5]);
  fit[5] = schaffer_F7(schaffer_y);

  double &&f = std::accumulate(fit.begin(), fit.end(), 0.0);
  return f;
}

/**
 * @brief Computes the hf06 function value for the input vector.
 *
 * The hf06 function is another hybrid function that integrates several benchmark functions to create a challenging optimization landscape characterized by multiple local minima and a global minimum. It is utilized to test the robustness and efficiency of optimization algorithms.
 *
 * @param prepared_y Input vector containing the coordinates in the domain space.
 * @return The function value at the point specified by the input vector.
 */
inline double hf06(const std::vector<double> &prepared_y)
{
  const int cf_num = 5;
  std::vector<double> fit(cf_num, 0);
  std::vector<int> G_nx(cf_num);
  std::vector<int> G(cf_num);
  std::vector<double> Gp = {0.3, 0.2, 0.2, 0.1, 0.2};

  int tmp = 0;
  for (int i = 0; i < cf_num - 1; ++i) {
    G_nx[i] = std::ceil(Gp[i] * prepared_y.size());
    tmp += G_nx[i];
  }
  G_nx[cf_num - 1] = prepared_y.size() - tmp;
  if (G_nx[cf_num - 1] < 0) {
    G_nx[cf_num - 1] = 0;
  }

  G[0] = 0;
  for (int i = 1; i < cf_num; ++i) {
    G[i] = G[i - 1] + G_nx[i - 1];
  }

  std::vector<double> katsuura_z(prepared_y.begin() + G[0], prepared_y.begin() + G[0] + G_nx[0]);
  fit[0] = katsuura(katsuura_z);

  std::vector<double> happycat_z(prepared_y.begin() + G[1], prepared_y.begin() + G[1] + G_nx[1]);
  fit[1] = happycat(happycat_z);

  std::vector<double> grie_rosen_z(prepared_y.begin() + G[2], prepared_y.begin() + G[2] + G_nx[2]);
  fit[2] = grie_rosen(grie_rosen_z);

  std::vector<double> schwefel_z(prepared_y.begin() + G[3], prepared_y.begin() + G[3] + G_nx[3]);
  fit[3] = schwefel(schwefel_z);

  std::vector<double> ackley_z(prepared_y.begin() + G[4], prepared_y.begin() + G[4] + G_nx[4]);
  fit[4] = ackley(ackley_z);

  double f = std::accumulate(fit.begin(), fit.end(), 0.0);
  return f;
}

/**
 * @brief Computes the cf_cal function value for the input vector.
 *
 * The cf_cal function is a helper function used in the computation of composite functions in optimization problems. It calculates the weighted sum of several function values, which is a common approach in creating hybrid functions to test optimization algorithms.
 *
 * @param x Input vector containing the coordinates in the domain space.
 * @param Os Transformation vectors used in the function computation.
 * @param delta Delta values used in the function computation.
 * @param bias Bias values used in the function computation.
 * @param fit Vector to store individual function values.
 * @return The weighted sum of function values.
 */
inline double cf_cal(const std::vector<double> &x,
                     const std::vector<std::vector<double>> &Os,
                     const std::vector<double> &delta,
                     const std::vector<double> &bias,
                     std::vector<double> &fit) {
  int nx = x.size();
  int cf_num = fit.size();
  constexpr double INF = std::numeric_limits<double>::infinity();

  std::vector<double> w(cf_num);
  double w_max = 0.0;
  double w_sum = 0.0;

  for (int i = 0; i < cf_num; i++) {
    fit[i] += bias[i];
    w[i] = 0.0;

    for (int j = 0; j < nx; j++) {
      w[i] += std::pow(x[j] - Os[i][j], 2.0);
    }

    if (w[i] != 0)
      w[i] = std::pow(1.0 / w[i], 0.5) *
             std::exp(-w[i] / (2.0 * nx * std::pow(delta[i], 2.0)));
    else
      w[i] = INF;

    w_max = std::max(w_max, w[i]);
  }

  for (const auto &weight : w) {
    w_sum += weight;
  }

  if (w_max == 0) {
    std::fill(w.begin(), w.end(), 1.0);
    w_sum = cf_num;
  }

  double f = 0.0;
  for (int i = 0; i < cf_num; i++) {
    f += (w[i] / w_sum) * fit[i];
  }
  return f;
}

/**
 * @brief Computes the cf01 function value for the input vector.
 *
 * The cf01 function is a composite function that integrates several benchmark functions with transformations such as scaling and rotation. It is used to create a complex optimization landscape to evaluate the performance of optimization algorithms.
 *
 * @param x Input vector containing the coordinates in the domain space.
 * @param Os Transformation vectors used in the function computation.
 * @param Mr Rotation matrices used in the function computation.
 * @param r_flag Rotation flag used in the transformation.
 * @return The function value at the point specified by the input vector.
 */
inline double cf01(
  const std::vector<double> &x,
  const std::vector<std::vector<double>> &Os,
  const std::vector<std::vector<std::vector<double>>> &Mr,
  int r_flag
)
{
  std::vector<double> deltas = {10, 20, 30, 40, 50};
  std::vector<double> biases = {0, 200, 300, 100, 400};
  std::vector<double> fit(5);

  int nx = x.size();

  std::vector<double> rosenbrock_z(nx);
  std::vector<double> rosenbrock_y(nx);
  ioh::problem::transformation::variables::scale_and_rotate(x, rosenbrock_z, rosenbrock_y, Os[0], Mr[0], 2.048 / 100.0, 1, r_flag);
  fit[0] = rosenbrock(rosenbrock_z);
  fit[0] = 10000 * fit[0] / 1e+4;

  std::vector<double> ellips_z(nx);
  std::vector<double> ellips_y(nx);
  ioh::problem::transformation::variables::scale_and_rotate(x, ellips_z, ellips_y, Os[1], Mr[1], 1.0, 1, 0);
  fit[1] = ellips(ellips_z);
  fit[1] = 10000 * fit[1] / 1e+10;

  std::vector<double> bent_cigar_z(nx);
  std::vector<double> bent_cigar_y(nx);
  ioh::problem::transformation::variables::scale_and_rotate(x, bent_cigar_z, bent_cigar_y, Os[2], Mr[2], 1.0, 1, r_flag);
  fit[2] = bent_cigar(bent_cigar_z);
  fit[2] = 10000 * fit[2] / 1e+30;

  std::vector<double> discus_z(nx);
  std::vector<double> discus_y(nx);
  ioh::problem::transformation::variables::scale_and_rotate(x, discus_z, discus_y, Os[3], Mr[3], 1.0, 1, r_flag);
  fit[3] = discus(discus_z);
  fit[3] = 10000 * fit[3] / 1e+10;

  std::vector<double> ellips_2_z(nx);
  std::vector<double> ellips_2_y(nx);
  ioh::problem::transformation::variables::scale_and_rotate(x, ellips_2_z, ellips_2_y, Os[4], Mr[4], 1.0, 1, 0);
  fit[4] = ellips(ellips_2_z);
  fit[4] = 10000 * fit[4] / 1e+10;

  double f = cf_cal(x, Os, deltas, biases, fit);
  return f;
}

/**
 * @brief Computes the cf02 function value for the input vector.
 *
 * Similar to cf01, the cf02 function is a composite function that integrates several benchmark functions with transformations to create a challenging optimization landscape. It is utilized to test the performance of optimization algorithms in complex, high-dimensional spaces.
 *
 * @param x Input vector containing the coordinates in the domain space.
 * @param Os Transformation vectors used in the function computation.
 * @param Mr Rotation matrices used in the function computation.
 * @param r_flag Rotation flag used in the transformation.
 * @return The function value at the point specified by the input vector.
 */
inline double cf02(const std::vector<double> &x,
          const std::vector<std::vector<double>> &Os,
          const std::vector<std::vector<std::vector<double>>> &Mr,
          int r_flag)
{
  const int nx = x.size();
  const std::vector<double> delta = {20, 10, 10};
  const std::vector<double> bias = {0, 200, 100};

  std::vector<double> fit(3);

  std::vector<double> schwefel_z(nx);
  std::vector<double> schwefel_y(nx);
  ioh::problem::transformation::variables::scale_and_rotate(x, schwefel_z, schwefel_y, Os[0], Mr[0], 1000.0 / 100.0, 1, 0);
  fit[0] = schwefel(schwefel_z);

  std::vector<double> rastrigin_z(nx);
  std::vector<double> rastrigin_y(nx);
  ioh::problem::transformation::variables::scale_and_rotate(x, rastrigin_z, rastrigin_y, Os[1], Mr[1], 5.12 / 100.0, 1, r_flag);
  fit[1] = rastrigin(rastrigin_z);

  std::vector<double> hgbat_z(nx);
  std::vector<double> hgbat_y(nx);
  ioh::problem::transformation::variables::scale_and_rotate(x, hgbat_z, hgbat_y, Os[2], Mr[2], 5.0 / 100.0, 1, r_flag);
  fit[2] = hgbat(hgbat_z);

  double f = cf_cal(x, Os, delta, bias, fit);
  return f;
}

/**
 * @brief Computes the cf06 function value for the input vector.
 *
 * The cf06 function is a composite function that combines several benchmark functions with transformations to create a complex optimization landscape. It is used to evaluate the performance of optimization algorithms in navigating complex, high-dimensional spaces.
 *
 * @param x Input vector containing the coordinates in the domain space.
 * @param Os Transformation vectors used in the function computation.
 * @param Mr Rotation matrices used in the function computation.
 * @param r_flag Rotation flag used in the transformation.
 * @return The function value at the point specified by the input vector.
 */
inline double cf06(const std::vector<double> &x,
          const std::vector<std::vector<double>> &Os,
          const std::vector<std::vector<std::vector<double>>> &Mr,
          int r_flag)
{
  const int nx = x.size();
  const std::vector<double> delta = {20, 20, 30, 30, 20};
  const std::vector<double> bias = {0, 200, 300, 400, 200};

  std::vector<double> fit(5);

  escaffer6(x, fit[0], Os[0], Mr[0], 1, r_flag);
  fit[0] *= 10000 / 2e+7;

  std::vector<double> schwefel_z(nx);
  std::vector<double> schwefel_y(nx);
  ioh::problem::transformation::variables::scale_and_rotate(x, schwefel_z, schwefel_y, Os[1], Mr[1], 1000.0 / 100.0, 1, r_flag);
  fit[1] = schwefel(schwefel_z);

  griewank(x, fit[2], Os[2], Mr[2], 1, r_flag);
  fit[2] *= 1000 / 100;

  std::vector<double> rosenbrock_z(nx);
  std::vector<double> rosenbrock_y(nx);
  ioh::problem::transformation::variables::scale_and_rotate(x, rosenbrock_z, rosenbrock_y, Os[3], Mr[3],
                   2.048 / 100.0, 1, r_flag);
  fit[3] = rosenbrock(rosenbrock_z);

  std::vector<double> rastrigin_z(nx);
  std::vector<double> rastrigin_y(nx);
  ioh::problem::transformation::variables::scale_and_rotate(x, rastrigin_z, rastrigin_y, Os[4], Mr[4], 5.12 / 100.0, 1, r_flag);
  fit[4] = rastrigin(rastrigin_z);
  fit[4] *= 10000 / 1e+3;

  double f = cf_cal(x, Os, delta, bias, fit);
  return f;
}

/**
 * @brief Computes the cf07 function value for the input vector.
 *
 * The cf07 function is a composite function that integrates several benchmark functions with transformations to create a challenging optimization landscape. It is utilized to test the robustness and efficiency of optimization algorithms in complex, high-dimensional spaces.
 *
 * @param x Input vector containing the coordinates in the domain space.
 * @param Os Transformation vectors used in the function computation.
 * @param Mr Rotation matrices used in the function computation.
 * @param r_flag Rotation flag used in the transformation.
 * @return The function value at the point specified by the input vector.
 */
inline double cf07(const std::vector<double> &x,
          const std::vector<std::vector<double>> &Os,
          const std::vector<std::vector<std::vector<double>>> &Mr,
          int r_flag)
{
  const int nx = x.size();
  std::vector<double> fit(6);
  std::vector<double> delta = {10, 20, 30, 40, 50, 60};
  std::vector<double> bias = {0, 300, 500, 100, 400, 200};

  std::vector<double> hgbat_z(nx);
  std::vector<double> hgbat_y(nx);
  ioh::problem::transformation::variables::scale_and_rotate(x, hgbat_z, hgbat_y, Os[0], Mr[0], 5.0 / 100.0, 1, r_flag);
  fit[0] = hgbat(hgbat_z);
  fit[0] *= 10000 / 1000;

  std::vector<double> rastrigin_z(nx);
  std::vector<double> rastrigin_y(nx);
  ioh::problem::transformation::variables::scale_and_rotate(x, rastrigin_z, rastrigin_y, Os[1], Mr[1], 5.12 / 100.0, 1, r_flag);
  fit[1] = rastrigin(rastrigin_z);
  fit[1] *= 10000 / 1e+3;

  std::vector<double> schwefel_z(nx);
  std::vector<double> schwefel_y(nx);
  ioh::problem::transformation::variables::scale_and_rotate(x, schwefel_z, schwefel_y, Os[2], Mr[2], 1000.0 / 100.0, 1, r_flag);
  fit[2] = schwefel(schwefel_z);
  fit[2] *= 10000 / 4e+3;

  // bent_cigar(x, fit[3], Os[3], Mr[3], 1, r_flag);
  // fit[3] *= 10000 / 1e+30;

  std::vector<double> ellips_z(nx);
  std::vector<double> ellips_y(nx);
  ioh::problem::transformation::variables::scale_and_rotate(x, ellips_z, ellips_y, Os[4], Mr[4], 1.0, 1, r_flag);
  fit[4] = ellips(ellips_z);
  fit[4] *= 10000 / 1e+10;

  escaffer6(x, fit[5], Os[5], Mr[5], 1, r_flag);
  fit[5] *= 10000 / 2e+7;

  double f = cf_cal(x, Os, delta, bias, fit);
  return f;
}

} // namespace ioh::problem
