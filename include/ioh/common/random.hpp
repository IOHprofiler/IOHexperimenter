#pragma once
#include <cstdint>
#include <vector>
#include <random>
#include <cmath>

#include "defines.hpp"
;

namespace ioh
{
	namespace common
	{
		class random
		{
		public:
			random(uint32_t seed = DEFAULT_SEED);

			void generate();

			static long _lcg_rand(const long& inseed);

			static void uniform_rand(const size_t& N, const long& inseed, std::vector<double>& rand_vec);

			static std::vector<double> gauss(size_t N, long inseed);

			// TODO: I think only these methods should be public
			double uniform_rand();

			double normal_rand();

			static bool bit(double p = 0.5);
			static int integer(int min = INT_MIN, int max = INT_MAX);
			static std::vector<int> integers(unsigned int n, int min=INT_MIN, int max=INT_MAX);
			static std::vector<bool> bitstring(unsigned int n, double p = 0.5);


		private:
			size_t _seed_index;

			// TODO: check why this is done
			inline static long a = RND_MULTIPLIER; /// < multiplier.
			inline static long m = RND_MODULUS; /// < modulus.
			inline static long q = RND_MODULUS_DIV; /// < modulusdiv multiplier.
			inline static long r = RND_MOD_MULTIPLIER; /// < modulus mod multiplier.
			inline static unsigned int short_lag = SHORT_LAG;
			inline static unsigned int long_lag = LONG_LAG;
			double x[607];
		};
	}
}
