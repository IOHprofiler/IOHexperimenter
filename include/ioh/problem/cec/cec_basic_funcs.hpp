#pragma once

#include "cec_utils.hpp"


namespace ioh::problem::cec
{
    class CecBasicFuncs
    {
    public:
        CecUtils cec_utils_;

        double bent_cigar_func(std::vector<double> &x,
                               const std::vector<double> &Os,
                               const std::vector<double> &Mr, bool s_flag,
                               bool r_flag, int nx)
        {
            cec_utils_.sr_func(x, Os, Mr, 1.0, s_flag, r_flag, nx);
            double f = x.at(0) * x.at(0);
            for (int i = 1; i < nx; i++)
            {
                f += pow(10.0, 6.0) * x.at(i) * x.at(i);
            }
            return f;
        }

        // void sphere_func(double *x, double *f, int nx, double *Os, double
        // *Mr,
        //                  int s_flag, int r_flag)
        // {
        //     int i;
        //     f[0] = 0.0;
        //     double *y = calloc(nx, sizeof(double));
        //     double *z = calloc(nx, sizeof(double));
        //     sr_func(x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y);
        //     for (i = 0; i < nx; i++)
        //     {
        //         f[0] += z[i] * z[i];
        //     }
        //     free(y);
        //     free(z);
        // }

        // void ellips_func(double *x, double *f, int nx, double *Os, double
        // *Mr,
        //                  int s_flag, int r_flag)
        // {
        //     int i;
        //     f[0] = 0.0;
        //     double *y = calloc(nx, sizeof(double));
        //     double *z = calloc(nx, sizeof(double));
        //     sr_func(x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y);
        //     for (i = 0; i < nx; i++)
        //     {
        //         f[0] += pow(10.0, 6.0 * i / (nx - 1)) * z[i] * z[i];
        //     }
        //     free(y);
        //     free(z);
        // }

        // void sum_diff_pow_func(double *x, double *f, int nx, double *Os,
        // double *Mr,
        //                        int s_flag, int r_flag)
        // {
        //     int i;
        //     double *y = calloc(nx, sizeof(double));
        //     double *z = calloc(nx, sizeof(double));
        //     sr_func(x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y);
        //     f[0] = 0.0;
        //     double sum = 0.0;
        //     for (i = 0; i < nx; i++)
        //     {
        //         double xi = z[i];
        //         double newv = pow((fabs(xi)), (i + 1));
        //         sum = sum + newv;
        //     }
        //     f[0] = sum;
        //     free(y);
        //     free(z);
        // }

        double levy_func(std::vector<double> &x, const std::vector<double> &Os,
                         const std::vector<double> &Mr, bool s_flag,
                         bool r_flag, int nx)
        {
            double f = 0.0;
            cec_utils_.sr_func(x, Os, Mr, 1.0, s_flag, r_flag, nx);
            double *w;
            w = (double *)malloc(sizeof(double) * nx);
            for (int i = 0; i < nx; i++)
            {
                w[i] = 1.0 + (x.at(i) - 1.0) / 4.0;
            }
            double term1 = pow((sin(M_PI * w[0])), 2);
            double term3 = pow((w[nx - 1] - 1), 2) *
                (1 + pow((sin(2 * M_PI * w[nx - 1])), 2));

            for (int i = 0; i < nx - 1; i++)
            {
                double wi = w[i];
                double newv =
                    pow((wi - 1), 2) * (1 + 10 * pow((sin(M_PI * wi + 1)), 2));
                f = f + newv;
            }
            f = term1 + f + term3;
            free(w);
            return f;
        }

        // void discus_func(double *x, double *f, int nx, double *Os, double
        // *Mr,
        //                  int s_flag, int r_flag)
        // {
        //     int i;
        //     double *y = calloc(nx, sizeof(double));
        //     double *z = calloc(nx, sizeof(double));
        //     sr_func(x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y);
        //     f[0] = pow(10.0, 6.0) * z[0] * z[0];
        //     for (i = 1; i < nx; i++)
        //     {
        //         f[0] += z[i] * z[i];
        //     }
        //     free(y);
        //     free(z);
        // }

