#pragma once

#include <cmath>
#include <numeric>
#include <vector>

#include "cec_problem.hpp"

namespace ioh::problem::cec
{
  constexpr double INF = 1.0e99;
  constexpr double EPS = 1.0e-14;
  constexpr double E = 2.7182818284590452353602874713526625;
  constexpr double PI = 3.1415926535897932384626433832795029;

  void neu_shiftfunc(const std::vector<double> &x, std::vector<double> &xshift,
                     const std::vector<double> &Os)
  {
    for (size_t i = 0; i < x.size(); ++i)
    {
      xshift[i] = x[i] - Os[i];
      LOG("neu_shiftfunc, i: " + std::to_string(i) + ", xshift[" +
          std::to_string(i) + "]: " + std::to_string(xshift[i]));
    }
  }

  void neu_rotatefunc(const std::vector<double> &x, std::vector<double> &xrot,
                      const std::vector<double> &Mr)
  {
    for (size_t i = 0; i < x.size(); ++i)
    {
      xrot[i] = 0;
      for (size_t j = 0; j < x.size(); ++j)
      {
        xrot[i] += x[j] * Mr[i * x.size() + j];
      }
      LOG("neu_rotatefunc, i: " + std::to_string(i) + ", xrot[" +
          std::to_string(i) + "]: " + std::to_string(xrot[i]));
    }
  }

  void scale_and_rotate(const std::vector<double> &x, std::vector<double> &sr_x,
                        std::vector<double> &y, const std::vector<double> &Os,
                        const std::vector<double> &Mr, double sh_rate,
                        bool s_flag, bool r_flag)
  {
    if (s_flag)
    {
      if (r_flag)
      {
        neu_shiftfunc(x, y, Os);
        for (size_t i = 0; i < x.size(); ++i)
        {
          y[i] *= sh_rate;
          LOG("scale_and_rotate (s_flag && r_flag), i: " + std::to_string(i) +
              ", y[" + std::to_string(i) + "]: " + std::to_string(y[i]));
        }
        neu_rotatefunc(y, sr_x, Mr);
      } else {
        neu_shiftfunc(x, sr_x, Os);
        for (size_t i = 0; i < x.size(); ++i)
        {
          sr_x[i] *= sh_rate;
          LOG("scale_and_rotate (s_flag && !r_flag), i: " + std::to_string(i) +
              ", sr_x[" + std::to_string(i) + "]: " + std::to_string(sr_x[i]));
        }
      }
    } else {
      if (r_flag)
      {
        for (size_t i = 0; i < x.size(); ++i)
        {
          y[i] = x[i] * sh_rate;
          LOG("scale_and_rotate (!s_flag && r_flag), i: " + std::to_string(i) +
              ", y[" + std::to_string(i) + "]: " + std::to_string(y[i]));
        }
        neu_rotatefunc(y, sr_x, Mr);
      } else {
        for (size_t i = 0; i < x.size(); ++i)
        {
          sr_x[i] = x[i] * sh_rate;
          LOG("scale_and_rotate (!s_flag && !r_flag), i: " + std::to_string(i) +
              ", sr_x[" + std::to_string(i) + "]: " + std::to_string(sr_x[i]));
        }
      }
    }
  }

  // Note: Functions now use `std::vector<double>` which is dynamic and resizable.
  // Also, replaced int flags with bool for better readability and intent clarity.

  inline void rastrigin(const std::vector<double> &x, double &result,
                        const std::vector<double> &offsets,
                        const std::vector<double> &matrix, bool should_shift,
                        bool should_rotate)
  {
    std::vector<double> y(x.size()), z(x.size());
    scale_and_rotate(x, z, y, offsets, matrix, 5.12 / 100.0, should_shift,
                     should_rotate);

    result =
        std::accumulate(z.begin(), z.end(), 0.0, [](double acc, double value)
        {
          return acc + (value * value - 10.0 * std::cos(2.0 * PI * value) + 10.0);
        });
    LOG("result:" << result);
  }

