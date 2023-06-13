#pragma once

#include "ioh/problem/problem.hpp"
#include "ioh/problem/transformation.hpp"

#define INF 1.0e99
#define E 2.7182818284590452353602874713526625


namespace ioh::problem::cec
{
    class CecUtils
    {
    public:
        void shiftfunc(std::vector<double> &x, const std::vector<double> &Os,
                       int nx)
        {
            for (int i = 0; i < nx; i++)
            {
                x.at(i) = x.at(i) - Os.at(i);
            }
        }

        void rotatefunc(std::vector<double> &x, const std::vector<double> &Mr,
                        int nx)
        {
            std::vector<double> cacheX(x);
            for (int i = 0; i < nx; i++)
            {
                x.at(i) = 0;
                for (int j = 0; j < nx; j++)
                {
                    x.at(i) = x.at(i) + cacheX.at(j) * Mr.at(i * nx + j);
                }
            }
        }

        void sr_func(std::vector<double> &x, const std::vector<double> &Os,
                     const std::vector<double> &Mr, double sh_rate, bool s_flag,
                     bool r_flag, int nx)
        {
            if (s_flag)
            {
                shiftfunc(x, Os, nx);
            }
            for (int i = 0; i < nx; i++)
            {
                x.at(i) = x.at(i) * sh_rate;
            }
            if (r_flag)
            {
                rotatefunc(x, Mr, nx);
            }
        }

        double getFunctionBias(const int biasFlag, const int fnNumber)
        {
            double bias = 0.0;
            double fnBiasDict[10] = {100.0,  1100.0, 700.0,  1900.0, 1700.0,
                                     1600.0, 2100.0, 2200.0, 2400.0, 2500.0};
            if (biasFlag)
            {
                bias = fnBiasDict[fnNumber - 1];
            }
            else
            {
                bias = 0.0;
            }
            return bias;
        }

        void loadMatrixData(std::vector<double> &Mr, std::string dataPath,
                            int dim, int fn, int cecVersion)
        {
            int funcTreshold, coeff = 0;
            if (cecVersion == 2014)
            {
                funcTreshold = 23;
                coeff = 10;
            }
            else if (cecVersion == 2015)
            {
                int cf_nums[] = {0, 1, 1, 1, 1, 1, 1, 1,
                                 1, 3, 3, 5, 5, 5, 7, 10};
                funcTreshold = -1;
                coeff = cf_nums[fn];
            }
            else if (cecVersion == 2017)
            {
                funcTreshold = 20;
                coeff = 10;
            }
            else if (cecVersion == 2019)
            {
                funcTreshold = 100;
                coeff = 1;
            }
            else if (cecVersion == 2021)
            {
                funcTreshold = 7;
                coeff = 10;
            }
            else if (cecVersion == 2022)
            {
                funcTreshold = 9;
                coeff = 12;
            }
            else
            {
                funcTreshold = -1;
                coeff = -1;
            }
            std::string FileName;
            std::stringstream ss;
            std::ifstream fptMData;
            ss << dataPath << "/cec" << std::to_string(cecVersion) << "/M_"
               << std::to_string(fn) << "_D" << std::to_string(dim) << ".txt";
            FileName = ss.str();
            fptMData.open(FileName);
            if (!fptMData)
            {
                std::cout << FileName << std::endl;
                perror("Error: Cannot open input file for reading");
            }
            int MatrixSize = fn < funcTreshold ? dim * dim : dim * dim * coeff;
            for (int i = 0; i < MatrixSize; ++i)
            {
                if (fptMData.peek() == EOF)
                {
                    break;
                }
                double matrixData;
                fptMData >> matrixData;
                Mr.push_back(matrixData);
            }
            fptMData.close();
        }