        // void dif_powers_func(double *x, double *f, int nx, double *Os, double
        // *Mr,
        //                      int s_flag, int r_flag)
        // {
        //     int i;
        //     f[0] = 0.0;
        //     double *y = calloc(nx, sizeof(double));
        //     double *z = calloc(nx, sizeof(double));
        //     sr_func(x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y);
        //     for (i = 0; i < nx; i++)
        //     {
        //         f[0] += pow(fabs(z[i]), 2 + 4 * i / (nx - 1));
        //     }
        //     f[0] = pow(f[0], 0.5);
        //     free(y);
        //     free(z);
        // }

        double rosenbrock_func(std::vector<double> &x,
                               const std::vector<double> &Os,
                               const std::vector<double> &Mr, bool s_flag,
                               bool r_flag, int nx)
        {
            double f = 0.0;
            cec_utils_.sr_func(x, Os, Mr, 2.048 / 100.0, s_flag, r_flag, nx);
            for (int i = 0; i < nx; i++)
            {
                x.at(i) += 1.0;
            }
            for (int i = 0; i < nx - 1; i++)
            {
                double item = 0.0;
                item = item + 100.0 * pow((pow(x.at(i), 2) - x.at(i + 1)), 2);
                item = item + pow((x.at(i) - 1.0), 2);
                f = f + item;
            }
            return f;
        }

        double schaffer_F7_func(std::vector<double> &x,
                                const std::vector<double> &Os,
                                const std::vector<double> &Mr, bool s_flag,
                                bool r_flag, int nx)
        {
            double f = 0.0;
            cec_utils_.sr_func(x, Os, Mr, 1.0, s_flag, r_flag, nx);
            for (size_t i = 0; i < x.size() - 1; i++)
            {
                double xi = x.at(i);
                double xi1 = x.at(i + 1);
                double si = pow(pow(xi, 2) + pow(xi1, 2), 0.5);
                double tmp = sin(50.0 * pow(si, 0.2));
                f += pow(si, 0.5) + pow(si, 0.5) * tmp * tmp;
            }
            f = f * f / (nx - 1) / (nx - 1);
            return f;
        }

        double ackley_func(std::vector<double> &x,
                           const std::vector<double> &Os,
                           const std::vector<double> &Mr, bool s_flag,
                           bool r_flag, int nx)
        {
            double f = 0.0;
            double sum1 = 0.0;
            double sum2 = 0.0;
            cec_utils_.sr_func(x, Os, Mr, 1.0, s_flag, r_flag, nx);
            for (int i = 0; i < nx; i++)
            {
                sum1 += x.at(i) * x.at(i);
                sum2 += cos(2.0 * M_PI * x.at(i));
            }
            sum1 = -0.2 * sqrt(sum1 / nx);
            sum2 /= nx;
            f = E - 20.0 * exp(sum1) - exp(sum2) + 20.0;
            return f;
        }

        // void weierstrass_func(double *x, double *f, int nx, double *Os,
        // double *Mr,
        //                       int s_flag, int r_flag)
        // {
        //     int i, j, k_max;
        //     double sum = 0.0;
        //     double sum2 = 0.0;
        //     double a = 0.5;
        //     double b = 3.0;
        //     k_max = 20;
        //     f[0] = 0.0;
        //     double *y = calloc(nx, sizeof(double));
        //     double *z = calloc(nx, sizeof(double));
        //     sr_func(x, z, nx, Os, Mr, 0.5 / 100.0, s_flag, r_flag, y);
        //     for (i = 0; i < nx; i++)
        //     {
        //         sum = 0.0;
        //         sum2 = 0.0;
        //         for (j = 0; j <= k_max; j++)
        //         {
        //             sum += pow(a, j) * cos(2.0 * M_PI * pow(b, j) * (z[i] +
        //             0.5)); sum2 += pow(a, j) * cos(2.0 * M_PI * pow(b, j) *
        //             0.5);
        //         }
        //         f[0] += sum;
        //     }
        //     f[0] -= nx * sum2;
        //     free(y);
        //     free(z);
        // }