  inline void rosenbrock_func(const std::vector<double> &x, double &result,
                              const std::vector<double> &offsets,
                              const std::vector<double> &matrix,
                              bool should_shift, bool should_rotate)
  {
    std::vector<double> y(x.size()), z(x.size());
    scale_and_rotate(x, z, y, offsets, matrix, 2.048 / 100.0, should_shift,
                     should_rotate);

    result = 0.0;
    z[0] += 1.0;
    for (size_t i = 0; i < x.size() - 1; ++i)
    {
      z[i + 1] += 1.0;
      auto tmp1 = z[i] * z[i] - z[i + 1];
      auto tmp2 = z[i] - 1.0;
      result += 100.0 * tmp1 * tmp1 + tmp2 * tmp2;
    }
  }

  inline void levy_func(const std::vector<double> &x, double &result,
                        const std::vector<double> &offsets,
                        const std::vector<double> &matrix, bool should_shift,
                        bool should_rotate)
  {
    std::vector<double> y(x.size()), z(x.size()), w(x.size());
    scale_and_rotate(x, z, y, offsets, matrix, 5.12 / 100, should_shift,
                     should_rotate);

    for (size_t i = 0; i < x.size(); ++i)
    {
      w[i] = 1.0 + (z[i] - 0.0) / 4.0;
    }

    double term1 = std::pow(std::sin(PI * w[0]), 2);
    double term3 = std::pow((w.back() - 1), 2) *
                   (1 + std::pow(std::sin(2 * PI * w.back()), 2));
    double sum = 0.0;

    for (size_t i = 0; i < x.size() - 1; ++i)
    {
      auto wi = w[i];
      auto newv =
          std::pow((wi - 1), 2) * (1 + 10 * std::pow(std::sin(PI * wi + 1), 2));
      sum += newv;
    }

    result = term1 + sum + term3;
  }

  inline void zakharov_func(const std::vector<double> &x, double &result,
                            const std::vector<double> &offsets,
                            const std::vector<double> &matrix, bool should_shift,
                            bool should_rotate)
  {
    std::vector<double> y(x.size()), z(x.size());
    scale_and_rotate(x, z, y, offsets, matrix, 1.0, should_shift, should_rotate);

    double sum1 = std::inner_product(z.begin(), z.end(), z.begin(), 0.0);
    size_t i = 0; // Declare i outside the lambda.
    double sum2 = std::accumulate(z.begin(), z.end(), 0.0,
                                  [&i, &z](double accum, const double &xi)
                                  {
                                    auto result = accum + 0.5 * (++i) * xi;
                                    if (i == z.size())
                                      i = 0;
                                    return result;
                                  });
    result = sum1 + std::pow(sum2, 2) + std::pow(sum2, 4);
  }

  void shiftfunc(const double *x, double *xshift, int nx, const double *Os)
  {
    for (int i = 0; i < nx; i++)
    {
      xshift[i] = x[i] - Os[i];
    }
  }

  void rotatefunc(const double *x, double *xrot, int nx, const double *Mr)
  {
    for (int i = 0; i < nx; i++)
    {
      xrot[i] = 0;
      for (int j = 0; j < nx; j++)
      {
        xrot[i] = xrot[i] + x[j] * Mr[i * nx + j];
      }
    }
  }

  void sr_func(const double *x, double *sr_x, double *y, int nx, const double *Os,
               const double *Mr, double sh_rate, int s_flag, int r_flag)
  {
    if (s_flag == 1)
    {
      if (r_flag == 1)
      {
        shiftfunc(x, y, nx, Os);
        for (int i = 0; i < nx; i++)
        {
          y[i] = y[i] * sh_rate;
        }
        rotatefunc(y, sr_x, nx, Mr);
      } else {
        shiftfunc(x, sr_x, nx, Os);
        for (int i = 0; i < nx; i++)
        {
          sr_x[i] = sr_x[i] * sh_rate;
        }
      }
    } else {
      if (r_flag == 1)
      {
        for (int i = 0; i < nx; i++)
        {
          y[i] = x[i] * sh_rate;
        }
        rotatefunc(y, sr_x, nx, Mr);
      } else {
        for (int i = 0; i < nx; i++)
        {
          sr_x[i] = x[i] * sh_rate;
        }
      }
    }
  }

