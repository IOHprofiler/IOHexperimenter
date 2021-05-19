#pragma once
#include <cmath>
#include <cstdint>
#include <random>
#include <vector>

#include "config.hpp"


namespace ioh
{
    namespace common
    {
        /**
         * \brief Class containing several random number generators
         */
        class Random
        {
            size_t seed_index_;
            double x_[607];

            /**
             * \brief Generates the values for x_ and resets the seed_index_
             */
            void generate()
            {
                for (unsigned int i = 0; i < IOH_SHORT_LAG; ++i)
                {
                    auto t = this->x_[i] + this->x_[
                        i + (IOH_LONG_LAG - IOH_SHORT_LAG)];
                    if (t >= 1.0)
                        t -= 1.0;
                    this->x_[i] = t;
                }
                for (unsigned int i = IOH_SHORT_LAG; i < IOH_LONG_LAG; ++i)
                {
                    auto t = this->x_[i] + this->x_[i - IOH_SHORT_LAG];
                    if (t >= 1.0)
                        t -= 1.0;
                    this->x_[i] = t;
                }
            }


            /**
             * \brief Linear congruential generator using a given seed. Used to generate
             * uniform random numbers.
             * \param seed Random seed
             * \return A new seed
             */
            static long lcg_rand(long seed)
            {
                const auto double_seed = static_cast<double>(seed);
                const auto double_mod_div = static_cast<double>(
                    IOH_RND_MODULUS_DIV);
                const auto seed_mod = static_cast<long>(std::floor(
                    double_seed / double_mod_div));

                seed = static_cast<long>(IOH_RND_MULTIPLIER * (
                        seed - seed_mod *
                        IOH_RND_MODULUS_DIV) -
                    IOH_RND_MOD_MULTIPLIER * seed_mod);

                if (seed < 0)
                    seed = seed + IOH_RND_MODULUS;
                return seed;
            }


        public:
            /**
             * \brief Initializes x_ and resets the internally used seed. This code was
             * ported from COCO. 
             * \param seed The random seed
             */
            explicit Random(uint32_t seed = IOH_DEFAULT_SEED) :
                seed_index_(0), x_{0}
            {
                for (unsigned int i = 0; i < IOH_LONG_LAG; ++i)
                {
                    this->x_[i] = static_cast<double>(seed) / static_cast<double>(
                        (static_cast<uint64_t>(1UL) << 32) - 1);
                    seed = static_cast<uint32_t>(1812433253UL) * (
                        seed ^ seed >> 30) + (
                        static_cast<uint32_t>(i) + 1);
                }
            }

            /**
             * \brief Returns a uniform random number
             * \return A uniform random number
             */
            double uniform()
            {
                if (this->seed_index_ >= IOH_LONG_LAG)
                {
                    generate();
                    this->seed_index_ = 0;
                }
                return this->x_[this->seed_index_++];
            }

            /**
             * \brief Returns a normal/gaussian random number
             * \return A normal/gaussian random number
             */
            double normal()
            {
                double normal;
#ifdef IOH_NORMAL_POLAR
                const auto u1 = this->uniform();
                const auto u2 = this->uniform();
                normal = std::sqrt(-2 * std::log(u1)) * std::cos(
                    2 * IOH_PI * u2);
#else
				normal = 0.0;
				for (int i = 0; i < 12; ++i) {
					normal += this->uniform();
				}
				normal -= 6.0;
#endif
                return normal;
            }


            /**
             * \brief Generate a vector of n uniform doubles, with a given seed using
             * the std::mt19937 random number generator. 
             * \param n the size of the vector
             * \param min lower bound for the random numbers
             * \param max upper bound for the random numbers
             * \param seed the seed passed to the random number generator
             * \return a vector of uniform random numbers
             */
            static std::vector<double> uniform(const unsigned int n, const double min = 0,
                                               const double max = 1, const int seed = 0)
            {
                static auto seed0 = seed;
                static std::mt19937 re(seed);
                if (seed0 != seed)
                {
                    re.seed(seed);
                    seed0 = seed;
                }

                std::uniform_real_distribution<double> d(min, max);


                std::vector<double> x;
                for (unsigned int i = 0; i < n; i++)
                    x.emplace_back(d(re));
                return x;
            }

