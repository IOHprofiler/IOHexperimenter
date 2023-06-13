#pragma once

#include "cec_basic_funcs.hpp"
#include "cec_utils.hpp"


namespace ioh::problem::cec
{
    class CecComplexFuncs
    {
    public:
        CecUtils cec_utils_;
        CecBasicFuncs cec_basic_funcs_;

        // void cec2022_cf01(double *x, double *f, int nx, double *Os, double *Mr,
        //                   int r_flag)
        // {
        //     int i, cf_num = 5;
        //     double fit[5];
        //     double delta[5] = {10, 20, 30, 40, 50};
        //     double bias[5] = {0, 200, 300, 100, 400};

        //     i = 0;
        //     rosenbrock_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1,
        //                     r_flag);
        //     fit[i] = 10000 * fit[i] / 1e+4;
        //     i = 1;
        //     ellips_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1,
        //                 r_flag);
        //     fit[i] = 10000 * fit[i] / 1e+10;
        //     i = 2;
        //     bent_cigar_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1,
        //                     r_flag);
        //     fit[i] = 10000 * fit[i] / 1e+30;
        //     i = 3;
        //     discus_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1,
        //                 r_flag);
        //     fit[i] = 10000 * fit[i] / 1e+10;
        //     i = 4;
        //     ellips_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1, 0);
        //     fit[i] = 10000 * fit[i] / 1e+10;
        //     cf_cal(x, f, nx, Os, delta, bias, fit, cf_num);
        // }
        // void cec2022_cf02(double *x, double *f, int nx, double *Os, double
        // *Mr,
        //                   int r_flag)
        // {
        //     int i, cf_num = 3;
        //     double fit[3];
        //     double delta[3] = {20, 10, 10};
        //     double bias[3] = {0, 200, 100};

        //     i = 0;
        //     schwefel_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1,
        //     0); i = 1; rastrigin_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx
        //     * nx], 1,
        //                    r_flag);
        //     i = 2;
        //     hgbat_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1,
        //                r_flag);
        //     cf_cal(x, f, nx, Os, delta, bias, fit, cf_num);
        // }
        // void cec2022_cf03(double *x, double *f, int nx, double *Os, double
        // *Mr,
        //                   int r_flag)
        // {
        //     int i, cf_num = 5;
        //     double fit[5];
        //     double delta[5] = {20, 20, 30, 30, 20};
        //     double bias[5] = {0, 200, 300, 400, 200};
        //     i = 0;
        //     escaffer6_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1,
        //                    r_flag);
        //     fit[i] = 10000 * fit[i] / 2e+7;
        //     i = 1;
        //     schwefel_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1,
        //                   r_flag);
        //     i = 2;
        //     griewank_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1,
        //                   r_flag);
        //     fit[i] = 1000 * fit[i] / 100;
        //     i = 3;
        //     rosenbrock_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1,
        //                     r_flag);
        //     i = 4;
        //     rastrigin_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1,
        //                    r_flag);
        //     fit[i] = 10000 * fit[i] / 1e+3;
        //     cf_cal(x, f, nx, Os, delta, bias, fit, cf_num);
        // }

        // void cec2022_cf04(double *x, double *f, int nx, double *Os, double
        // *Mr,
        //                   int r_flag)
        // {
        //     int i, cf_num = 6;
        //     double fit[6];
        //     double delta[6] = {10, 20, 30, 40, 50, 60};
        //     double bias[6] = {0, 300, 500, 100, 400, 200};
        //     i = 0;
        //     hgbat_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1,
        //                r_flag);
        //     fit[i] = 10000 * fit[i] / 1000;
        //     i = 1;
        //     rastrigin_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1,
        //                    r_flag);
        //     fit[i] = 10000 * fit[i] / 1e+3;
        //     i = 2;
        //     schwefel_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1,
        //                   r_flag);
        //     fit[i] = 10000 * fit[i] / 4e+3;
        //     i = 3;
        //     bent_cigar_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1,
        //                     r_flag);
        //     fit[i] = 10000 * fit[i] / 1e+30;
        //     i = 4;
        //     ellips_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1,
        //                 r_flag);
        //     fit[i] = 10000 * fit[i] / 1e+10;
        //     i = 5;
        //     escaffer6_func(x, &fit[i], nx, &Os[i * nx], &Mr[i * nx * nx], 1,
        //                    r_flag);
        //     fit[i] = 10000 * fit[i] / 2e+7;
        //     cf_cal(x, f, nx, Os, delta, bias, fit, cf_num);
        // }
    };
} // namespace ioh::problem::cec