  inline double hgbat(const std::vector<double> &x, const std::vector<double> &Os,
                      const std::vector<double> &Mr, bool s_flag, bool r_flag)
  {
    LOG("[hgbat] Entered function.");

    const double alpha = 1.0 / 4.0;

    std::vector<double> z(x.size()), y(x.size());

    scale_and_rotate(x, z, y, Os, Mr, 5.0 / 100.0, s_flag, r_flag);
    LOG("[hgbat] Performed shift and rotate.");

    std::transform(z.begin(), z.end(), z.begin(),
                   [](double value) { return value - 1.0; });

    // Use std::for_each for logging:
    for (size_t i = 0; i < z.size(); ++i)
      LOG("[hgbat] z[" + std::to_string(i) + "]: " + std::to_string(z[i]));

    const double r2 = std::inner_product(z.begin(), z.end(), z.begin(), 0.0);
    const double sum_z = std::accumulate(z.begin(), z.end(), 0.0);

    LOG("[hgbat] r2: " + std::to_string(r2) +
        ", sum_z: " + std::to_string(sum_z));

    const double term1 = std::pow(r2, 2.0);
    LOG("[hgbat] term1 (pow(r2, 2.0)): " + std::to_string(term1));

    const double term2 = std::pow(sum_z, 2.0);
    LOG("[hgbat] term2 (pow(sum_z, 2.0)): " + std::to_string(term2));

    const double term3 = std::pow(std::abs(term1 - term2), 2 * alpha);
    LOG("[hgbat] term3 (pow(fabs(term1 - term2), 2*alpha)): " +
        std::to_string(term3));

    const double term4 = (0.5 * r2 + sum_z) / static_cast<double>(x.size());
    LOG("[hgbat] term4 ((0.5*r2 + sum_z)/nx): " + std::to_string(term4));

    const double result = term3 + term4 + 0.5;
    LOG("[hgbat] result: " + std::to_string(result));

    return result;
  }

  inline void katsuura_func(const std::vector<double> &x, double &result,
                            const std::vector<double> &offsets,
                            const std::vector<double> &matrix, bool should_shift,
                            bool should_rotate)
  {
    std::vector<double> y(x.size()), z(x.size());
    scale_and_rotate(x, z, y, offsets, matrix, 5.0 / 100.0, should_shift,
                     should_rotate);

    double tmp3 = std::pow(static_cast<double>(x.size()), 1.2);

    result = std::accumulate(
        z.cbegin(), z.cend(), 1.0,
        [index = 0, tmp3](double acc, double z_val) mutable {
          double temp = 0.0;
          for (int j = 1; j <= 32; ++j)
          {
            double tmp1 = std::pow(2.0, j);
            double tmp2 = tmp1 * z_val;
            temp += std::fabs(tmp2 - std::floor(tmp2 + 0.5)) / tmp1;
          }
          return acc * std::pow(1.0 + (++index) * temp, 10.0 / tmp3);
        });

    double tmp1 = 10.0 / x.size() / x.size();
    result = result * tmp1 - tmp1;
  }

  inline void ackley_func(const std::vector<double> &x, double &result,
                          const std::vector<double> &offsets,
                          const std::vector<double> &matrix, bool should_shift,
                          bool should_rotate)
  {
    LOG("[ackley_func] Entered function." << std::endl);

    std::vector<double> y(x.size()), z(x.size());
    scale_and_rotate(x, z, y, offsets, matrix, 1.0, should_shift, should_rotate);

    LOG("[ackley_func] Performed shift and rotate." << std::endl);

    const double sum1 = z.empty()
        ? 0.0
        : -0.2 * std::sqrt(std::inner_product(z.begin(), z.end(), z.begin(), 0.0) / static_cast<double>(z.size()));

    const double sum2 = z.empty()
        ? 0.0
        : std::accumulate(z.begin(), z.end(), 0.0,
                          [](double acc, double value)
                          {
                              return acc + std::cos(2.0 * PI * value);
                          }) / static_cast<double>(z.size());

    LOG("[ackley_func] sum1: " << sum1 << std::endl);
    LOG("[ackley_func] sum2: " << sum2 << std::endl);
    result = E - 20.0 * std::exp(sum1) - std::exp(sum2) + 20.0;

    LOG("[ackley_func] Exited function. Result: " << result << std::endl);
  }

