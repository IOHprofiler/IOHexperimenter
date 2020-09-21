#include "ioh/common/random.hpp"



namespace ioh
{
	namespace common
	{
		random::random(uint32_t seed) : _seed_index(0)
		{
			for (int i = 0; i < long_lag; ++i) {
				this->x[i] = ((double)seed) / (double)(((uint64_t)1UL << 32) - 1);
				seed = (uint32_t)1812433253UL * (seed ^ (seed >> 30)) + ((uint32_t)i + 1);
			}
		}

		void random::generate()
		{
			for (int i = 0; i < short_lag; ++i) {
				double t = this->x[i] + this->x[i + (long_lag - short_lag)];
				if (t >= 1.0)
					t -= 1.0;
				this->x[i] = t;
			}
			for (int i = short_lag; i < long_lag; ++i) {
				double t = this->x[i] + this->x[i - short_lag];
				if (t >= 1.0)
					t -= 1.0;
				this->x[i] = t;
			}
			this->_seed_index = 0;
		}

		long random::_lcg_rand(const long& inseed)
		{
			long new_inseed = (long)(a * (inseed - (long)floor((double)inseed 
				/ (double)q) * q) - r * (long)floor((double)inseed / (double)q));
			if (new_inseed < 0) {
				new_inseed = new_inseed + m;
			}
			return new_inseed;
		}

		void random::uniform_rand(const size_t& N, const long& inseed, std::vector<double>& rand_vec)
		{
			if (rand_vec.size() != 0) {
				std::vector<double>().swap(rand_vec);
			}
			rand_vec.reserve(N);

			long rand_seed[32];
			long seed;
			long rand_value;

			seed = inseed;
			if (seed < 0) {
				seed = -seed;
			}
			if (seed < 1) {
				seed = 1;
			}

			seed = inseed;
			for (int i = 39; i >= 0; --i) {
				seed = _lcg_rand(seed);
				if (i < 32) {
					rand_seed[i] = seed;
				}
			}

			int seed_index = 0;
			seed = rand_seed[0];
			for (int i = 0; i < N; ++i) {
				rand_value = _lcg_rand(seed);

				seed_index = (int)floor((double)seed / (double)67108865);
				seed = rand_seed[seed_index];
				rand_seed[seed_index] = rand_value;

				rand_vec.push_back((double)seed / 2.147483647e9);
				if (rand_vec[i] == 0.) {
					rand_vec[i] = 1e-99;
				}
			}
		}

		std::vector<double> random::gauss(const size_t N, const long inseed)
		{
			std::vector<double> rand_vec;
			std::vector<double> uniform_rand_vec;
			rand_vec.reserve(N);

			const long seed = max(1, abs(inseed));
			uniform_rand(2 * N, seed, uniform_rand_vec);

			for (int i = 0; i < N; i++) {
				rand_vec.push_back(sqrt(-2 * log(uniform_rand_vec[i])) * cos(2 * IOH_PI * uniform_rand_vec[N + i]));
				if (rand_vec[i] == 0.) {
					rand_vec[i] = 1e-99;
				}
			}
			return rand_vec;
		}

		double random::uniform_rand()
		{
			if (this->_seed_index >= long_lag) {
				generate();
			}
			return this->x[this->_seed_index++];
		}

		double random::normal_rand()
		{
			double normal;
			#ifdef NORMAL_POLAR
			const double u1 = this->uniform_rand();
			const double u2 = this->uniform_rand();
			normal = sqrt(-2 * log(u1)) * cos(2 * IOH_PI * u2);
			#else
			normal = 0.0;
			for (int i = 0; i < 12; ++i) {
				normal += this->uniform_rand();
			}
			normal -= 6.0;
			#endif
			return normal;
		}
		
	}
}