        void loadOShiftData(std::vector<double> &Os, std::string dataPath,
                            int dim, int fn, int cecVersion)
        {
            int funcTreshold = 0;
            int coeff = 0;
            if (cecVersion == 2014)
            {
                funcTreshold = 23;
                coeff = 10;
            }
            else if (cecVersion == 2015)
            {
                int coeffs[] = {0, 1, 1, 1, 1, 1, 1, 1,
                                1, 3, 3, 5, 5, 5, 7, 10};
                coeff = coeffs[fn];
            }
            else if (cecVersion == 2017)
            {
                funcTreshold = 20;
                coeff = 10;
            }
            else if (cecVersion == 2019)
            {
                funcTreshold = 100;
                coeff = 1;
            }
            else if (cecVersion == 2022)
            {
                funcTreshold = 9;
                coeff = 12;
            }
            else
            {
                funcTreshold = -1;
                coeff = -1;
            }
            std::string FileName;
            std::stringstream ss;
            std::ifstream fptOShiftData;
            ss << dataPath << "/cec" << std::to_string(cecVersion)
               << "/shift_data_" << std::to_string(fn) << ".txt";
            FileName = ss.str();
            fptOShiftData.open(FileName);
            if (!fptOShiftData)
            {
                perror("Error: Cannot open input file for reading");
            }
            int OShiftSize = fn < funcTreshold ? dim : coeff * dim;
            if (fn < funcTreshold)
            {
                for (int i = 0; i < OShiftSize; ++i)
                {
                    if (fptOShiftData.peek() == EOF)
                    {
                        break;
                    }
                    double shiftData;
                    fptOShiftData >> shiftData;
                    Os.push_back(shiftData);
                }
            }
            // else
            // {
            //     for (int i = 0; i < coeff - 1; i++)
            //     {
            //         for (int j = 0; j < dim; j++)
            //         {
            //             int count =
            //                 fscanf(fptOShiftData, "%lf", &cd->OShift[i * dim
            //                 + j]);
            //             if (count == -1)
            //             {
            //                 break;
            //             }
            //         }
            //         int count = fscanf(fptOShiftData, "%*[^\n]%*c");
            //         if (count == -1)
            //         {
            //             break;
            //         }
            //     }
            //     for (int j = 0; j < dim; j++)
            //     {
            //         if (fscanf(fptOShiftData, "%lf",
            //                    &cd->OShift[(coeff - 1) * dim + j]) == -1)
            //         {
            //             break;
            //         }
            //     }
            // }
            fptOShiftData.close();
        }

        void loadShuffleData(std::vector<int> &SS, std::string dataPath,
                             int dim, int fn, int cecVersion)
        {
            int coeff = 0;
            int shuffleFlag = 0;
            if (cecVersion == 2014 || cecVersion == 2017 ||
                cecVersion == 2019 || cecVersion == 2021 || cecVersion == 2022)
            {
                coeff = 10;
            }
            else if (cecVersion == 2015)
            {
                int cf_nums[] = {0, 1, 1, 1, 1, 1, 1, 1,
                                 1, 3, 3, 5, 5, 5, 7, 10};
                coeff = cf_nums[fn];
            }
            if (cecVersion == 2014)
            {
                shuffleFlag = (fn >= 17 && fn <= 22) ? 1 : 0;
            }
            else if (cecVersion == 2015)
            {
                shuffleFlag = 0;
            }
            else if (cecVersion == 2017)
            {
                shuffleFlag = (fn >= 11 && fn <= 20) ? 1 : 0;
            }
            else if (cecVersion == 2021)
            {
                shuffleFlag = (fn >= 5 && fn <= 7) ? 1 : 0;
            }
            else if (cecVersion == 2022)
            {
                shuffleFlag = (fn >= 6 && fn <= 8) ? 1 : 0;
            }

            std::string FileName;
            std::stringstream ss;
            std::ifstream fptShuffleData;
            ss << dataPath << "/cec" << std::to_string(cecVersion)
               << "/shuffle_data_" << std::to_string(fn) << "_D"
               << std::to_string(dim) << ".txt";
            FileName = ss.str();
            fptShuffleData.open(FileName);
            if (!fptShuffleData)
            {
                perror("Error: Cannot open input file for reading");
            }
            int ShuffleSize = shuffleFlag ? dim : coeff * dim;
            for (int i = 0; i < ShuffleSize; ++i)
            {
                if (fptShuffleData.peek() == EOF)
                {
                    break;
                }
                double shuffleData;
                fptShuffleData >> shuffleData;
                SS.push_back(shuffleData);
            }
            fptShuffleData.close();
        }


        double cf_cal(std::vector<double> &x, std::vector<double> &Os,
                      std::vector<double> &delta, std::vector<double> &bias,
                      std::vector<double> &fit, int cf_num, int nx)
        {
            double f = 0.0;
            double w_max = 0.0, w_sum = 0.0;
            std::vector<double> w(cf_num, 0);
            for (int i = 0; i < cf_num; i++)
            {
                fit.at(i) += bias.at(i);
                for (int j = 0; j < nx; j++)
                {
                    w.at(i) += pow(x.at(j) - Os.at(i * nx + j), 2.0);
                }
                if (w.at(i) != 0)
                {
                    w.at(i) = pow(1.0 / w.at(i), 0.5) *
                        exp(-w.at(i) / 2.0 / nx / pow(delta.at(i), 2.0));
                }
                else
                {
                    w.at(i) = INF;
                }
                if (w.at(i) > w_max)
                    w_max = w.at(i);
            }
            for (int i = 0; i < cf_num; i++)
            {
                w_sum = w_sum + w.at(i);
            }
            if (w_max == 0)
            {
                for (int i = 0; i < cf_num; i++)
                    w.at(i) = 1;
                w_sum = cf_num;
            }
            for (int i = 0; i < cf_num; i++)
            {
                f = f + w.at(i) / w_sum * fit.at(i);
            }
            return f;
        }
    };
} // namespace ioh::problem::cec