  inline void schwefel_func(const std::vector<double> &x, double &result,
                            const std::vector<double> &offsets,
                            const std::vector<double> &matrix, bool should_shift,
                            bool should_rotate)
  {
    LOG("[schwefel_func] Entered function." << std::endl);

    std::vector<double> y(x.size()), z(x.size());
    scale_and_rotate(x, z, y, offsets, matrix, 1000.0 / 100.0, should_shift,
                     should_rotate);

    LOG("[schwefel_func] Performed shift and rotate." << std::endl);

    result = 0.0;
    for (size_t i = 0; i < x.size(); ++i)
    {
      z[i] += 4.209687462275036e+002;
      double tmp;

      if (z[i] > 500)
      {
        result -= (500.0 - std::fmod(z[i], 500)) *
                  std::sin(std::pow(500.0 - std::fmod(z[i], 500), 0.5));
        tmp = (z[i] - 500.0) / 100;
        result += tmp * tmp / x.size();
      } else if (z[i] < -500)
      {
        result -=
            (-500.0 + std::fmod(std::fabs(z[i]), 500)) *
            std::sin(std::pow(500.0 - std::fmod(std::fabs(z[i]), 500), 0.5));
        tmp = (z[i] + 500.0) / 100;
        result += tmp * tmp / x.size();
      } else {
        result -= z[i] * std::sin(std::pow(std::fabs(z[i]), 0.5));
      }

      LOG("[schwefel_func] z[" << i << "]: " << z[i] << ", result: " << result
                               << std::endl);
    }
    result += 4.189828872724338e+002 * x.size();

    LOG("[schwefel_func] Exited function. Result: " << result << std::endl);
  }

