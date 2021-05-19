#pragma once

#include <cmath>
#include <random>
#include <vector>

#include "config.hpp"


namespace ioh::common::random
{
    /**
     * \brief Linear congruential generator using a given seed. Used to generate uniform random numbers.
     * \param seed Random seed
     * \return A new seed
     */
    inline long lcg_rand(long seed)
    {
        const auto double_seed = static_cast<double>(seed);
        const auto double_mod_div = static_cast<double>(IOH_RND_MODULUS_DIV);
        const auto seed_mod = static_cast<long>(std::floor(double_seed / double_mod_div));

        seed = static_cast<long>(IOH_RND_MULTIPLIER * (seed - seed_mod * IOH_RND_MODULUS_DIV) - IOH_RND_MOD_MULTIPLIER *
            seed_mod);

        if (seed < 0)
            seed = seed + IOH_RND_MODULUS;
        return seed;
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
    static std::vector<int> bit_string(const unsigned int n, const double p = 0.5)
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
            const auto seed_index = static_cast<int>(std::floor(
                static_cast<double>(seed) / static_cast<double>(67108865)));

            seed = rand_seed[seed_index];
            rand_seed[seed_index] = rand_value;

            rand_vec.emplace_back(
                static_cast<double>(seed) / 2.147483647e9);
            if (rand_vec[i] == 0.)
                rand_vec[i] = 1e-99;
        }
    }

    /**
     * \brief Generate a vector of n uniform doubles, with a given seed
     * \param n the size of the vector
     * \param lb lower bound for the random numbers
     * \param ub upper bound for the random numbers
     * \param seed the seed passed to the random number generator
     * \return a vector of uniform random numbers
     */
    inline std::vector<double> uniform(const unsigned int n, const double lb = 0, const double ub = 1,
                                       const int seed = 0)
    {
        std::vector<double> numbers;
        uniform(n, seed, numbers);
        for (auto &xi : numbers)
            xi = xi * (ub - lb) + lb;
        return numbers;
    }


    /**
     * \brief Generates a vector of size n, containing random gaussian numbers
     * \param n The size of the vector
     * \param seed The seed to be used
     * \param lb lower bound for the random numbers
     * \param ub upper bound for the random numbers
     * \return A vector of random numbers
     */
    inline std::vector<double> normal(const size_t n, const long seed, const double lb = 0, const double ub = 1)
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
            rand_vec[i] = rand_vec[i] * (ub - lb) + lb;
        }
        return rand_vec;
    }
};
