#pragma once

#include "ioh/problem/structures.hpp"
#include "ioh/problem/utils.hpp"

/* Transformation namespace */
namespace ioh::problem::transformation {
namespace objective {
/**
 * \brief Exclusive or operation on two integers, which should either be 0 or 1
 * \param x1 first operand
 * \param x2 second operand
 * \return the result of xor
 */
inline int exclusive_or(const int x1, const int x2) {
  return static_cast<int>(x1 != x2);
}

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
inline double oscillate(const double y, const double factor = 0.1) {
  if (y != 0.0) {
    const auto log_y = log(fabs(y)) / factor;
    if (y > 0)
      return pow(exp(log_y + 0.49 * (sin(log_y) + sin(0.79 * log_y))), factor);
    return -pow(exp(log_y + 0.49 * (sin(0.55 * log_y) + sin(0.31 * log_y))),
                factor);
  }
  return y;
}

using Transformation = std::function<double(double, double)>;

/**
 * \brief applies a given transformation method t(double y, double a) with a
 * random number for a. \param t A transformation method \param y the raw y
 * value \param seed the seed for the uniform random number generator \param lb
 * the lower bound for the random number \param ub the upper bound for the
 * random number \return the transformed y value
 */
inline double uniform(const Transformation &t, const double y, const int seed,
                      const double lb, const double ub) {
  const auto scalar = common::random::pbo::uniform(1, seed, lb, ub).at(0);
  return t(y, scalar);
}

/**
 * \brief penalizes the objective value for x when it is out of bounds, weighed
 * by a constant factor \param x the object in the search space \param lb the
 * lower bound \param ub the upper bound \param factor the weight \param y the
 * raw y value \return the penalized y value
 */
inline double penalize(const std::vector<double> &x, const double lb,
                       const double ub, const double factor, const double y) {
  auto penalty = 0.0;
  for (const auto xi : x) {
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
// * \brief penalizes the objective value for x when it is out of bounds,
// weighed by a constant factor
// * \param x the object in the search space
// * \param constraint the bounds of the problem
// * \param factor the weight
// * \param y the raw y value
// * \return the penalized y value
// */
// template <typename T>
// double penalize(const std::vector<double> &x, const BoxConstraint<T>
// constraint, const double factor,
//                const double y)
//{
//    return penalize(x, static_cast<double>(constraint.lb.at(0)),
//    static_cast<double>(constraint.ub.at(0)),
//                    factor, y);
//}
} // namespace objective

namespace variables {
/**
 * \brief randomly flips a bit
 * \param x raw variables
 * \param seed seed for the random flip
 */
inline void random_flip(std::vector<int> &x, const int seed) {
  const auto n = static_cast<int>(x.size());
  const auto rx = common::random::pbo::uniform(n, seed);

  for (auto i = 0; i < n; ++i)
    x[i] = objective::exclusive_or(
        x[i], static_cast<int>(2.0 * floor(1e4 * rx[i]) / 1e4));
}

/**
 * \brief randomly reorder the elements from x
 * \param x raw variables
 * \param seed seed for the random flip
 */
inline void random_reorder(std::vector<int> &x, const int seed) {
  std::vector<int> index(x.size());
  std::iota(index.begin(), index.end(), 0);

  const auto n = static_cast<int>(x.size());
  const auto rx = common::random::pbo::uniform(n, seed);
  const auto copy_x = x;

  for (auto i = 0; i != n; ++i) {
    const auto t = static_cast<int>(floor(rx[i] * n));
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
inline std::vector<int> random_reorder_reset(const std::vector<int> &x_1,
                                             const int seed) {
  std::vector<int> x(x_1.size());
  std::vector<int> index(x_1.size());
  std::iota(index.begin(), index.end(), 0);

  const auto n = static_cast<int>(x_1.size());
  const auto rx = common::random::pbo::uniform(n, seed);

  for (auto i = 0; i != n; ++i) {
    const auto t = static_cast<int>(floor(rx[i] * n));
    const auto temp = index[0];
    index[0] = index[t];
    index[t] = temp;
  }
  for (auto i = 0; i < n; ++i)
    x[index[i]] = x_1[i];
  return x;
}

/**
 * \brief Affine transformation for x using matrix M and vector B
 * \param x raw variables
 * \param m transformation matrix
 * \param b transformation vector
 */
inline void affine(std::vector<double> &x,
                   const std::vector<std::vector<double>> &m,
                   const std::vector<double> &b) {
  auto temp_x = x;
  for (size_t i = 0; i < x.size(); ++i) {
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
inline void asymmetric(std::vector<double> &x, const double beta) {
  const auto n_eff = static_cast<double>(x.size()) - 1.0;
  for (auto i = 0; i < static_cast<int>(x.size()); ++i)
    if (x[i] > 0.0)
      x[i] = pow(x[i], 1.0 + beta * i / n_eff * sqrt(x[i]));
}

/**
 * \brief brs transformation on x
 * \param x raw variables
 */
inline void brs(std::vector<double> &x) {
  const auto n_eff = static_cast<double>(x.size()) - 1.0;
  for (auto i = 0; i < static_cast<int>(x.size()); ++i) {
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
inline void conditioning(std::vector<double> &x, const double alpha) {
  const auto n_eff = static_cast<double>(x.size()) - 1.0;
  for (auto i = 0; i < static_cast<int>(x.size()); ++i)
    x[i] = pow(alpha, 0.5 * i / n_eff) * x[i];
}

/**
 * \brief oscillate each variable in x
 * \param x raw variables
 * \param alpha  the factor of oscillation
 */
inline void oscillate(std::vector<double> &x, const double alpha = 0.1) {
  for (auto &xi : x)
    xi = objective::oscillate(xi, alpha);
}

/**
 * \brief scale x by a scalar
 * \param x raw variables
 * \param scalar the factor to scale x by
 */
inline void scale(std::vector<double> &x, const double scalar) {
  for (auto &xi : x)
    xi = scalar * xi;
}

/**
 * \brief subtract an offset from each xi
 * \param x raw variables
 * \param offset a vector of offsets for each xi
 */
inline void subtract(std::vector<double> &x,
                     const std::vector<double> &offset) {
  for (auto i = 0; i < static_cast<int>(x.size()); ++i)
    x[i] = x[i] - offset[i];
}

/**
 * \brief randomly reverse the sign for each xi
 * \param x raw variables
 * \param seed for generating the random vector
 */
inline void random_sign_flip(std::vector<double> &x, const long seed) {
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
inline void z_hat(std::vector<double> &x, const std::vector<double> &xopt) {
  const auto temp_x = x;
  for (size_t i = 1; i < x.size(); ++i)
    x[i] = temp_x[i] + 0.25 * (temp_x[i - 1] - 2.0 * fabs(xopt[i - 1]));
}

/**
 * @brief Performs a series of transformations on the input vector `x`, including scaling and either rotation or affine transformation, based on the flags provided.
 *
 * This function is a part of the IOHexperimenter's problem transformations module, which is used to apply various transformations on optimization problem variables. The function takes an input vector `x` and applies a series of transformations including scaling and either rotation (using a rotation matrix `Mr`) or an affine transformation. The transformed vector is stored in `sr_x`.
 *
 * @param x A constant reference to the input vector that needs to be transformed. It represents the raw variables of the optimization problem.
 * @param sr_x A reference to the vector where the final transformed variables will be stored. This vector is expected to be initialized with the same size as `x`.
 * @param y A reference to a vector used as a temporary storage during the transformation process. This vector is expected to be initialized with the same size as `x`.
 * @param Os A constant reference to a vector used in the subtract transformation. It represents the offset values for each variable in `x`.
 * @param Mr A constant reference to a 2D vector representing the rotation matrix or the matrix used in the affine transformation.
 * @param sh_rate A double representing the scaling factor applied to each variable in `x` during the scaling transformation.
 * @param s_flag A boolean flag indicating whether the scaling transformation should be applied. If true, scaling is applied; otherwise, it is skipped.
 * @param r_flag A boolean flag indicating whether the rotation or affine transformation should be applied. If true, rotation or affine transformation is applied; otherwise, it is skipped.
 *
 * @note The function uses the `subtract` and `affine` functions for the subtract and affine transformations, respectively.
 * @note The `b` vector is initialized to a zero vector as it is used in the affine transformation to add a constant vector to `x`, but in this case, it acts as a no-op.
 *
 * @return void
 */
inline void scale_and_rotate(const std::vector<double> &x,
                             std::vector<double> &sr_x, std::vector<double> &y,
                             const std::vector<double> &Os,
                             const std::vector<std::vector<double>> &Mr,
                             double sh_rate, bool s_flag, bool r_flag) {
  std::vector<double> b(x.size(), 0.0);  // Create a zero vector for the affine transformation
  if (s_flag) {
    if (r_flag) {
      y = x;  // Copy x to y before calling subtract
      subtract(y, Os);  // Replaced shiftfunc with subtract
      for (size_t i = 0; i < x.size(); ++i) {
        y[i] *= sh_rate;
      }
      std::vector<double> y_copy = y;  // Create a copy of y to preserve the original values
      affine(y_copy, Mr, b);  // Replaced rotatefunc with affine
      sr_x = y_copy;  // Copy the result back to sr_x
    } else {
      sr_x = x;  // Copy x to sr_x before calling subtract
      subtract(sr_x, Os);  // Replaced shiftfunc with subtract
      for (size_t i = 0; i < x.size(); ++i) {
        sr_x[i] *= sh_rate;
      }
    }
  } else {
    if (r_flag) {
      for (size_t i = 0; i < x.size(); ++i) {
        y[i] = x[i] * sh_rate;
      }
      std::vector<double> y_copy = y;  // Create a copy of y to preserve the original values
      affine(y_copy, Mr, b);  // Replaced rotatefunc with affine
      sr_x = y_copy;  // Copy the result back to sr_x
    } else {
      for (size_t i = 0; i < x.size(); ++i) {
        sr_x[i] = x[i] * sh_rate;
      }
    }
  }
}

} // namespace variables
} // namespace ioh::problem::transformation