        // void griewank_func(double *x, double *f, int nx, double *Os, double
        // *Mr,
        //                    int s_flag, int r_flag)
        // {
        //     int i;
        //     double s, p;
        //     s = 0.0;
        //     p = 1.0;
        //     double *y = calloc(nx, sizeof(double));
        //     double *z = calloc(nx, sizeof(double));
        //     sr_func(x, z, nx, Os, Mr, 600.0 / 100.0, s_flag, r_flag, y);
        //     for (i = 0; i < nx; i++)
        //     {
        //         s += z[i] * z[i];
        //         p *= cos(z[i] / sqrt(1.0 + i));
        //     }
        //     f[0] = 1.0 + s / 4000.0 - p;
        //     free(y);
        //     free(z);
        // }

        double rastrigin_func(std::vector<double> &x,
                              const std::vector<double> &Os,
                              const std::vector<double> &Mr, bool s_flag,
                              bool r_flag, int nx)
        {
            double f = 0.0;
            cec_utils_.sr_func(x, Os, Mr, 5.12 / 100.0, s_flag, r_flag, nx);
            for (int i = 0; i < nx; i++)
            {
                f += pow(x.at(i), 2) - 10.0 * cos(2.0 * M_PI * x.at(i)) + 10.0;
            }
            return f;
        }

        double schwefel_func(std::vector<double> &x,
                             const std::vector<double> &Os,
                             const std::vector<double> &Mr, bool s_flag,
                             bool r_flag, int nx)
        {
            double f = 0.0;
            double tmp = 0.0;
            cec_utils_.sr_func(x, Os, Mr, 1000.0 / 100.0, s_flag, r_flag, nx);
            for (int i = 0; i < nx; i++)
            {
                x.at(i) += 4.209687462275036e+002;
                if (x.at(i) > 500)
                {
                    f -= (500.0 - fmod(x.at(i), 500)) *
                        sin(pow(500.0 - fmod(x.at(i), 500), 0.5));
                    tmp = (x.at(i) - 500.0) / 100;
                    f += tmp * tmp / nx;
                }
                else if (x.at(i) < -500)
                {
                    f -= (-500.0 + fmod(fabs(x.at(i)), 500)) *
                        sin(pow(500.0 - fmod(fabs(x.at(i)), 500), 0.5));
                    tmp = (x.at(i) + 500.0) / 100;
                    f += tmp * tmp / nx;
                }
                else
                {
                    f -= x.at(i) * sin(pow(fabs(x.at(i)), 0.5));
                }
            }
            f += 4.189828872724338e+002 * nx;
            return f;
        }

        double katsuura_func(std::vector<double> &x,
                             const std::vector<double> &Os,
                             const std::vector<double> &Mr, bool s_flag,
                             bool r_flag, int nx)
        {
            double f = 0.0;
            double tmp3 = pow(1.0 * nx, 1.2);
            cec_utils_.sr_func(x, Os, Mr, 5.0 / 100.0, s_flag, r_flag, nx);
            for (int i = 0; i < nx; i++)
            {
                double temp = 0.0;
                for (int j = 1; j <= 32; j++)
                {
                    double tmp1 = pow(2.0, j);
                    double tmp2 = tmp1 * x.at(i);
                    temp += fabs(tmp2 - floor(tmp2 + 0.5)) / tmp1;
                }
                f *= pow(1.0 + (i + 1) * temp, 10.0 / tmp3);
            }
            double tmp1 = 10.0 / nx / nx;
            f = f * tmp1 - tmp1;
            return f;
        }

