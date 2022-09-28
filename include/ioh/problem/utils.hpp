#pragma once

#include <algorithm>
#include <numeric>
#include "ioh/common/random.hpp"

namespace ioh
{
    namespace problem
    {
        /*Problem Utils*/
        namespace utils
        {
            /**
             * \brief Returns a set of indices selected randomly from the range [0, n_variables].
             * Selects floor(n_variables * select_rate) indices with replacement.
             * \param n_variables The range of the index
             * \param select_rate The probability of selection for any given index.
             * \param seed The random seed for the random number generator
             * \return A list of selected indices, in ascending order.
             */
            inline std::vector<int> dummy(const int n_variables, const double select_rate, const long seed)
            {
                const auto select_num = static_cast<int>(floor(n_variables * select_rate));
                const auto random_numbers = common::random::pbo::uniform(static_cast<size_t>(select_num), seed);

                std::vector<int> position(n_variables);
                std::iota(position.begin(), position.end(), 0);

                for (auto i = 0; i != select_num; ++i)
                {
                    const auto random_index = static_cast<int>(floor(random_numbers[i] * 1e4 / 1e4 * n_variables));
                    const auto current_value = position[i];
                    position[i] = position[random_index];
                    position[random_index] = current_value;
                }

                std::sort(position.begin(), position.begin() + select_num);
                return {position.begin(), position.begin() + select_num};
            }

            inline std::vector<int> neutrality(const std::vector<int> &x, const int mu)
            {
                const auto n_variables = static_cast<int>(x.size());
                const auto n = static_cast<int>(floor(static_cast<double>(n_variables) / static_cast<double>(mu)));

                std::vector<int> new_variables;
                new_variables.reserve(n);

                auto cum_sum = 0;

                for (auto i = 0; i < n_variables; i++)
                {
                    cum_sum += x.at(i);
                    if ((i + 1) % mu == 0 && i != 0)
                    {
                        new_variables.push_back(static_cast<int>(cum_sum >= mu / 2.0));
                        cum_sum = 0;
                    }
                }
                return new_variables;
            }

            inline std::vector<int> epistasis(const std::vector<int> &variables, int v)
            {
                int epistasis_result;
                const auto number_of_variables = static_cast<int>(variables.size());
                std::vector<int> new_variables;
                new_variables.reserve(number_of_variables);
                auto h = 0;
                while (h + v - 1 < number_of_variables)
                {
                    auto i = 0;
                    while (i < v)
                    {
                        epistasis_result = -1;
                        for (auto j = 0; j < v; ++j)
                            if (v - j - 1 != (v - i - 1 - 1) % 4)
                                epistasis_result = epistasis_result == -1
                                    ? variables[static_cast<size_t>(j) + h]
                                    : epistasis_result != variables[static_cast<size_t>(j) + h];

                        new_variables.push_back(epistasis_result);
                        ++i;
                    }
                    h += v;
                }
                if (number_of_variables - h > 0)
                {
                    v = static_cast<int>(number_of_variables) - h;
                    for (auto i = 0; i < v; i++)
                    {
                        epistasis_result = -1;
                        for (auto j = 0; j < v; ++j)
                            if (v - j - 1 != (v - i - 1 - 1) % 4)
                                epistasis_result = epistasis_result == -1
                                    ? variables.at(static_cast<size_t>(h) + j)
                                    : epistasis_result != variables.at(static_cast<size_t>(h) + j);
                        new_variables.push_back(epistasis_result);
                    }
                }
                return new_variables;
            }

            inline double ruggedness1(double y, int number_of_variables)
            {
                double ruggedness_y;
                const auto s = static_cast<double>(number_of_variables);
                if (y == s)
                {
                    ruggedness_y = ceil(y / 2.0) + 1.0;
                }
                else if (y < s && number_of_variables % 2 == 0)
                {
                    ruggedness_y = floor(y / 2.0) + 1.0;
                }
                else if (y < s && number_of_variables % 2 != 0)
                {
                    ruggedness_y = ceil(y / 2.0) + 1.0;
                }
                else
                {
                    ruggedness_y = y;
                    assert(y <= s);
                }
                return ruggedness_y;
            }

            inline double ruggedness2(double y, int number_of_variables)
            {
                double ruggedness_y;
                const auto tempy = static_cast<int>(y + 0.5);
                if (tempy == number_of_variables)
                {
                    ruggedness_y = y;
                }
                else if (tempy < number_of_variables && tempy % 2 == 0 && number_of_variables % 2 == 0)
                {
                    ruggedness_y = y + 1.0;
                }
                else if (tempy < number_of_variables && tempy % 2 == 0 && number_of_variables % 2 != 0)
                {
                    ruggedness_y = y - 1.0 > 0 ? y - 1.0 : 0;
                }
                else if (tempy < number_of_variables && tempy % 2 != 0 && number_of_variables % 2 == 0)
                {
                    ruggedness_y = y - 1.0 > 0 ? y - 1.0 : 0;
                }
                else if (tempy < number_of_variables && tempy % 2 != 0 && number_of_variables % 2 != 0)
                {
                    ruggedness_y = y + 1.0;
                }
                else
                {
                    ruggedness_y = y;
                    assert(tempy <= number_of_variables);
                }
                return ruggedness_y;
            }