  inline void schaffer_F7_func(const std::vector<double> &x, double &result,
                               std::vector<double> &y,
                               const std::vector<double> &offsets,
                               const std::vector<double> &matrix,
                               int should_shift, int should_rotate)
  {

    LOG("[schaffer_F7_func] Entered function.");

    // Check input sizes
    LOG("[schaffer_F7_func] Size of x: " + std::to_string(x.size()));
    LOG("[schaffer_F7_func] Size of offsets: " + std::to_string(offsets.size()));
    LOG("[schaffer_F7_func] Size of matrix: " + std::to_string(matrix.size()));

    std::vector<double> z(x.size());
    double tmp;

    for (size_t j = 0; j < x.size(); j++)
    {
      LOG("[schaffer_F7_func] x[" + std::to_string(j) +
          "]: " + std::to_string(x[j]));
      LOG("[schaffer_F7_func] y[" + std::to_string(j) +
          "]: " + std::to_string(y[j]));
      LOG("[schaffer_F7_func] z[" + std::to_string(j) +
          "]: " + std::to_string(z[j]));
    }

    scale_and_rotate(x, z, y, offsets, matrix, 0.5 / 100, should_shift,
                     should_rotate);
    LOG("[schaffer_F7_func] Completed scale_and_rotate execution.");

    // Log x, y, and z values before the loop
    for (size_t j = 0; j < x.size(); j++)
    {
      LOG("[schaffer_F7_func] x[" + std::to_string(j) +
          "]: " + std::to_string(x[j]));
      LOG("[schaffer_F7_func] y[" + std::to_string(j) +
          "]: " + std::to_string(y[j]));
      LOG("[schaffer_F7_func] z[" + std::to_string(j) +
          "]: " + std::to_string(z[j]));
    }

    for (size_t i = 0; i < x.size() - 1; ++i)
    {
      LOG("[schaffer_F7_func] Processing index " + std::to_string(i));

      auto y_squared_sum = y[i] * y[i] + y[i + 1] * y[i + 1];
      LOG("[schaffer_F7_func] Computed y_squared_sum for index " +
          std::to_string(i) + ": " + std::to_string(y_squared_sum));

      z[i] = std::sqrt(y_squared_sum);
      LOG("[schaffer_F7_func] Updated z[" + std::to_string(i) +
          "] using square root of y_squared_sum: " + std::to_string(z[i]));

      auto pow_val = std::pow(z[i], 0.2);
      LOG("[schaffer_F7_func] Computed power value (z[" + std::to_string(i) +
          "]^0.2): " + std::to_string(pow_val));

      tmp = std::sin(50.0 * pow_val);
      LOG("[schaffer_F7_func] Computed sin(50 * pow_val): " +
          std::to_string(tmp));

      auto tmp_squared = tmp * tmp;
      LOG("[schaffer_F7_func] Computed tmp_squared: " +
          std::to_string(tmp_squared));

      auto z_rooted = std::sqrt(z[i]);
      LOG("[schaffer_F7_func] Recomputed square root for z[" + std::to_string(i) +
          "]: " + std::to_string(z_rooted));

      auto val_addition = z_rooted * tmp_squared;
      LOG("[schaffer_F7_func] Computed val_addition using z_rooted and "
          "tmp_squared: " +
          std::to_string(val_addition));

      result += z_rooted + val_addition;
      LOG("[schaffer_F7_func] Incremented result by (z_rooted + val_addition). "
          "Cumulative result after iteration " +
          std::to_string(i) + ": " + std::to_string(result));
    }

    LOG("[schaffer_F7_func] Completed loop iterations. Result so far: " +
        std::to_string(result));

    double original_result = result;
    LOG("[schaffer_F7_func] Storing intermediate result before final "
        "computation: " +
        std::to_string(original_result));

    result = result * result / (x.size() - 1) / (x.size() - 1);
    LOG("[schaffer_F7_func] Final computed result after adjustment: " +
        std::to_string(result));

    LOG("[schaffer_F7_func] Exited function. Result: " + std::to_string(result));
  }

  inline void bent_cigar_func(const std::vector<double> &x, double &result,
                              const std::vector<double> &Os,
                              const std::vector<double> &Mr, int s_flag,
                              int r_flag)
  {

    std::vector<double> z(x.size());
    std::vector<double> y(x.size());

    scale_and_rotate(x, z, y, Os, Mr, 1.0, s_flag, r_flag);

    result = z[0] * z[0];
    LOG("bent_cigar_func result (using z[0]): " + std::to_string(result));

    for (size_t i = 1; i < z.size(); ++i)
    {
      LOG("Iteration: " + std::to_string(i));
      LOG("z[" + std::to_string(i) + "]: " + std::to_string(z[i]));
      LOG("x[" + std::to_string(i) + "]: " + std::to_string(x[i]));

      result += std::pow(10.0, 6.0) * z[i] * z[i];
      LOG("result after iteration " + std::to_string(i) + ": " +
          std::to_string(result));
    }
  }

  inline void happycat_func(const std::vector<double>& x, double& f,
                            const std::vector<double>& Os, const std::vector<double>& Mr,
                            int s_flag, int r_flag)
  {
      const double alpha = 1.0 / 8.0;
      std::vector<double> z(x.size());
      std::vector<double> y(x.size());

      scale_and_rotate(x, z, y, Os, Mr, 5.0 / 100.0, s_flag, r_flag);

      double r2 = 0.0;
      double sum_z = 0.0;

      for (size_t i = 0; i < x.size(); ++i)
      {
          z[i] -= 1.0;
          r2 += z[i] * z[i];
          sum_z += z[i];
      }

      f = std::pow(std::fabs(r2 - x.size()), 2 * alpha) + (0.5 * r2 + sum_z) / x.size() + 0.5;
  }