        double grie_rosen_func(std::vector<double> &x,
                               const std::vector<double> &Os,
                               const std::vector<double> &Mr, bool s_flag,
                               bool r_flag, int nx)
        {
            double f = 0.0;
            cec_utils_.sr_func(x, Os, Mr, 5.0 / 100.0, s_flag, r_flag, nx);
            x.at(0) += 1.0;
            for (int i = 0; i < nx - 1; i++)
            {
                x.at(i + 1) += 1.0;
                double tmp1 = x.at(i) * x.at(i) - x.at(i + 1);
                double tmp2 = x.at(i) - 1.0;
                double temp = 100.0 * tmp1 * tmp1 + tmp2 * tmp2;
                f += (temp * temp) / 4000.0 - cos(temp) + 1.0;
            }
            double tmp1 = x.at(nx - 1) * x.at(nx - 1) - x.at(0);
            double tmp2 = x.at(nx - 1) - 1.0;
            double temp = 100.0 * tmp1 * tmp1 + tmp2 * tmp2;
            f += (temp * temp) / 4000.0 - cos(temp) + 1.0;
            return f;
        }

        // void escaffer6_func(double *x, double *f, int nx, double *Os, double
        // *Mr,
        //                     int s_flag, int r_flag)
        // {
        //     int i;
        //     double temp1, temp2;
        //     double *y = calloc(nx, sizeof(double));
        //     double *z = calloc(nx, sizeof(double));
        //     sr_func(x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y); /* shift and
        //     rotate
        //                                                         */

        //     f[0] = 0.0;
        //     for (i = 0; i < nx - 1; i++)
        //     {
        //         temp1 = sin(sqrt(z[i] * z[i] + z[i + 1] * z[i + 1]));
        //         temp1 = temp1 * temp1;
        //         temp2 = 1.0 + 0.001 * (z[i] * z[i] + z[i + 1] * z[i + 1]);
        //         f[0] += 0.5 + (temp1 - 0.5) / (temp2 * temp2);
        //     }
        //     temp1 = sin(sqrt(z[nx - 1] * z[nx - 1] + z[0] * z[0]));
        //     temp1 = temp1 * temp1;
        //     temp2 = 1.0 + 0.001 * (z[nx - 1] * z[nx - 1] + z[0] * z[0]);
        //     f[0] += 0.5 + (temp1 - 0.5) / (temp2 * temp2);
        //     free(z);
        //     free(y);
        // }

        double happycat_func(std::vector<double> &x,
                             const std::vector<double> &Os,
                             const std::vector<double> &Mr, bool s_flag,
                             bool r_flag, int nx)
        {
            double f = 0.0;
            cec_utils_.sr_func(x, Os, Mr, 5.0 / 100.0, s_flag, r_flag, nx);
            double alpha, r2, sum_z;
            alpha = 1.0 / 8.0;
            r2 = 0.0;
            sum_z = 0.0;
            for (int i = 0; i < nx; i++)
            {
                x.at(i) = x.at(i) - 1.0;
                r2 += x.at(i) * x.at(i);
                sum_z += x.at(i);
            }
            f = pow(fabs(r2 - nx), 2 * alpha) + (0.5 * r2 + sum_z) / nx + 0.5;
            return f;
        }

        double hgbat_func(std::vector<double> &x, const std::vector<double> &Os,
                          const std::vector<double> &Mr, bool s_flag,
                          bool r_flag, int nx)
        {
            double f = 0.0;
            cec_utils_.sr_func(x, Os, Mr, 5.0 / 100.0, s_flag, r_flag, nx);
            double alpha, r2, sum_z;
            alpha = 1.0 / 4.0;
            r2 = 0.0;
            sum_z = 0.0;
            for (int i = 0; i < nx; i++)
            {
                x.at(i) = x.at(i) - 1.0;
                r2 += x.at(i) * x.at(i);
                sum_z += x.at(i);
            }
            f = pow(fabs(pow(r2, 2.0) - pow(sum_z, 2.0)), 2 * alpha) +
                (0.5 * r2 + sum_z) / nx + 0.5;
            return f;
        }

