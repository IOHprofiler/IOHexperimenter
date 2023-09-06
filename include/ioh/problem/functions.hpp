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

// ===============================================================================================================================
#define DEBUG
#ifdef DEBUG
#define LOG_FILE_NAME "cec_test_log.txt"

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

#else
#define LOG(message) // Nothing
#endif
// ===============================================================================================================================

namespace ioh::problem {
constexpr double INF = 1.0e99;
constexpr double EPS = 1.0e-14;
constexpr double E = 2.7182818284590452353602874713526625;
constexpr double PI = 3.1415926535897932384626433832795029;

inline void shiftfunc(const std::vector<double> &x, std::vector<double> &xshift,
                      const std::vector<double> &Os) {
  for (size_t i = 0; i < x.size(); ++i) {
    xshift[i] = x[i] - Os[i];
  }
}

inline void rotatefunc(const std::vector<double> &x, std::vector<double> &xrot,
                       const std::vector<std::vector<double>> &Mr) {
  for (size_t i = 0; i < x.size(); ++i) {
    xrot[i] = 0;
    for (size_t j = 0; j < x.size(); ++j) {
      xrot[i] += x[j] * Mr[i][j];
    }
  }
}

inline void scale_and_rotate(const std::vector<double> &x,
                             std::vector<double> &sr_x, std::vector<double> &y,
                             const std::vector<double> &Os,
                             const std::vector<std::vector<double>> &Mr,
                             double sh_rate, bool s_flag, bool r_flag) {
  if (s_flag) {
    if (r_flag) {
      shiftfunc(x, y, Os);
      for (size_t i = 0; i < x.size(); ++i) {
        y[i] *= sh_rate;
      }
      rotatefunc(y, sr_x, Mr);
    } else {
      shiftfunc(x, sr_x, Os);
      for (size_t i = 0; i < x.size(); ++i) {
        sr_x[i] *= sh_rate;
      }
    }
  } else {
    if (r_flag) {
      for (size_t i = 0; i < x.size(); ++i) {
        y[i] = x[i] * sh_rate;
      }
      rotatefunc(y, sr_x, Mr);
    } else {
      for (size_t i = 0; i < x.size(); ++i) {
        sr_x[i] = x[i] * sh_rate;
      }
    }
  }
}

inline double rastrigin(const std::vector<double> &z) {
  double result =
      std::accumulate(z.begin(), z.end(), 0.0, [](double acc, double value) {
        return acc + (value * value - 10.0 * std::cos(2.0 * PI * value) + 10.0);
      });
  return result;
}

inline double rosenbrock_func(const std::vector<double> &z) {
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

inline double levy_func(const std::vector<double> &z) {
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

inline double zakharov_func(const std::vector<double> &x) {
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

inline double schaffer_F7_func(const std::vector<double> &y) {
  std::vector<double> z(y.size());
  double tmp;

  double result = 0;
  for (size_t i = 0; i < z.size() - 1; ++i) {
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

inline double bent_cigar_func(const std::vector<double> &z)
{
  static const auto condition = 1.0e6;
  double result = z[0] * z[0];
  for (size_t i = 1; i < z.size(); ++i)
  {
    result += condition * z[i] * z[i];
  }
  return result;
}

inline double katsuura_func(const std::vector<double> &z) {
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

inline double ackley_func(const std::vector<double> &z) {
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

inline double schwefel_func(const std::vector<double> &z) {
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

inline double happycat_func(const std::vector<double> &z)
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

inline double grie_rosen_func(const std::vector<double> &z)
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

inline double ellips_func(const std::vector<double> &z)
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

inline double discus_func(const std::vector<double> &z)
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

inline void escaffer6_func(const std::vector<double> &x, double &f,
                           const std::vector<double> &Os,
                           const std::vector<std::vector<double>> &Mr,
                           int s_flag, int r_flag) {
  std::vector<double> z(x.size());
  std::vector<double> y(x.size());

  scale_and_rotate(x, z, y, Os, Mr, 1.0, s_flag, r_flag);

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

inline void griewank_func(const std::vector<double> &x, double &f,
                          const std::vector<double> &Os,
                          const std::vector<std::vector<double>> &Mr,
                          int s_flag, int r_flag) {
  std::vector<double> z(x.size());
  std::vector<double> y(x.size());

  scale_and_rotate(x, z, y, Os, Mr, 600.0 / 100.0, s_flag, r_flag);

  double s =
      std::accumulate(z.begin(), z.end(), 0.0,
                      [](double sum, double val) { return sum + val * val; });

  double p = 1.0;
  for (size_t i = 0; i < x.size(); i++) {
    p *= cos(z[i] / sqrt(1.0 + i));
  }

  f = 1.0 + s / 4000.0 - p;
}

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

  std::vector<double> bent_cigar_func_z(prepared_y.begin() + G[0], prepared_y.begin() + G[0] + G_nx[0]);
  fit[0] = bent_cigar_func(bent_cigar_func_z);

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

  std::vector<double> katsuura_func_z(prepared_y.begin() + G[1], prepared_y.begin() + G[1] + G_nx[1]);
  fit[1] = katsuura_func(katsuura_func_z);

  std::vector<double> ackley_func_z(prepared_y.begin() + G[2], prepared_y.begin() + G[2] + G_nx[2]);
  fit[2] = ackley_func(ackley_func_z);

  std::vector<double> rastrigin_z(prepared_y.begin() + G[3], prepared_y.begin() + G[3] + G_nx[3]);
  fit[3] = rastrigin(rastrigin_z);

  std::vector<double> schwefel_func_z(prepared_y.begin() + G[4], prepared_y.begin() + G[4] + G_nx[4]);
  fit[4] = schwefel_func(schwefel_func_z);

  std::vector<double> schaffer_y(prepared_y.begin() + G[5], prepared_y.begin() + G[5] + G_nx[5]);
  fit[5] = schaffer_F7_func(schaffer_y);

  double &&f = std::accumulate(fit.begin(), fit.end(), 0.0);
  return f;
}

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

  std::vector<double> katsuura_func_z(prepared_y.begin() + G[0], prepared_y.begin() + G[0] + G_nx[0]);
  fit[0] = katsuura_func(katsuura_func_z);

  std::vector<double> happycat_func_z(prepared_y.begin() + G[1], prepared_y.begin() + G[1] + G_nx[1]);
  fit[1] = happycat_func(happycat_func_z);

  std::vector<double> grie_rosen_func_z(prepared_y.begin() + G[2], prepared_y.begin() + G[2] + G_nx[2]);
  fit[2] = grie_rosen_func(grie_rosen_func_z);

  std::vector<double> schwefel_func_z(prepared_y.begin() + G[3], prepared_y.begin() + G[3] + G_nx[3]);
  fit[3] = schwefel_func(schwefel_func_z);

  std::vector<double> ackley_func_z(prepared_y.begin() + G[4], prepared_y.begin() + G[4] + G_nx[4]);
  fit[4] = ackley_func(ackley_func_z);

  double f = std::accumulate(fit.begin(), fit.end(), 0.0);
  return f;
}

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

  std::vector<double> rosenbrock_func_z(nx);
  std::vector<double> rosenbrock_func_y(nx);
  scale_and_rotate(x, rosenbrock_func_z, rosenbrock_func_y, Os[0], Mr[0], 2.048 / 100.0, 1, r_flag);
  fit[0] = rosenbrock_func(rosenbrock_func_z);
  fit[0] = 10000 * fit[0] / 1e+4;

  std::vector<double> ellips_func_z(nx);
  std::vector<double> ellips_func_y(nx);
  scale_and_rotate(x, ellips_func_z, ellips_func_y, Os[1], Mr[1], 1.0, 1, 0);
  fit[1] = ellips_func(ellips_func_z);
  fit[1] = 10000 * fit[1] / 1e+10;

  std::vector<double> bent_cigar_func_z(nx);
  std::vector<double> bent_cigar_func_y(nx);
  scale_and_rotate(x, bent_cigar_func_z, bent_cigar_func_y, Os[2], Mr[2], 1.0, 1, r_flag);
  fit[2] = bent_cigar_func(bent_cigar_func_z);
  fit[2] = 10000 * fit[2] / 1e+30;

  std::vector<double> discus_func_z(nx);
  std::vector<double> discus_func_y(nx);
  scale_and_rotate(x, discus_func_z, discus_func_y, Os[3], Mr[3], 1.0, 1, r_flag);
  fit[3] = discus_func(discus_func_z);
  fit[3] = 10000 * fit[3] / 1e+10;

  std::vector<double> ellips_func_2_z(nx);
  std::vector<double> ellips_func_2_y(nx);
  scale_and_rotate(x, ellips_func_2_z, ellips_func_2_y, Os[4], Mr[4], 1.0, 1, 0);
  fit[4] = ellips_func(ellips_func_2_z);
  fit[4] = 10000 * fit[4] / 1e+10;

  double f = cf_cal(x, Os, deltas, biases, fit);
  return f;
}

inline double cf02(const std::vector<double> &x,
          const std::vector<std::vector<double>> &Os,
          const std::vector<std::vector<std::vector<double>>> &Mr,
          int r_flag)
{
  const int nx = x.size();
  const std::vector<double> delta = {20, 10, 10};
  const std::vector<double> bias = {0, 200, 100};

  std::vector<double> fit(3);

  std::vector<double> schwefel_func_z(nx);
  std::vector<double> schwefel_func_y(nx);
  scale_and_rotate(x, schwefel_func_z, schwefel_func_y, Os[0], Mr[0], 1000.0 / 100.0, 1, 0);
  fit[0] = schwefel_func(schwefel_func_z);

  std::vector<double> rastrigin_z(nx);
  std::vector<double> rastrigin_y(nx);
  scale_and_rotate(x, rastrigin_z, rastrigin_y, Os[1], Mr[1], 5.12 / 100.0, 1, r_flag);
  fit[1] = rastrigin(rastrigin_z);

  std::vector<double> hgbat_z(nx);
  std::vector<double> hgbat_y(nx);
  scale_and_rotate(x, hgbat_z, hgbat_y, Os[2], Mr[2], 5.0 / 100.0, 1, r_flag);
  fit[2] = hgbat(hgbat_z);

  double f = cf_cal(x, Os, delta, bias, fit);
  return f;
}


inline double cf06(const std::vector<double> &x,
          const std::vector<std::vector<double>> &Os,
          const std::vector<std::vector<std::vector<double>>> &Mr,
          int r_flag)
{
  const int nx = x.size();
  const std::vector<double> delta = {20, 20, 30, 30, 20};
  const std::vector<double> bias = {0, 200, 300, 400, 200};

  std::vector<double> fit(5);

  escaffer6_func(x, fit[0], Os[0], Mr[0], 1, r_flag);
  fit[0] *= 10000 / 2e+7;

  std::vector<double> schwefel_func_z(nx);
  std::vector<double> schwefel_func_y(nx);
  scale_and_rotate(x, schwefel_func_z, schwefel_func_y, Os[1], Mr[1], 1000.0 / 100.0, 1, r_flag);
  fit[1] = schwefel_func(schwefel_func_z);

  griewank_func(x, fit[2], Os[2], Mr[2], 1, r_flag);
  fit[2] *= 1000 / 100;

  std::vector<double> rosenbrock_func_z(nx);
  std::vector<double> rosenbrock_func_y(nx);
  scale_and_rotate(x, rosenbrock_func_z, rosenbrock_func_y, Os[3], Mr[3],
                   2.048 / 100.0, 1, r_flag);
  fit[3] = rosenbrock_func(rosenbrock_func_z);

  std::vector<double> rastrigin_z(nx);
  std::vector<double> rastrigin_y(nx);
  scale_and_rotate(x, rastrigin_z, rastrigin_y, Os[4], Mr[4], 5.12 / 100.0, 1, r_flag);
  fit[4] = rastrigin(rastrigin_z);
  fit[4] *= 10000 / 1e+3;

  double f = cf_cal(x, Os, delta, bias, fit);
  return f;
}

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
  scale_and_rotate(x, hgbat_z, hgbat_y, Os[0], Mr[0], 5.0 / 100.0, 1, r_flag);
  fit[0] = hgbat(hgbat_z);
  fit[0] *= 10000 / 1000;

  std::vector<double> rastrigin_z(nx);
  std::vector<double> rastrigin_y(nx);
  scale_and_rotate(x, rastrigin_z, rastrigin_y, Os[1], Mr[1], 5.12 / 100.0, 1, r_flag);
  fit[1] = rastrigin(rastrigin_z);
  fit[1] *= 10000 / 1e+3;

  std::vector<double> schwefel_func_z(nx);
  std::vector<double> schwefel_func_y(nx);
  scale_and_rotate(x, schwefel_func_z, schwefel_func_y, Os[2], Mr[2], 1000.0 / 100.0, 1, r_flag);
  fit[2] = schwefel_func(schwefel_func_z);
  fit[2] *= 10000 / 4e+3;

  // bent_cigar_func(x, fit[3], Os[3], Mr[3], 1, r_flag);
  // fit[3] *= 10000 / 1e+30;

  std::vector<double> ellips_func_z(nx);
  std::vector<double> ellips_func_y(nx);
  scale_and_rotate(x, ellips_func_z, ellips_func_y, Os[4], Mr[4], 1.0, 1, r_flag);
  fit[4] = ellips_func(ellips_func_z);
  fit[4] *= 10000 / 1e+10;

  escaffer6_func(x, fit[5], Os[5], Mr[5], 1, r_flag);
  fit[5] *= 10000 / 2e+7;

  double f = cf_cal(x, Os, delta, bias, fit);
  return f;
}

} // namespace ioh::problem