  inline void grie_rosen_func(const std::vector<double>& x, double& f,
                              const std::vector<double>& Os, const std::vector<double>& Mr,
                              int s_flag, int r_flag)
  {
      std::vector<double> z(x.size());
      std::vector<double> y(x.size());

      scale_and_rotate(x, z, y, Os, Mr, 5.0 / 100.0, s_flag, r_flag);

      z[0] += 1.0;
      f = 0.0;

      for (size_t i = 0; i < x.size() - 1; ++i)
      {
          z[i + 1] += 1.0;
          double tmp1 = z[i] * z[i] - z[i + 1];
          double tmp2 = z[i] - 1.0;
          double temp = 100.0 * tmp1 * tmp1 + tmp2 * tmp2;
          f += (temp * temp) / 4000.0 - std::cos(temp) + 1.0;
      }

      double tmp1 = z.back() * z.back() - z[0];
      double tmp2 = z.back() - 1.0;
      double temp = 100.0 * tmp1 * tmp1 + tmp2 * tmp2;
      f += (temp * temp) / 4000.0 - std::cos(temp) + 1.0;
  }

  inline void hf02(const std::vector<double> &x, double &f,
                   const std::vector<double> &Os, const std::vector<double> &Mr,
                   const std::vector<int> &S, bool s_flag, bool r_flag)
  {
    int nx = x.size();

    std::vector<double> z(nx);
    std::vector<double> y(nx);
    int cf_num = 3;
    std::vector<double> fit(cf_num);
    std::vector<int> G(cf_num);
    std::vector<int> G_nx(cf_num);
    std::vector<double> Gp = {0.4, 0.4, 0.2};

    int tmp = 0;
    for (int i = 0; i < cf_num - 1; ++i)
    {
      G_nx[i] = std::ceil(Gp[i] * nx);
      tmp += G_nx[i];
    }
    G_nx[cf_num - 1] = nx - tmp;

    G[0] = 0;
    for (int i = 1; i < cf_num; ++i)
    {
      G[i] = G[i - 1] + G_nx[i - 1];
    }

    scale_and_rotate(x, z, y, Os, Mr, 1.0, s_flag, r_flag);

    for (int i = 0; i < nx; ++i)
    {
      y[i] = z[S[i] - 1];
    }

    bent_cigar_func
    (
      std::vector<double>(y.begin() + G[0], y.begin() + G[0] + G_nx[0]), fit[0],
      Os, Mr, 0, 0
    );
    fit[1] = hgbat(std::vector<double>(y.begin() + G[1], y.begin() + G[1] + G_nx[1]), Os, Mr, 0, 0);
    rastrigin
    (
      std::vector<double>(y.begin() + G[2], y.begin() + G[2] + G_nx[2]),
      fit[2], Os, Mr, 0, 0
    );

    f = 0.0;
    for (const auto &val : fit)
    {
      f += val;
    }
  }