        double zakharov_func(std::vector<double> &x,
                             const std::vector<double> &Os,
                             const std::vector<double> &Mr, bool s_flag,
                             bool r_flag, int nx)
        {
            double f = 0.0;
            cec_utils_.sr_func(x, Os, Mr, 1.0, s_flag, r_flag, nx);
            std::vector<double> terms = {0, 0};
            for (int i = 0; i < nx; i++)
            {
                terms.at(0) += pow(x.at(i), 2);
                terms.at(1) += 0.5 * (i + 1) * x.at(i);
            }
            f = terms.at(0) + pow(terms.at(1), 2) + pow(terms.at(1), 4);
            return f;
        }

        // void dixon_price_func(double *x, double *f, int nx, double *Os,
        // double *Mr,
        //                       int s_flag, int r_flag)
        // {
        //     int i;
        //     double *y = calloc(nx, sizeof(double));
        //     double *z = calloc(nx, sizeof(double));
        //     sr_func(x, z, nx, Os, Mr, 1.0, s_flag, r_flag, y);
        //     f[0] = 0;
        //     double x1 = z[0];
        //     ;
        //     double term1 = pow((x1 - 1), 2);

        //     double sum = 0;
        //     for (i = 1; i < nx; i++)
        //     {
        //         double xi = z[i];
        //         double xold = z[i - 1];
        //         double newv = i * pow((pow(2 * xi, 2) - xold), 2);
        //         sum = sum + newv;
        //     }
        //     f[0] = term1 + sum;
        //     free(y);
        //     free(z);
        // }

        // void Lennard_Jones(double *x, int D, double *f)
        // {
        //     f[0] = 0;
        //     int i, j, k, a, b;
        //     double xd, yd, zd, ed, ud, sum = 0;
        //     k = D / 3;
        //     if (k < 2)
        //     {
        //         k = 2;
        //         D = 6;
        //     }
        //     for (i = 0; i < k - 1; i++)
        //     {
        //         for (j = i + 1; j < k; j++)
        //         {
        //             a = 3 * i;
        //             b = 3 * j;
        //             xd = x[a] - x[b];
        //             yd = x[a + 1] - x[b + 1];
        //             zd = x[a + 2] - x[b + 2];
        //             ed = xd * xd + yd * yd + zd * zd;
        //             ud = ed * ed * ed;
        //             if (ud > 1.0e-10)
        //                 sum += (1.0 / ud - 2.0) / ud;
        //             else
        //                 sum += 1.0e20;
        //         }
        //     }
        //     f[0] += sum;
        //     f[0] += 12.7120622568;
        // }

        // void Hilbert(double *x, int D, double *f)
        // {
        //     f[0] = 0;
        //     int i, j, k, b;
        //     double sum = 0;
        //     static double hilbert[10][10], y[10][10];
        //     b = (int)sqrt((double)D);
        //     for (i = 0; i < b; i++)
        //     {
        //         for (j = 0; j < b; j++)
        //         {
        //             hilbert[i][j] = 1. / (double)(i + j + 1);
        //         }
        //     }
        //     for (j = 0; j < b; j++)
        //     {
        //         for (k = 0; k < b; k++)
        //         {
        //             y[j][k] = 0;
        //             for (i = 0; i < b; i++)
        //             {
        //                 y[j][k] += hilbert[j][i] * x[k + b * i];
        //             }
        //         }
        //     }
        //     for (i = 0; i < b; i++)
        //     {
        //         for (j = 0; j < b; j++)
        //         {
        //             if (i == j)
        //             {
        //                 sum += fabs(y[i][j] - 1);
        //             }
        //             else
        //             {
        //                 sum += fabs(y[i][j]);
        //             }
        //         }
        //     }

        //     f[0] += sum;
        // }

        // void Chebyshev(double *x, int D, double *f)
        // {
        //     f[0] = 0.0;
        //     int i, j;
        //     static int sample;
        //     double a = 1., b = 1.2, px, y = -1, sum = 0;
        //     static double dx, dy;

        //     for (j = 0; j < D - 2; j++)
        //     {
        //         dx = 2.4 * b - a;
        //         a = b;
        //         b = dx;
        //     }
        //     sample = 32 * D;
        //     dy = 2. / (long double)sample;

