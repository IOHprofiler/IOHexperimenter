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

void shiftfunc(const std::vector<double> &x, std::vector<double> &xshift,
               const std::vector<double> &Os)
{
  for (size_t i = 0; i < x.size(); ++i)
  {
    xshift[i] = x[i] - Os[i];
  }
}

void rotatefunc(const std::vector<double> &x, std::vector<double> &xrot,
                const std::vector<double> &Mr)
{
  for (size_t i = 0; i < x.size(); ++i)
  {
    xrot[i] = 0;
    for (size_t j = 0; j < x.size(); ++j)
    {
      xrot[i] += x[j] * Mr[i * x.size() + j];
    }
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
      shiftfunc(x, y, Os);
      for (size_t i = 0; i < x.size(); ++i)
      {
        y[i] *= sh_rate;
      }
      rotatefunc(y, sr_x, Mr);
    } else
    {
      shiftfunc(x, sr_x, Os);
      for (size_t i = 0; i < x.size(); ++i)
      {
        sr_x[i] *= sh_rate;
      }
    }
  } else
  {
    if (r_flag)
    {
      for (size_t i = 0; i < x.size(); ++i)
      {
        y[i] = x[i] * sh_rate;
      }
      rotatefunc(y, sr_x, Mr);
    } else
    {
      for (size_t i = 0; i < x.size(); ++i)
      {
        sr_x[i] = x[i] * sh_rate;
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

inline double hgbat(const std::vector<double> &x, const std::vector<double> &Os,
                    const std::vector<double> &Mr, bool s_flag, bool r_flag)
{

  const double alpha = 1.0 / 4.0;

  std::vector<double> z(x.size()), y(x.size());

  scale_and_rotate(x, z, y, Os, Mr, 5.0 / 100.0, s_flag, r_flag);

  std::transform(z.begin(), z.end(), z.begin(),
                 [](double value) { return value - 1.0; });

  const double r2 = std::inner_product(z.begin(), z.end(), z.begin(), 0.0);
  const double sum_z = std::accumulate(z.begin(), z.end(), 0.0);

  const double term1 = std::pow(r2, 2.0);

  const double term2 = std::pow(sum_z, 2.0);

  const double term3 = std::pow(std::abs(term1 - term2), 2 * alpha);

  const double term4 = (0.5 * r2 + sum_z) / static_cast<double>(x.size());

  const double result = term3 + term4 + 0.5;

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
      [index = 0, tmp3](double acc, double z_val) mutable
      {
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

  std::vector<double> y(x.size()), z(x.size());
  scale_and_rotate(x, z, y, offsets, matrix, 1.0, should_shift, should_rotate);

  const double sum1 =
      z.empty() ? 0.0
                : -0.2 * std::sqrt(std::inner_product(z.begin(), z.end(),
                                                      z.begin(), 0.0) /
                                   static_cast<double>(z.size()));

  const double sum2 =
      z.empty() ? 0.0
                : std::accumulate(z.begin(), z.end(), 0.0,
                                  [](double acc, double value)
                                  {
                                    return acc + std::cos(2.0 * PI * value);
                                  }) /
                      static_cast<double>(z.size());

  result = E - 20.0 * std::exp(sum1) - std::exp(sum2) + 20.0;
}

inline void schwefel_func(const std::vector<double> &x, double &result,
                          const std::vector<double> &offsets,
                          const std::vector<double> &matrix, bool should_shift,
                          bool should_rotate)
{

  std::vector<double> y(x.size()), z(x.size());
  scale_and_rotate(x, z, y, offsets, matrix, 1000.0 / 100.0, should_shift,
                   should_rotate);

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
    } else
    {
      result -= z[i] * std::sin(std::pow(std::fabs(z[i]), 0.5));
    }
  }
  result += 4.189828872724338e+002 * x.size();
}

inline void schaffer_F7_func(const std::vector<double> &x, double &result,
                             std::vector<double> &y,
                             const std::vector<double> &offsets,
                             const std::vector<double> &matrix,
                             int should_shift, int should_rotate)
{

  // Check input sizes

  std::vector<double> z(x.size());
  double tmp;

  scale_and_rotate(x, z, y, offsets, matrix, 0.5 / 100, should_shift,
                   should_rotate);

  for (size_t i = 0; i < x.size() - 1; ++i)
  {

    auto y_squared_sum = y[i] * y[i] + y[i + 1] * y[i + 1];

    z[i] = std::sqrt(y_squared_sum);

    auto pow_val = std::pow(z[i], 0.2);

    tmp = std::sin(50.0 * pow_val);

    auto tmp_squared = tmp * tmp;

    auto z_rooted = std::sqrt(z[i]);

    auto val_addition = z_rooted * tmp_squared;

    result += z_rooted + val_addition;
  }

  result = result * result / (x.size() - 1) / (x.size() - 1);
}

inline void bent_cigar_func(const std::vector<double> &x, double &result,
                            const std::vector<double> &Os,
                            const std::vector<double> &Mr, bool s_flag,
                            bool r_flag)
{

  std::vector<double> z(x.size());
  std::vector<double> y(x.size());

  scale_and_rotate(x, z, y, Os, Mr, 1.0, s_flag, r_flag);

  result = z[0] * z[0];

  for (size_t i = 1; i < z.size(); ++i)
  {

    result += std::pow(10.0, 6.0) * z[i] * z[i];
  }
}

inline void happycat_func(const std::vector<double> &x, double &f,
                          const std::vector<double> &Os,
                          const std::vector<double> &Mr, int s_flag,
                          int r_flag)
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

  f = std::pow(std::fabs(r2 - x.size()), 2 * alpha) +
      (0.5 * r2 + sum_z) / x.size() + 0.5;
}

inline void grie_rosen_func(const std::vector<double> &x, double &f,
                            const std::vector<double> &Os,
                            const std::vector<double> &Mr, int s_flag,
                            int r_flag)
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

inline void griewank_func(const std::vector<double> &x, double &f,
                          const std::vector<double> &Os,
                          const std::vector<double> &Mr, int s_flag,
                          int r_flag)
{
  std::vector<double> z(x.size());
  std::vector<double> y(x.size());

  scale_and_rotate(x, z, y, Os, Mr, 600.0 / 100.0, s_flag, r_flag);

  double s =
      std::accumulate(z.begin(), z.end(), 0.0,
                      [](double sum, double val) { return sum + val * val; });

  double p = 1.0;
  for (size_t i = 0; i < x.size(); i++)
  {
    p *= cos(z[i] / sqrt(1.0 + i));
  }

  f = 1.0 + s / 4000.0 - p;
}

inline void escaffer6_func(const std::vector<double> &x, double &f,
                           const std::vector<double> &Os,
                           const std::vector<double> &Mr, int s_flag,
                           int r_flag)
{
  std::vector<double> z(x.size());
  std::vector<double> y(x.size());

  scale_and_rotate(x, z, y, Os, Mr, 1.0, s_flag, r_flag);

  f = 0.0;

  for (size_t i = 0; i < x.size() - 1; i++)
  {
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

void discus_func(const std::vector<double> &x, double &result,
                 const std::vector<double> &Os, const std::vector<double> &Mr,
                 bool s_flag, bool r_flag)
{
  constexpr double MULTIPLIER = std::pow(10.0, 6.0);

  auto nx = x.size();

  std::vector<double> z(nx);
  std::vector<double> y(nx);

  scale_and_rotate(x, z, y, Os, Mr, 1.0, s_flag, r_flag);

  result = MULTIPLIER * z[0] * z[0];
  for (size_t i = 1; i < nx; ++i)
  {
    result += z[i] * z[i];
  }
}

void ellips_func(const std::vector<double> &x, double &result,
                 const std::vector<double> &Os, const std::vector<double> &Mr,
                 bool s_flag, bool r_flag)
{
  constexpr double BASE = 10.0;
  constexpr double EXPONENT_SCALE = 6.0;

  auto nx = x.size();
  result = 0.0;

  std::vector<double> z(nx);
  std::vector<double> y(nx);

  scale_and_rotate(x, z, y, Os, Mr, 1.0, s_flag, r_flag);

  for (size_t i = 0; i < nx; ++i)
  {
    result += std::pow(BASE, EXPONENT_SCALE * i / (nx - 1)) * z[i] * z[i];
  }
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

  bent_cigar_func(
      std::vector<double>(y.begin() + G[0], y.begin() + G[0] + G_nx[0]), fit[0],
      Os, Mr, 0, 0);
  fit[1] =
      hgbat(std::vector<double>(y.begin() + G[1], y.begin() + G[1] + G_nx[1]),
            Os, Mr, 0, 0);
  rastrigin(std::vector<double>(y.begin() + G[2], y.begin() + G[2] + G_nx[2]),
            fit[2], Os, Mr, 0, 0);

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

inline void hf06(const std::vector<double> &x, double &f,
                 const std::vector<double> &Os, const std::vector<double> &Mr,
                 const std::vector<int> &S, int s_flag, int r_flag)
{
  const int cf_num = 5;
  std::vector<double> fit(cf_num, 0);
  std::vector<int> G_nx(cf_num);
  std::vector<int> G(cf_num);
  std::vector<double> Gp = {0.3, 0.2, 0.2, 0.1, 0.2};

  int tmp = 0;
  for (int i = 0; i < cf_num - 1; ++i)
  {
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
    if (i != G_nx.size() - 1)
      G_log << ", ";
  }
  G_log << "]";
  int i = 0;
  katsuura_func(
      std::vector<double>(y.begin() + G[i], y.begin() + G[i] + G_nx[i]), fit[i],
      Os, Mr, 0, 0);
  i = 1;
  happycat_func(
      std::vector<double>(y.begin() + G[i], y.begin() + G[i] + G_nx[i]), fit[i],
      Os, Mr, 0, 0);
  i = 2;
  grie_rosen_func(
      std::vector<double>(y.begin() + G[i], y.begin() + G[i] + G_nx[i]), fit[i],
      Os, Mr, 0, 0);
  i = 3;
  schwefel_func(
      std::vector<double>(y.begin() + G[i], y.begin() + G[i] + G_nx[i]), fit[i],
      Os, Mr, 0, 0);
  i = 4;
  ackley_func(std::vector<double>(y.begin() + G[i], y.begin() + G[i] + G_nx[i]),
              fit[i], Os, Mr, 0, 0);

  f = std::accumulate(fit.begin(), fit.end(), 0.0);
}

void cf_cal(const std::vector<double> &x, double &f,
            const std::vector<double> &Os, const std::vector<double> &delta,
            const std::vector<double> &bias, std::vector<double> &fit)
{
  int nx = x.size();
  int cf_num = fit.size();
  constexpr double INF = std::numeric_limits<double>::infinity();

  std::vector<double> w(cf_num);
  double w_max = 0.0;
  double w_sum = 0.0;

  for (int i = 0; i < cf_num; i++)
  {
    fit[i] += bias[i];
    w[i] = 0.0;

    for (int j = 0; j < nx; j++)
    {
      w[i] += std::pow(x[j] - Os[i * nx + j], 2.0);
    }

    if (w[i] != 0)
      w[i] = std::pow(1.0 / w[i], 0.5) *
             std::exp(-w[i] / (2.0 * nx * std::pow(delta[i], 2.0)));
    else
      w[i] = INF;

    w_max = std::max(w_max, w[i]);
  }

  for (const auto &weight : w)
  {
    w_sum += weight;
  }

  if (w_max == 0)
  {
    std::fill(w.begin(), w.end(), 1.0);
    w_sum = cf_num;
  }

  f = 0.0;
  for (int i = 0; i < cf_num; i++)
  {
    f += (w[i] / w_sum) * fit[i];
  }
}

void cf01(const std::vector<double> &x, double &f,
          const std::vector<double> &Os, const std::vector<double> &Mr,
          bool r_flag)
{
  std::vector<double> deltas = {10, 20, 30, 40, 50};
  std::vector<double> biases = {0, 200, 300, 100, 400};

  std::vector<double> fit(5);

  int nx = x.size();
  int i = 0;
  rosenbrock_func(
      x, fit[i],
      std::vector<double>(Os.begin() + i * nx, Os.begin() + (i + 1) * nx),
      std::vector<double>(Mr.begin() + i * nx * nx,
                          Mr.begin() + (i + 1) * nx * nx),
      1, r_flag);
  fit[i] = 10000 * fit[i] / 1e+4;

  i = 1;
  ellips_func(
      x, fit[i],
      std::vector<double>(Os.begin() + i * nx, Os.begin() + (i + 1) * nx),
      std::vector<double>(Mr.begin() + i * nx * nx,
                          Mr.begin() + (i + 1) * nx * nx),
      1, 0);
  fit[i] = 10000 * fit[i] / 1e+10;

  i = 2;
  bent_cigar_func(
      x, fit[i],
      std::vector<double>(Os.begin() + i * nx, Os.begin() + (i + 1) * nx),
      std::vector<double>(Mr.begin() + i * nx * nx,
                          Mr.begin() + (i + 1) * nx * nx),
      1, r_flag);
  fit[i] = 10000 * fit[i] / 1e+30;

  i = 3;
  discus_func(
      x, fit[i],
      std::vector<double>(Os.begin() + i * nx, Os.begin() + (i + 1) * nx),
      std::vector<double>(Mr.begin() + i * nx * nx,
                          Mr.begin() + (i + 1) * nx * nx),
      1, r_flag);
  fit[i] = 10000 * fit[i] / 1e+10;

  i = 4;
  ellips_func(
      x, fit[i],
      std::vector<double>(Os.begin() + i * nx, Os.begin() + (i + 1) * nx),
      std::vector<double>(Mr.begin() + i * nx * nx,
                          Mr.begin() + (i + 1) * nx * nx),
      1, 0);
  fit[i] = 10000 * fit[i] / 1e+10;

  cf_cal(x, f, Os, deltas, biases, fit);
}

void cf02(const std::vector<double> &x, double &f,
          const std::vector<double> &Os, const std::vector<double> &Mr,
          int r_flag)
{
  const int nx = x.size();
  const std::vector<double> delta = {20, 10, 10};
  const std::vector<double> bias = {0, 200, 100};

  std::vector<double> fit(3);

  schwefel_func(x, fit[0], std::vector<double>(Os.begin(), Os.begin() + nx),
                std::vector<double>(Mr.begin(), Mr.begin() + nx * nx), 1, 0);

  rastrigin(x, fit[1],
            std::vector<double>(Os.begin() + nx, Os.begin() + 2 * nx),
            std::vector<double>(Mr.begin() + nx * nx, Mr.begin() + 2 * nx * nx),
            1, r_flag);

  fit[2] = hgbat(
      x, std::vector<double>(Os.begin() + 2 * nx, Os.begin() + 3 * nx),
      std::vector<double>(Mr.begin() + 2 * nx * nx, Mr.begin() + 3 * nx * nx),
      1, r_flag);

  cf_cal(x, f, Os, delta, bias, fit);
}

void cf06(const std::vector<double> &x, double &f,
          const std::vector<double> &Os, const std::vector<double> &Mr,
          int r_flag)
{
  const int nx = x.size();
  const std::vector<double> delta = {20, 20, 30, 30, 20};
  const std::vector<double> bias = {0, 200, 300, 400, 200};

  std::vector<double> fit(5);

  // Explicit call to escaffer6_func
  escaffer6_func(x, fit[0], std::vector<double>(Os.begin(), Os.begin() + nx),
                 std::vector<double>(Mr.begin(), Mr.begin() + nx * nx), 1,
                 r_flag);
  fit[0] *= 10000 / 2e+7;

  // Explicit call to schwefel_func
  schwefel_func(
      x, fit[1], std::vector<double>(Os.begin() + nx, Os.begin() + 2 * nx),
      std::vector<double>(Mr.begin() + nx * nx, Mr.begin() + 2 * nx * nx), 1,
      r_flag);

  // Explicit call to griewank_func
  griewank_func(
      x, fit[2], std::vector<double>(Os.begin() + 2 * nx, Os.begin() + 3 * nx),
      std::vector<double>(Mr.begin() + 2 * nx * nx, Mr.begin() + 3 * nx * nx),
      1, r_flag);
  fit[2] *= 1000 / 100;

  // Explicit call to rosenbrock_func
  rosenbrock_func(
      x, fit[3], std::vector<double>(Os.begin() + 3 * nx, Os.begin() + 4 * nx),
      std::vector<double>(Mr.begin() + 3 * nx * nx, Mr.begin() + 4 * nx * nx),
      1, r_flag);

  // Explicit call to rastrigin
  rastrigin(
      x, fit[4], std::vector<double>(Os.begin() + 4 * nx, Os.begin() + 5 * nx),
      std::vector<double>(Mr.begin() + 4 * nx * nx, Mr.begin() + 5 * nx * nx),
      1, r_flag);
  fit[4] *= 10000 / 1e+3;

  cf_cal(x, f, Os, delta, bias, fit);
}

void cf07(const std::vector<double> &x, double &f,
          const std::vector<double> &Os, const std::vector<double> &Mr,
          int r_flag)
{
  std::vector<double> fit(6);
  std::vector<double> delta = {10, 20, 30, 40, 50, 60};
  std::vector<double> bias = {0, 300, 500, 100, 400, 200};

  // Explicit call to hgbat_func
  fit[0] =
      hgbat(x, std::vector<double>(Os.begin(), Os.begin() + x.size()),
            std::vector<double>(Mr.begin(), Mr.begin() + x.size() * x.size()),
            1, r_flag);
  fit[0] *= 10000 / 1000;

  // Explicit call to rastrigin_func
  rastrigin(
      x, fit[1],
      std::vector<double>(Os.begin() + x.size(), Os.begin() + 2 * x.size()),
      std::vector<double>(Mr.begin() + x.size() * x.size(),
                          Mr.begin() + 2 * x.size() * x.size()),
      1, r_flag);
  fit[1] *= 10000 / 1e+3;

  // Explicit call to schwefel_func
  schwefel_func(
      x, fit[2],
      std::vector<double>(Os.begin() + 2 * x.size(), Os.begin() + 3 * x.size()),
      std::vector<double>(Mr.begin() + 2 * x.size() * x.size(),
                          Mr.begin() + 3 * x.size() * x.size()),
      1, r_flag);
  fit[2] *= 10000 / 4e+3;

  // Explicit call to bent_cigar_func
  bent_cigar_func(
      x, fit[3],
      std::vector<double>(Os.begin() + 3 * x.size(), Os.begin() + 4 * x.size()),
      std::vector<double>(Mr.begin() + 3 * x.size() * x.size(),
                          Mr.begin() + 4 * x.size() * x.size()),
      1, r_flag);
  fit[3] *= 10000 / 1e+30;

  // Explicit call to ellips_func
  ellips_func(
      x, fit[4],
      std::vector<double>(Os.begin() + 4 * x.size(), Os.begin() + 5 * x.size()),
      std::vector<double>(Mr.begin() + 4 * x.size() * x.size(),
                          Mr.begin() + 5 * x.size() * x.size()),
      1, r_flag);
  fit[4] *= 10000 / 1e+10;

  // Explicit call to escaffer6_func
  escaffer6_func(
      x, fit[5],
      std::vector<double>(Os.begin() + 5 * x.size(), Os.begin() + 6 * x.size()),
      std::vector<double>(Mr.begin() + 5 * x.size() * x.size(),
                          Mr.begin() + 6 * x.size() * x.size()),
      1, r_flag);
  fit[5] *= 10000 / 2e+7;

  cf_cal(x, f, Os, delta, bias, fit);
}

} // namespace ioh::problem::cec