            inline std::vector<double> ruggedness3(const int number_of_variables)
            {
                std::vector<double> ruggedness_fitness(size_t{1} + static_cast<size_t>(number_of_variables), 0.0);

                for (auto j = 1; j <= number_of_variables / 5; ++j)
                {
                    for (auto k = 0; k < 5; ++k)
                    {
                        ruggedness_fitness[number_of_variables - 5 * j + k] =
                            static_cast<double>(number_of_variables - 5 * j + (4 - k));
                    }
                }
                for (auto k = 0; k < number_of_variables - number_of_variables / 5 * 5; ++k)
                {
                    ruggedness_fitness[k] =
                        static_cast<double>(number_of_variables - number_of_variables / 5 * 5 - 1 - k);
                }
                ruggedness_fitness[number_of_variables] = static_cast<double>(number_of_variables);
                return ruggedness_fitness;
            }

            // Following is the w-model soure code from Raphael's work, which refer the source code of Thomas Weise.
            inline void layer_neutrality_compute(const std::vector<int> xIn, std::vector<int> &xOut, const int mu)
            {
                const auto thresholdFor1 = (mu >> 1) + (mu & 1);
                int temp;
                const auto dim = static_cast<int>(xIn.size());
                const auto temp_dim = dim / mu;
                if (static_cast<int>(xOut.size()) != temp_dim)
                {
                    xOut.resize(temp_dim);
                }
                auto i = 0;
                auto j = 0;
                auto ones = 0;
                auto flush = mu;
                while (i < dim && j < temp_dim)
                {
                    if (xIn[i] == 1)
                    {
                        ones += 1;
                    }
                    i += 1;
                    if (i >= flush)
                    {
                        flush += mu;
                        if (ones >= thresholdFor1)
                        {
                            temp = 1;
                        }
                        else
                        {
                            temp = 0;
                        }
                        xOut[j] = temp;
                        j += 1;
                        ones = 0;
                    }
                }
            }

            inline void base_epistasis(const std::vector<int> &xIn, const int start, const int nu,
                                       std::vector<int> &xOut)
            {
                const auto end = start + nu - 1;
                const auto flip = xIn[start];
                auto skip = start;
                for (auto i = end; i >= start; --i)
                {
                    auto result = flip;
                    for (auto j = end; j > start; --j)
                    {
                        if (j != skip)
                        {
                            result ^= xIn[j];
                        }
                    }
                    xOut[i] = result;
                    if (--skip < start)
                    {
                        skip = end;
                    }
                }
            }

            inline void epistasis_compute(const std::vector<int> &xIn, std::vector<int> &xOut, const int nu)
            {
                const auto length = static_cast<int>(xIn.size());
                const auto end = length - nu;
                int i;
                for (i = 0; i <= end; i += nu)
                {
                    base_epistasis(xIn, i, nu, xOut);
                }
                if (i < length)
                {
                    base_epistasis(xIn, i, static_cast<int>(length - i), xOut);
                }
            }

            inline void layer_epistasis_compute(const std::vector<int> &x, std::vector<int> &epistasis_x,
                                                const int block_size)
            {
                epistasis_compute(x, epistasis_x, block_size);
            }

            inline int max_gamma(int q) { return static_cast<int>(q * (q - 1) >> 1); }

            inline std::vector<int> ruggedness_raw(int gamma, int q)
            {
                int i, j, start;
                std::vector<int> r(q + 1, 0);
                r[0] = 0;
                const auto max = max_gamma(q);
                if (gamma <= 0)
                {
                    start = 0;
                }
                else
                {
                    start = q - 1 - static_cast<int>(0.5 + sqrt(0.25 + ((max - gamma) << 1)));
                }
                auto k = 0;
                for (j = 1; j <= start; j++)
                {
                    if ((j & 1) != 0)
                    {
                        r[j] = q - k;
                    }
                    else
                    {
                        k = k + 1;
                        r[j] = k;
                    }
                }
                for (; j <= q; j++)
                {
                    k = k + 1;
                    if ((start & 1) != 0)
                    {
                        r[j] = q - k;
                    }
                    else
                    {
                        r[j] = k;
                    }
                }
                const auto upper = gamma - max + ((q - start - 1) * (q - start) >> 1);
                j--;
                for (i = 1; i <= upper; i++)
                {
                    j = j - 1;

                    if (j > 0)
                    {
                        const auto t = r[j];
                        r[j] = r[q];
                        r[q] = t;
                    }
                }

                std::vector<int> r2(1 + q, 0);
                for (i = 0; i <= q; i++)
                {
                    r2[i] = q - r[q - i];
                }
                return r2;
            }

            inline int ruggedness_translate(int gamma, int q)
            {
                int j, k;

                if (gamma <= 0)
                {
                    return 0;
                }
                const auto g = gamma;
                const auto max = max_gamma(q);
                const auto lastUpper = (q >> 1) * ((q + 1) >> 1);
                if (g <= lastUpper)
                {
                    j = abs(static_cast<int>((q + 2) * 0.5 - sqrt(q * q * 0.25 + 1 - g)));

                    k = g - (q + 2) * j + j * j + q;
                    return k + 1 + (((q + 2) * j - j * j - q - 1) << 1) - (j - 1);
                }

                j = abs(static_cast<int>((q % 2 + 1) * 0.5 + sqrt((1 - q % 2) * 0.25 + g - 1 - lastUpper)));

                k = g - ((j - q % 2) * (j - 1) + 1 + lastUpper);

                return max - k - (2 * j * j - j) - q % 2 * (-2 * j + 1);
            }

            //! Helper function which forwards its first argument
            template<typename T, typename... Args>
            T identity(T p, Args &&...) {return p;}

        } // namespace utils
    } // namespace problem
} // namespace ioh