        //     for (i = 0; i <= sample; i++)
        //     {
        //         px = x[0];
        //         for (j = 1; j < D; j++)
        //         {
        //             px = y * px + x[j];
        //         }
        //         if (px < -1 || px > 1)
        //             sum += (1. - fabs(px)) * (1. - fabs(px));
        //         y += dy;
        //     }
        //     for (i = -1; i <= 1; i += 2)
        //     {
        //         px = x[0];
        //         for (j = 1; j < D; j++)
        //         {
        //             px = 1.2 * px + x[j];
        //         }
        //         if (px < dx)
        //             sum += px * px;
        //     }
        //     f[0] += sum;
        // }

        double step_rastrigin_func(std::vector<double> &x,
                                   const std::vector<double> &Os,
                                   const std::vector<double> &Mr, bool s_flag,
                                   bool r_flag, int nx)
        {
            double f = 0.0;
            cec_utils_.sr_func(x, Os, Mr, 5.12 / 100.0, s_flag, r_flag, nx);
            for (int i = 0; i < nx; i++)
            {
                f += pow(x.at(i), 2) - 10.0 * cos(2.0 * M_PI * x.at(i)) + 10.0;
            }
            return f;
        }

        // void bi_rastrigin_func(double *x, double *f, int nx, double *Os,
        // double *Mr,
        //                        int s_flag, int r_flag)
        // {
        //     int i;
        //     double mu0 = 2.5, d = 1.0, s, mu1, tmp, tmp1, tmp2;
        //     double *tmpx;
        //     tmpx = (double *)calloc(nx, sizeof(double));
        //     s = 1.0 - 1.0 / (2.0 * pow(nx + 20.0, 0.5) - 8.2);
        //     mu1 = -pow((mu0 * mu0 - d) / s, 0.5);
        //     double *y = calloc(nx, sizeof(double));
        //     double *z = calloc(nx, sizeof(double));

        //     if (s_flag == 1)
        //         shiftfunc(x, y, nx, Os);
        //     else
        //     {
        //         for (i = 0; i < nx; i++) // shrink to the orginal search
        //         range
        //         {
        //             y[i] = x[i];
        //         }
        //     }
        //     for (i = 0; i < nx; i++) // shrink to the orginal search range
        //     {
        //         y[i] *= 10.0 / 100.0;
        //     }

        //     for (i = 0; i < nx; i++)
        //     {
        //         tmpx[i] = 2 * y[i];
        //         if (Os[i] < 0.0)
        //             tmpx[i] *= -1.;
        //     }
        //     for (i = 0; i < nx; i++)
        //     {
        //         z[i] = tmpx[i];
        //         tmpx[i] += mu0;
        //     }
        //     tmp1 = 0.0;
        //     tmp2 = 0.0;
        //     for (i = 0; i < nx; i++)
        //     {
        //         tmp = tmpx[i] - mu0;
        //         tmp1 += tmp * tmp;
        //         tmp = tmpx[i] - mu1;
        //         tmp2 += tmp * tmp;
        //     }
        //     tmp2 *= s;
        //     tmp2 += d * nx;
        //     tmp = 0.0;

        //     if (r_flag == 1)
        //     {
        //         rotatefunc(z, y, nx, Mr);
        //         for (i = 0; i < nx; i++)
        //         {
        //             tmp += cos(2.0 * M_PI * y[i]);
        //         }
        //         if (tmp1 < tmp2)
        //             f[0] = tmp1;
        //         else
        //             f[0] = tmp2;
        //         f[0] += 10.0 * (nx - tmp);
        //     }
        //     else
        //     {
        //         for (i = 0; i < nx; i++)
        //         {
        //             tmp += cos(2.0 * M_PI * z[i]);
        //         }
        //         if (tmp1 < tmp2)
        //             f[0] = tmp1;
        //         else
        //             f[0] = tmp2;
        //         f[0] += 10.0 * (nx - tmp);
        //     }
        //     free(y);
        //     free(z);
        //     free(tmpx);
        // }
    };
} // namespace ioh::problem::cec
