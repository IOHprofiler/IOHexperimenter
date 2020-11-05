#pragma once
#include <cstdint>
#include <vector>
#include <random>
#include <cmath>

#include "config.hpp"

 
namespace ioh
{
	namespace common
	{
		class random
		{ 
			size_t _seed_index;

			// TODO: check why this is done
			inline static long a = IOH_RND_MULTIPLIER;		/// < multiplier.
			inline static long m = IOH_RND_MODULUS;			/// < modulus.
			inline static long q = IOH_RND_MODULUS_DIV;		/// < modulusdiv multiplier.
			inline static long r = IOH_RND_MOD_MULTIPLIER;	/// < modulus mod multiplier.
			inline static unsigned int short_lag = IOH_SHORT_LAG;
			inline static unsigned int long_lag = IOH_LONG_LAG;
			double x[607];
			
		public:
			random(uint32_t seed = IOH_DEFAULT_SEED) : _seed_index(0)
			{
				for (unsigned int i = 0; i < long_lag; ++i)
				{
					this->x[i] = static_cast<double>(seed) / static_cast<double>((static_cast<uint64_t>(1UL) << 32) - 1);
					seed = static_cast<uint32_t>(1812433253UL) * (seed ^ (seed >> 30)) + (static_cast<uint32_t>(i) + 1);
				}
			}

			void generate()
			{
				for (unsigned int i = 0; i < short_lag; ++i)
				{
					double t = this->x[i] + this->x[i + (long_lag - short_lag)];
					if (t >= 1.0)
						t -= 1.0;
					this->x[i] = t;
				}
				for (unsigned int i = short_lag; i < long_lag; ++i)
				{
					double t = this->x[i] + this->x[i - short_lag];
					if (t >= 1.0)
						t -= 1.0;
					this->x[i] = t;
				}
				this->_seed_index = 0;
			}

			static long _lcg_rand(const long& inseed)
			{
				long new_inseed = static_cast<long>(a * (inseed - static_cast<long>(std::floor(static_cast<double>(inseed)
					/ static_cast<double>(q))) * q) - r * static_cast<long>(std::floor(
						static_cast<double>(inseed) / static_cast<double>(q))));
				if (new_inseed < 0)
				{
					new_inseed = new_inseed + m;
				}
				return new_inseed;
			}

			static void uniform_rand(const size_t& N, const long& inseed, std::vector<double>& rand_vec)
			{
				if (rand_vec.size() != 0)
				{
					std::vector<double>().swap(rand_vec);
				}
				rand_vec.reserve(N);

				long rand_seed[32];
				long seed;
				long rand_value;

				seed = inseed;
				for (int i = 39; i >= 0; --i)
				{
					seed = _lcg_rand(seed);
					if (i < 32)
					{
						rand_seed[i] = seed;
					}
				}

				int seed_index = 0;
				seed = rand_seed[0];
				for (unsigned int i = 0; i < N; ++i)
				{
					rand_value = _lcg_rand(seed);

					seed_index = static_cast<int>(std::floor(static_cast<double>(seed) / static_cast<double>(67108865)));
					seed = rand_seed[seed_index];
					rand_seed[seed_index] = rand_value;

					rand_vec.emplace_back(static_cast<double>(seed) / 2.147483647e9);
					if (rand_vec[i] == 0.)
					{
						rand_vec[i] = 1e-99;
					}
				}
			}

			static std::vector<double> gauss(const size_t N, const long inseed)
			{
				std::vector<double> rand_vec;
				std::vector<double> uniform_rand_vec;
				rand_vec.reserve(N);

				const long seed = (std::max)(1L, std::abs(inseed));
				uniform_rand(2 * N, seed, uniform_rand_vec);

				for (unsigned int i = 0; i < N; i++)
				{
					rand_vec.emplace_back(
						std::sqrt(-2 * std::log(uniform_rand_vec[i])) * std::cos(2 * IOH_PI * uniform_rand_vec[N + i]));
					if (rand_vec[i] == 0.)
					{
						rand_vec[i] = 1e-99;
					}
				}
				return rand_vec;
			}

			// TODO: I think only these methods should be public
			double uniform_rand()
			{
				if (this->_seed_index >= long_lag)
					generate();
				return this->x[this->_seed_index++];
			}

			double normal_rand()
			{
				double normal;
#ifdef IOH_NORMAL_POLAR
				const double u1 = this->uniform_rand();
				const double u2 = this->uniform_rand();
				normal = std::sqrt(-2 * std::log(u1)) * std::cos(2 * IOH_PI * u2);
#else
				normal = 0.0;
				for (int i = 0; i < 12; ++i) {
					normal += this->uniform_rand();
				}
				normal -= 6.0;
#endif
				return normal;
			}

			static int bit(double p = 0.5)
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
			static int integer(const int min = std::numeric_limits<int>::min(), const int max = std::numeric_limits<int>::max())
			{
				static std::random_device rd;
				static std::mt19937 gen(rd());
				std::uniform_int_distribution<int> d(min, max);
				return d(gen);
			}
			/**
			 * \brief 
			 * \param n 
			 * \param min 
			 * \param max 
			 * \return 
			 */
			static std::vector<int> integers(const unsigned int n, const int min = std::numeric_limits<int>::min(), const int max = std::numeric_limits<int>::max())
			{
				std::vector<int> x;
				x.reserve(n);
				for (auto i = 0; i < n; i++)
					x.emplace_back(integer(min, max));
				return x;
			}
			static std::vector<int> bitstring(const unsigned int n, const double p = 0.5)
			{
				std::vector<int> x;
				x.reserve(n);
				for (auto i = 0; i < n; i++)
					x.emplace_back(bit(p));
				return x;
			}			
		};
	}
}