            /**
             * \brief Returns a random integer [0, 1], sampled from a bernoulli distribution.
             * \param p The mean of the distribution
             * \return A random bit
             */
            static int bit(const double p = 0.5)
            {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::bernoulli_distribution d(p);
                return d(gen);
            }

            /**
             * \brief This function returns a random uniform integer within the range [min, max]
             * \param min the minimal boundary
             * \param max the maximum boundary
             * \return a random uniform integer
             */
            static int integer(const int min = std::numeric_limits<int>::min(),
                               const int max = std::numeric_limits<int>::max())
            {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_int_distribution<int> d(min, max);
                return d(gen);
            }

            /**
             * \brief This function returns a vector of random uniform integer within the range [min, max] of size n
             * \param n The size of the vector
             * \param min The minimal boundary
             * \param max The maximum boundary
             * \return A vector of uniform random integers
             */
            static std::vector<int> integers(const unsigned int n,
                                             const int min = std::numeric_limits
                                                 <int>::min(),
                                             const int max = std::numeric_limits
                                                 <int>::max())
            {
                std::vector<int> x;
                for (unsigned int i = 0; i < n; i++)
                    x.emplace_back(integer(min, max));
                return x;
            }

            /**
             * \brief Returns a vector of random integers [0, 1], sampled from a bernoulli distribution, of size 
             * \param n The size of the vector
             * \param p The mean of the distribution
             * \return A vector of random bits
             */
            static std::vector<int>
            bit_string(const unsigned int n, const double p = 0.5)
            {
                std::vector<int> x;
                for (unsigned int i = 0; i < n; i++)
                    x.emplace_back(bit(p));
                return x;
            }

            /**
             * \brief Fills a rand_vec with n uniform random numbers, generated using in_seed
             * \param n The size of rand_vec
             * \param seed The random seed
             * \param rand_vec The vector in which to place the random numbers
             */
            static void uniform(const size_t &n, long seed,
                                std::vector<double> &rand_vec)
            {
                if (!rand_vec.empty())
                    std::vector<double>().swap(rand_vec);
                rand_vec.reserve(n);

                long rand_seed[32];

                for (auto i = 39; i >= 0; --i)
                {
                    seed = lcg_rand(seed);
                    if (i < 32)
                        rand_seed[i] = seed;
                }

                seed = rand_seed[0];
                for (unsigned int i = 0; i < n; ++i)
                {
                    const auto rand_value = lcg_rand(seed);
                    const auto seed_index = static_cast<int>(std::floor(static_cast<double>(seed) / static_cast<double>(67108865)));

                    seed = rand_seed[seed_index];
                    rand_seed[seed_index] = rand_value;

                    rand_vec.emplace_back(
                        static_cast<double>(seed) / 2.147483647e9);
                    if (rand_vec[i] == 0.)
                        rand_vec[i] = 1e-99;
                }
            }

            /**
             * \brief Generates a vector of size n, containing random gaussian numbers
             * \param n The size of the vector
             * \param seed The seed to be used
             * \return A vector of random numbers
             */
            static std::vector<double> normal(const size_t n, const long seed)
            {
                std::vector<double> rand_vec;
                std::vector<double> uniform_rand_vec;
                rand_vec.reserve(n);

                uniform(2 * n, std::max(1L, std::abs(seed)), uniform_rand_vec);

                for (unsigned int i = 0; i < n; i++)
                {
                    rand_vec.emplace_back(
                        std::sqrt(-2 * std::log(uniform_rand_vec[i])) *
                            std::cos(2 * IOH_PI * uniform_rand_vec[n + i]));
                    if (rand_vec[i] == 0.)
                        rand_vec[i] = 1e-99;
                }
                return rand_vec;
            }
        };
    }
}