  inline void hf10(const std::vector<double> &x, double &f,
                   const std::vector<double> &Os, const std::vector<double> &Mr,
                   const std::vector<int> &S, bool s_flag, bool r_flag)
  {
    const int cf_num = 6;
    std::vector<double> z(x.size());
    std::vector<double> y(x.size());
    std::vector<double> fit(cf_num, 0.0);
    std::vector<int> G_nx(cf_num);
    std::vector<int> G(cf_num);
    std::vector<double> Gp = {0.1, 0.2, 0.2, 0.2, 0.1, 0.2};
    int nx = x.size();

    int tmp = 0;
    for (int i = 0; i < cf_num - 1; i++)
    {
      G_nx[i] = static_cast<int>(std::ceil(Gp[i] * nx));
      tmp += G_nx[i];
    }
    G_nx[cf_num - 1] = nx - tmp;

    G[0] = 0;
    for (int i = 1; i < cf_num; i++)
    {
      G[i] = G[i - 1] + G_nx[i - 1];
    }

    scale_and_rotate(x, z, y, Os, Mr, 1.0, s_flag, r_flag);

    for (int i = 0; i < nx; i++)
    {
      y[i] = z[S[i] - 1];
    }

    std::vector<double> hgbat_func_x(y.begin() + G[0], y.begin() + G[1]);
    fit[0] = hgbat(hgbat_func_x, Os, Mr, 0, 0);

    std::vector<double> katsuura_func_x(y.begin() + G[1], y.begin() + G[2]);
    katsuura_func(katsuura_func_x, fit[1], Os, Mr, 0, 0);

    std::vector<double> ackley_func_x(y.begin() + G[2], y.begin() + G[3]);
    ackley_func(ackley_func_x, fit[2], Os, Mr, 0, 0);

    std::vector<double> rastrigin_x(y.begin() + G[3], y.begin() + G[4]);
    rastrigin(rastrigin_x, fit[3], Os, Mr, 0, 0);

    std::vector<double> schwefel_func_x(y.begin() + G[4], y.begin() + G[5]);
    schwefel_func(schwefel_func_x, fit[4], Os, Mr, 0, 0);

    std::vector<double> schaffer_x(y.begin() + G[5], y.end());
    std::vector<double> schaffer_y(y.begin(), y.end());
    schaffer_F7_func(schaffer_x, fit[5], schaffer_y, Os, Mr, 0, 0);

    f = std::accumulate(fit.begin(), fit.end(), 0.0);
  }

  inline void hf06(const std::vector<double> &x, double &f, const std::vector<double> &Os,
            const std::vector<double> &Mr, const std::vector<int> &S, int s_flag, int r_flag)
  {
    const int cf_num = 5;
    std::vector<double> fit(cf_num, 0);
    std::vector<int> G_nx(cf_num);
    std::vector<int> G(cf_num);
    std::vector<double> Gp = {0.3, 0.2, 0.2, 0.1, 0.2};

    int tmp = 0;
    for (int i = 0; i < cf_num - 1; ++i) {
        G_nx[i] = std::ceil(Gp[i] * x.size());
        tmp += G_nx[i];
    }
    G_nx[cf_num - 1] = x.size() - tmp;
    if (G_nx[cf_num - 1] < 0)
    {
      G_nx[cf_num - 1] = 0;
    }

    G[0] = 0;
    for (int i = 1; i < cf_num; ++i)
    {
        G[i] = G[i - 1] + G_nx[i - 1];
    }

    std::vector<double> z(x.size());
    std::vector<double> y(x.size());
    scale_and_rotate(x, z, y, Os, Mr, 1.0, s_flag, r_flag);

    for (size_t i = 0; i < x.size(); ++i)
    {
        y[i] = z[S[i] - 1];
    }

    // Log the G vector
    std::stringstream G_log;
    G_log << "G_nx: [";
    for (size_t i = 0; i < G_nx.size(); ++i)
    {
        G_log << G_nx[i];
        if (i != G_nx.size() - 1) G_log << ", ";
    }
    G_log << "]";
    LOG(G_log.str());
    int i = 0;
    katsuura_func
    (
      std::vector<double>(y.begin() + G[i], y.begin() + G[i] + G_nx[i]), fit[i],
      Os, Mr, 0, 0
    );
    i = 1;
    happycat_func
    (
      std::vector<double>(y.begin() + G[i], y.begin() + G[i] + G_nx[i]), fit[i],
      Os, Mr, 0, 0
    );
    i = 2;
    grie_rosen_func
    (
      std::vector<double>(y.begin() + G[i], y.begin() + G[i] + G_nx[i]), fit[i],
      Os, Mr, 0, 0
    );
    i = 3;
    schwefel_func
    (
      std::vector<double>(y.begin() + G[i], y.begin() + G[i] + G_nx[i]), fit[i],
      Os, Mr, 0, 0
    );
    i = 4;
    ackley_func
    (
      std::vector<double>(y.begin() + G[i], y.begin() + G[i] + G_nx[i]), fit[i],
      Os, Mr, 0, 0
    );

    f = std::accumulate(fit.begin(), fit.end(), 0.0);
  }

} // namespace ioh::problem::cec
