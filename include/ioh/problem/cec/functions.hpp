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
                const std::vector<std::vector<double>> &Mr)
{
  for (size_t i = 0; i < x.size(); ++i)
  {
    xrot[i] = 0;
    for (size_t j = 0; j < x.size(); ++j)
    {
      xrot[i] += x[j] * Mr[i][j];
    }
  }
}

void scale_and_rotate(const std::vector<double> &x, std::vector<double> &sr_x,
                      std::vector<double> &y, const std::vector<double> &Os,
                      const std::vector<std::vector<double>> &Mr, double sh_rate,
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

inline double rastrigin(const std::vector<double> &z)
{
  double&& result =
      std::accumulate(z.begin(), z.end(), 0.0, [](double acc, double value)
      {
        return acc + (value * value - 10.0 * std::cos(2.0 * PI * value) + 10.0);
      });
  return result;
}

inline double rosenbrock_func(const std::vector<double> &z)
{
  std::vector<double> z_copy(z);
  double&& result = 0.0;
  z_copy[0] += 1.0;
  for (size_t i = 0; i < z_copy.size() - 1; ++i)
  {
    z_copy[i + 1] += 1.0;
    auto tmp1 = z_copy[i] * z_copy[i] - z_copy[i + 1];
    auto tmp2 = z_copy[i] - 1.0;
    result += 100.0 * tmp1 * tmp1 + tmp2 * tmp2;
  }
  return result;
}

inline double levy_func(const std::vector<double> &z)
{
  std::vector<double> w(z.size());

  for (size_t i = 0; i < z.size(); ++i)
  {
    w[i] = 1.0 + (z[i] - 0.0) / 4.0;
  }

  double term1 = std::pow(std::sin(PI * w[0]), 2);
  double term3 = std::pow((w.back() - 1), 2) *
                 (1 + std::pow(std::sin(2 * PI * w.back()), 2));
  double sum = 0.0;

  for (size_t i = 0; i < z.size() - 1; ++i)
  {
    auto wi = w[i];
    auto newv =
        std::pow((wi - 1), 2) * (1 + 10 * std::pow(std::sin(PI * wi + 1), 2));
    sum += newv;
  }

  double&& result = term1 + sum + term3;
  return result;
}

inline double zakharov_func(const std::vector<double> &x)
{
  double sum1 = std::inner_product(x.begin(), x.end(), x.begin(), 0.0);
  size_t i = 0; // Declare i outside the lambda.
  double sum2 = std::accumulate(x.begin(), x.end(), 0.0,
                                [&i, &x](double accum, const double &xi)
                                {
                                  auto result = accum + 0.5 * (++i) * xi;
                                  if (i == x.size())
                                    i = 0;
                                  return result;
                                });
  double&& result = sum1 + std::pow(sum2, 2) + std::pow(sum2, 4);
  return result;
}

inline double schaffer_F7_func(const std::vector<double> &y)
{
  std::vector<double> z(y.size());
  double tmp;

  double result = 0;
  for (size_t i = 0; i < z.size() - 1; ++i)
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

  result = result * result / (y.size() - 1) / (y.size() - 1);
  return result;
}







inline double hgbat(const std::vector<double> &z)
{
  std::vector<double> z_copy(z);

  std::transform(z_copy.begin(), z_copy.end(), z_copy.begin(),
                 [](double value) { return value - 1.0; });

  const double alpha = 1.0 / 4.0;
  const double r2 = std::inner_product(z_copy.begin(), z_copy.end(), z_copy.begin(), 0.0);
  const double sum_z = std::accumulate(z_copy.begin(), z_copy.end(), 0.0);
  const double term1 = std::pow(r2, 2.0);
  const double term2 = std::pow(sum_z, 2.0);
  const double term3 = std::pow(std::abs(term1 - term2), 2 * alpha);
  const double term4 = (0.5 * r2 + sum_z) / static_cast<double>(z_copy.size());
  const double result = term3 + term4 + 0.5;

  return result;
}


inline void bent_cigar_func(const std::vector<double> &x, double &result,
                            const std::vector<double> &Os,
                            const std::vector<std::vector<double>> &Mr, bool s_flag,
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





inline double katsuura_func(const std::vector<double> &z)
{
  double tmp3 = std::pow(static_cast<double>(z.size()), 1.2);

  double&& result = std::accumulate(
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

  double tmp1 = 10.0 / z.size() / z.size();
  result = result * tmp1 - tmp1;
  return result;
}

inline double ackley_func(const std::vector<double> &z)
{
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

  double result = E - 20.0 * std::exp(sum1) - std::exp(sum2) + 20.0;
  return result;
}

inline double schwefel_func(const std::vector<double> &z)
{
  std::vector<double> z_copy(z);

  double result = 0.0;
  for (size_t i = 0; i < z_copy.size(); ++i)
  {
    z_copy[i] += 4.209687462275036e+002;
    double tmp;

    if (z_copy[i] > 500)
    {
      result -= (500.0 - std::fmod(z_copy[i], 500)) *
                std::sin(std::pow(500.0 - std::fmod(z_copy[i], 500), 0.5));
      tmp = (z_copy[i] - 500.0) / 100;
      result += tmp * tmp / z_copy.size();
    } else if (z_copy[i] < -500)
    {
      result -=
          (-500.0 + std::fmod(std::fabs(z_copy[i]), 500)) *
          std::sin(std::pow(500.0 - std::fmod(std::fabs(z_copy[i]), 500), 0.5));
      tmp = (z_copy[i] + 500.0) / 100;
      result += tmp * tmp / z_copy.size();
    } else
    {
      result -= z_copy[i] * std::sin(std::pow(std::fabs(z_copy[i]), 0.5));
    }
  }
  result += 4.189828872724338e+002 * z_copy.size();
  return result;
}









inline void hf02(const std::vector<double> &x, double &f,
                 const std::vector<double> &Os, const std::vector<std::vector<double>> &Mr,
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

  std::vector<double> hgbat_x(y.begin() + G[1], y.begin() + G[1] + G_nx[1]);
  std::vector<double> hgbat_z(hgbat_x.size());
  std::vector<double> hgbat_y(hgbat_x.size());
  scale_and_rotate(hgbat_x, hgbat_z, hgbat_y, Os, Mr, 5.0 / 100.0, 0, 0);
  fit[1] = hgbat(hgbat_z);

  fit[2] = rastrigin(std::vector<double>(y.begin() + G[2], y.begin() + G[2] + G_nx[2]));

  f = 0.0;
  for (const auto &val : fit)
  {
    f += val;
  }
}


inline double hf10(const std::vector<double> &x,
                 const std::vector<double> &Os, const std::vector<std::vector<double>> &Mr,
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

  std::vector<double> hgbat_x(y.begin() + G[0], y.begin() + G[0] + G_nx[0]);
  std::vector<double> hgbat_z(hgbat_x.size());
  std::vector<double> hgbat_y(hgbat_x.size());
  scale_and_rotate(hgbat_x, hgbat_z, hgbat_y, Os, Mr, 5.0 / 100.0, 0, 0);
  fit[0] = hgbat(hgbat_z);

  std::vector<double> katsuura_func_x(y.begin() + G[1], y.begin() + G[1] + G_nx[1]);
  std::vector<double> katsuura_func_z(katsuura_func_x.size());
  std::vector<double> katsuura_func_y(katsuura_func_x.size());
  scale_and_rotate(katsuura_func_x, katsuura_func_z, katsuura_func_y, Os, Mr, 5.0 / 100.0, 0, 0);
  fit[1] = katsuura_func(katsuura_func_z);

  std::vector<double> ackley_func_x(y.begin() + G[2], y.begin() + G[2] + G_nx[2]);
  std::vector<double> ackley_func_z(ackley_func_x.size());
  std::vector<double> ackley_func_y(ackley_func_x.size());
  scale_and_rotate(ackley_func_x, ackley_func_z, ackley_func_y, Os, Mr, 1.0, 0, 0);
  fit[2] = ackley_func(ackley_func_z);

  std::vector<double> rastrigin_x(y.begin() + G[3], y.begin() + G[3] + G_nx[3]);
  std::vector<double> rastrigin_z(rastrigin_x.size());
  std::vector<double> rastrigin_y(rastrigin_x.size());
  scale_and_rotate(rastrigin_x, rastrigin_z, rastrigin_y, Os, Mr, 5.12 / 100.0, 0, 0);
  fit[3] = rastrigin(rastrigin_z);

  std::vector<double> schwefel_func_x(y.begin() + G[4], y.begin() + G[4] + G_nx[4]);
  std::vector<double> schwefel_func_z(schwefel_func_x.size());
  std::vector<double> schwefel_func_y(schwefel_func_x.size());
  scale_and_rotate(schwefel_func_x, schwefel_func_z, schwefel_func_y, Os, Mr, 1000.0 / 100.0, 0, 0);
  fit[4] = schwefel_func(schwefel_func_z);

  std::vector<double> schaffer_y(y.begin(), y.begin() + G_nx[5]);
  fit[5] = schaffer_F7_func(schaffer_y);

  double&& f = std::accumulate(fit.begin(), fit.end(), 0.0);
  return f;
}





} // namespace ioh::problem::cec
