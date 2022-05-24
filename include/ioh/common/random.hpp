#pragma once

#include <array>
#include <cassert>
#include <cmath>
#include <limits>
#include <random>
#include <vector>

#define IOH_RND_MULTIPLIER 16807 // NOLINT
#define IOH_RND_MODULUS 2147483647 // NOLINT
#define IOH_RND_MODULUS_DIV 127773 // NOLINT
#define IOH_RND_MOD_MULTIPLIER 2836 // NOLINT
#define IOH_PI 3.14159265358979323846 // NOLINT

/** Random utilities. */
namespace ioh::common::random
{
    
    //! Global random device
    inline std::random_device rd;
    
    //! Global random generator
    inline std::mt19937 gen(rd());

    /**
     * @brief Seed the random number generator device. This is a std::mt19937 rng
     */
    inline void seed(const unsigned int seed){
        gen.seed(seed);
    }
    
    /**
     * \brief Linear congruential generator using a given seed. Used to generate uniform random numbers.
     * \param seed Random seed
     * \return A new seed
     */
    inline long lcg_rand(long seed)
    {
        static auto double_mod_div = static_cast<double>(IOH_RND_MODULUS_DIV);
        const auto double_seed = static_cast<double>(seed);

        const auto seed_mod = static_cast<long>(std::floor(double_seed / double_mod_div));

        seed = static_cast<long>(IOH_RND_MULTIPLIER * (seed - seed_mod * IOH_RND_MODULUS_DIV) -
                                 IOH_RND_MOD_MULTIPLIER * seed_mod);

        if (seed < 0)
            seed = seed + IOH_RND_MODULUS;
        return seed;
    }

    /**
     * \brief This function returns a random uniform integer within the range [min, max]
     * \param min the minimal boundary
     * \param max the maximum boundary
     * \return a random uniform integer
     */
    inline int integer(const int min = std::numeric_limits<int>::min(), const int max = std::numeric_limits<int>::max())
    {
        std::uniform_int_distribution<int> d(min, max);
        return d(gen);
    }

    /**
     * \brief This function returns a random uniform double within the range [min, max]
     * \param min the minimal boundary
     * \param max the maximum boundary
     * \return a random double
     */
    inline double real(const double min = 0.0, const double max = 1.0)
    {
        std::uniform_real_distribution<double> d(min, max);
        return d(gen);
    }

    /**
     * \brief This function returns a vector of random uniform integer within the range [min, max] of size n
     * \param n The size of the vector
     * \param min The minimal boundary
     * \param max The maximum boundary
     * \return A vector of uniform random integers
     */
    inline std::vector<int> integers(const unsigned int n, const int min = std::numeric_limits<int>::min(),
                                     const int max = std::numeric_limits<int>::max())
    {
        std::vector<int> x;
        for (unsigned int i = 0; i < n; i++)
            x.emplace_back(integer(min, max));
        return x;
    }

     /**
     * \brief This function returns a vector random uniform doubles within the range [min, max]
     * \param n The size of the vector
     * \param min the minimal boundary
     * \param max the maximum boundary
     * \return vector of uniform random doubles
     */
    inline std::vector<double> doubles(const unsigned int n, const double min = 0.0, const double max = 1.0)
    {
        std::vector<double> x;
        for (unsigned int i = 0; i < n; i++)
            x.emplace_back(real(min, max));
        return x;
    }

    namespace pbo
    {

        /**
         * \brief Fills a rand_vec with n uniform random numbers, generated using in_seed
         * \param n The size of rand_vec
         * \param seed The random seed
         * \param lb lower bound for the random numbers
         * \param ub upper bound for the random numbers
         */
        inline std::vector<double> uniform(const size_t &n, long seed, const double lb = 0, const double ub = 1)
        {
            auto rand_vec = std::vector<double>(n);
            long rand_seed[32] = {};

            for (auto i = 39; i >= 0; --i)
            {
                seed = lcg_rand(seed);
                if (i < 32)
                    rand_seed[i] = seed;
            }

            for (unsigned int i = 0; i < n; ++i)
            {
                const auto rand_value = lcg_rand(seed);
                const auto seed_index =
                    static_cast<int>(std::floor(static_cast<double>(seed) / static_cast<double>(67108865)));

                seed = rand_seed[seed_index];
                rand_seed[seed_index] = rand_value;

                rand_vec[i] = static_cast<double>(seed) / 2.147483647e9;
                if (rand_vec[i] == 0.)
                    rand_vec[i] = 1e-99;

                rand_vec[i] = rand_vec[i] * (ub - lb) + lb;
            }

            return rand_vec;
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
            const auto uniform_rand_vec = uniform(2 * n, std::max(1L, std::abs(seed)));
            std::vector<double> rand_vec;
            rand_vec.reserve(n);

            for (unsigned int i = 0; i < n; i++)
            {
                rand_vec.emplace_back(std::sqrt(-2 * std::log(uniform_rand_vec[i])) *
                                      std::cos(2 * IOH_PI * uniform_rand_vec[n + i]));
                if (rand_vec[i] == 0.)
                    rand_vec[i] = 1e-99;
                rand_vec[i] = rand_vec[i] * (ub - lb) + lb;
            }
            return rand_vec;
        }

    } // namespace pbo

    namespace bbob2009
    {
        inline std::vector<double> uniform(const size_t n, const int initial_seed, const double lb = 0,
                                           const double ub = 1)
        {
            auto generators = std::array<int, 32>();
            auto seed = std::max(1, abs(initial_seed));

            // Initialize the seed and generator
            for (auto i = 39; i >= 0; i--)
            {
                seed = lcg_rand(seed);
                if (i < 32)
                    generators[i] = seed;
            }


            auto x = std::vector<double>(n);
            auto random_number = generators.front();

            for (size_t i = 0; i < n; i++)
            {
                const auto index = static_cast<int>(floor(random_number / 67108865.0));

                seed = lcg_rand(seed);
                random_number = generators[index];
                generators[index] = seed;

                x.at(i) = random_number / 2.147483647e9;
                if (x.at(i) == 0.)
                    x.at(i) = 1e-99;

                x.at(i) = x.at(i) * (ub - lb) + lb;
            }
            return x;
        }

        inline std::vector<double> normal(const size_t n, const long seed, const double lb = 0, const double ub = 1)
        {
            assert(2 * n < 6000);
            const auto uniform_random = uniform(2 * n, seed);

            std::vector<double> x(n);

            for (size_t i = 0; i < n; i++)
            {
                x.at(i) = sqrt(-2 * std::log(uniform_random[i])) * cos(2 * IOH_PI * uniform_random[n + i]);
                if (x.at(i) == 0.)
                    x.at(i) = 1e-99;
                x.at(i) = x.at(i) * (ub - lb) + lb;
            }
            return x;
        } // namespace random
    } // namespace bbob2009
} // namespace ioh::common::random
