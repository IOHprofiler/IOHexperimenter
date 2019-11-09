/// \file IOHprofiler_random.hpp
/// \brief Head file for class IOHprofiler_transformation.
///
/// A detailed file description. The implementation refer to the work of NumBBO/CoCO team.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _IOHPROFILER_RANDOM_HPP
#define _IOHPROFILER_RANDOM_HPP

#include "IOHprofiler_common.h"
#define IOHprofiler_NORMAL_POLAR /* Use polar transformation method */


#define IOH_PI 3.14159265358979323846
#define DEFAULT_SEED 1000
/* OMS: mutators vs. accessors should be checked throughout; see example in l.137 where const is needed. */
class IOHprofiler_random {
public:
  IOHprofiler_random() { /* OMS: The following two c'tors should be merged using a default uint32 */ 
    uint32_t seed = DEFAULT_SEED;
    for (int i = 0; i < IOHprofiler_LONG_LAG; ++i) {
      x[i] = ((double)seed) / (double)(((uint64_t)1UL << 32) - 1);
      seed = (uint32_t)1812433253UL * (seed ^ (seed >> 30)) + ((uint32_t)i + 1);
    }
    _seed_index = 0;
  }

  IOHprofiler_random(uint32_t seed) {
    for (int i = 0; i < IOHprofiler_LONG_LAG; ++i) {
      x[i] = ((double)seed) / (double)(((uint64_t)1UL << 32) - 1);
      seed = (uint32_t)1812433253UL * (seed ^ (seed >> 30)) + ((uint32_t)i + 1);
    }
    _seed_index = 0;
  }

  void IOHprofiler_random_generate() {
    for (int i = 0; i < IOHprofiler_SHORT_LAG; ++i) {
        double t = this->x[i] + this->x[i + (IOHprofiler_LONG_LAG - IOHprofiler_SHORT_LAG)];
        if (t >= 1.0)
            t -= 1.0;
        this->x[i] = t;
    }
    for (int i = IOHprofiler_SHORT_LAG; i < IOHprofiler_LONG_LAG; ++i) {
        double t = this->x[i] + this->x[i - IOHprofiler_SHORT_LAG];
        if (t >= 1.0)
            t -= 1.0;
        this->x[i] = t;
    }
    this->_seed_index = 0;
}

  long _lcg_rand(const long &inseed) {
    
    long new_inseed =  (long)(a * (inseed - (long)floor((double)inseed / (double)q) * q) - r * (long)floor((double)inseed / (double)q));
    if (new_inseed < 0) {
      new_inseed = new_inseed + m;
    }
    return new_inseed;
  }

  void IOHprofiler_uniform_rand(const size_t &N, const long &inseed, std::vector<double> &rand_vec) {
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
      if(i < 32) {
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

      rand_vec.push_back((double)seed/2.147483647e9);
      if (rand_vec[i] == 0.) {
        rand_vec[i] = 1e-99;
      }
    }
  }

  std::vector<double> IOHprofiler_gauss(const size_t N, const long inseed) {
    std::vector<double> rand_vec;
    std::vector<double> uniform_rand_vec;
    rand_vec.reserve(N);

    long seed;
    long rand_value;

    seed = inseed;
    if (seed < 0) {
      seed = -seed;
    }
    if (seed < 1) {
      seed = 1;
    }

     IOHprofiler_uniform_rand(2 * N, seed,uniform_rand_vec);

    for (int i = 0; i < N; i++) {
      rand_vec.push_back(sqrt(-2 * log(uniform_rand_vec[i])) * cos(2 * IOH_PI * uniform_rand_vec[N + i]));
      if (rand_vec[i] == 0.) {
        rand_vec[i] = 1e-99;
      }
    }
    return rand_vec;
  }

  double IOHprofiler_uniform_rand() {
    if (this->_seed_index >= IOHprofiler_LONG_LAG) {
      IOHprofiler_random_generate();
    }
    return this->x[this->_seed_index++];
  }

  double IOHprofiler_normal_rand() { /* OMS: accessor - should become const */
    // double normal;
    // normal = 0.0;
    // for (int i = 0; i < 12; ++i) {
    //     normal += this->IOHprofiler_uniform_rand();
    // }
    // normal -= 6.0;
    // return normal;
    double normal;
#ifdef IOHprofiler_NORMAL_POLAR
    const double u1 = this->IOHprofiler_uniform_rand();
    const double u2 = this->IOHprofiler_uniform_rand();
    normal = sqrt(-2 * log(u1)) * cos(2 * IOH_PI * u2);
#else
    int i;
    normal = 0.0;
    for (i = 0; i < 12; ++i) {
        normal += this->IOHprofiler_uniform_rand();
    }
    normal -= 6.0;
#endif
    return normal;
  }

private: 
  //long _seed[32];
  size_t _seed_index;

  //double rand_r;
/* OMS: All the following assignments are misplaced - should appear in the c'tors */
  const long a = 16807; /// < multiplier.
  const long m = 2147483647; /// < modulus.
  const long q = 127773; /// < modulusdiv multiplier.
  const long r = 2836; /// < modulus mod multiplier.

  const int IOHprofiler_SHORT_LAG  = 273;
  const int IOHprofiler_LONG_LAG = 607;
  double x[607];
};

#endif //_IOHPROFILER_RANDOM_HPP
