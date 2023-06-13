#pragma once

#include "cec_basic_funcs.hpp"
#include "cec_utils.hpp"


namespace ioh::problem::cec
{
    class CecHybridFuncs
    {
    public:
        CecUtils cec_utils_;
        CecBasicFuncs cec_basic_funcs_;

        void split_gx(std::vector<size_t> &Gnx,
                      std::vector<std::vector<double>> &Gx,
                      const std::vector<double> &Gp,
                      const std::vector<double> &x, const std::vector<int> &SS,
                      int cf_num, int nx)
        {
            std::vector<double> y;
            int tmp = 0;
            for (int i = 0; i < cf_num - 1; i++)
            {
                Gnx.push_back(ceil(Gp.at(i) * nx));
                tmp += Gnx.at(i);
            }
            Gnx.push_back(nx - tmp);
            for (int i = 0; i < nx; i++)
            {
                y.push_back(x.at(SS.at(i) - 1));
            }
            int j = -1;
            for (int i = 0; i < nx; i++)
            {
                if (Gx.size() == 0 || Gx.at(j).size() >= Gnx.at(j))
                {
                    std::vector<double> cache;
                    Gx.push_back(cache);
                    j += 1;
                }
                Gx.at(j).push_back(y.at(i));
            }
        }

        double cec2022_hf01(std::vector<double> &x,
                            const std::vector<double> &Os,
                            const std::vector<double> &Mr,
                            const std::vector<int> &SS, bool s_flag,
                            bool r_flag, int nx)
        {
            int cf_num = 3;
            std::vector<size_t> Gnx;
            std::vector<double> Gp = {0.4, 0.4, 0.2};
            std::vector<std::vector<double>> Gx;
            cec_utils_.sr_func(x, Os, Mr, 1.0, s_flag, r_flag, nx);
            split_gx(Gnx, Gx, Gp, x, SS, cf_num, nx);
            double f1 = cec_basic_funcs_.bent_cigar_func(
                Gx.at(0), Os, Mr, false, false, Gnx.at(0));
            double f2 = cec_basic_funcs_.hgbat_func(Gx.at(1), Os, Mr, false,
                                                    false, Gnx.at(1));
            double f3 = cec_basic_funcs_.rastrigin_func(Gx.at(2), Os, Mr, false,
                                                        false, Gnx.at(2));
            return f1 + f2 + f3;
        }

        double cec2022_hf02(std::vector<double> &x,
                            const std::vector<double> &Os,
                            const std::vector<double> &Mr,
                            const std::vector<int> &SS, bool s_flag,
                            bool r_flag, int nx)
        {
            int cf_num = 6;
            std::vector<size_t> Gnx;
            std::vector<double> Gp = {0.1, 0.2, 0.2, 0.2, 0.1, 0.2};
            std::vector<std::vector<double>> Gx;
            cec_utils_.sr_func(x, Os, Mr, 1.0, s_flag, r_flag, nx);
            split_gx(Gnx, Gx, Gp, x, SS, cf_num, nx);
            double f1 = cec_basic_funcs_.hgbat_func(Gx.at(0), Os, Mr, false,
                                                    false, Gnx.at(0));
            double f2 = cec_basic_funcs_.katsuura_func(Gx.at(1), Os, Mr, false,
                                                       false, Gnx.at(1));
            double f3 = cec_basic_funcs_.ackley_func(Gx.at(2), Os, Mr, false,
                                                     false, Gnx.at(2));
            double f4 = cec_basic_funcs_.rastrigin_func(Gx.at(3), Os, Mr, false,
                                                        false, Gnx.at(3));
            double f5 = cec_basic_funcs_.schwefel_func(Gx.at(4), Os, Mr, false,
                                                       false, Gnx.at(4));
            double f6 = cec_basic_funcs_.schaffer_F7_func(
                Gx.at(5), Os, Mr, false, false, Gnx.at(5));
            return f1 + f2 + f3 + f4 + f5 + f6;
        }

        double cec2022_hf03(std::vector<double> &x,
                            const std::vector<double> &Os,
                            const std::vector<double> &Mr,
                            const std::vector<int> &SS, bool s_flag,
                            bool r_flag, int nx)
        {
            int cf_num = 5;
            std::vector<size_t> Gnx;
            std::vector<double> Gp = {0.3, 0.2, 0.2, 0.1, 0.2};
            std::vector<std::vector<double>> Gx;
            cec_utils_.sr_func(x, Os, Mr, 1.0, s_flag, r_flag, nx);
            split_gx(Gnx, Gx, Gp, x, SS, cf_num, nx);
            double f1 = cec_basic_funcs_.katsuura_func(Gx.at(0), Os, Mr, false,
                                                       false, Gnx.at(0));
            double f2 = cec_basic_funcs_.happycat_func(Gx.at(1), Os, Mr, false,
                                                       false, Gnx.at(1));
            double f3 = cec_basic_funcs_.grie_rosen_func(
                Gx.at(2), Os, Mr, false, false, Gnx.at(2));
            double f4 = cec_basic_funcs_.schwefel_func(Gx.at(3), Os, Mr, false,
                                                       false, Gnx.at(3));
            double f5 = cec_basic_funcs_.ackley_func(Gx.at(4), Os, Mr, false,
                                                     false, Gnx.at(4));
            return f1 + f2 + f3 + f4 + f5;
        }
    };
} // namespace ioh::problem::cec
