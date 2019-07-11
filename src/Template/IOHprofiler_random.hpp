/// \file IOHprofiler_random.hpp
/// \brief Head file for class IOHprofiler_transformation.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _IOHPROFILER_RANDOM_HPP
#define _IOHPROFILER_RANDOM_HPP

#include "IOHprofiler_common.h"

#define PI 3.14159265359

class IOHprofiler_random {
public:
  IOHprofiler_random() {
    _seed_index = 0;
    for (int i = 0; i < 32; ++i) {
      this->_seed[i] = (long)time(NULL);
    }
  }

  IOHprofiler_random(long seed) {
    _seed_index = 0;
    for (int i = 64; i >= 0; --i) {
      seed = _lcg_rand(seed);
      if(i < 32) this->_seed[i] = seed;
    } 
  }

  long _lcg_rand(long inseed) {
    long a = 16807; /// < multiplier.
    long  m = 2147483647; /// < modulus.
    long q = 127773; /// < modulusdiv multiplier.
    long r = 2836; /// < modulus mod multiplier.

    long tmp = (long)floor((double)inseed / (double)q);
    long new_inseed =  (long)(a * (inseed - tmp * q) - r * tmp);
    if (new_inseed < 0) {
      new_inseed = new_inseed + m;
    }
    return new_inseed;
  }

  std::vector<double> IOHprofiler_uniform_rand(const size_t N, const long inseed) {
    std::vector<double> rand_vec;
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
    for (int i = 64; i >= 0; --i) {
      seed = _lcg_rand(seed);
      if(i < 32) {
        rand_seed[i] = seed;
      }
    }
    
    int seed_index = 0;
    seed = rand_seed[0];
    for (int i = 0; i < N; ++i) {
      rand_value = _lcg_rand(seed);

      rand_seed[seed_index] = rand_value;
      seed_index = (int)floor((double)rand_value / (double)67108865);
      seed = rand_seed[seed_index];


      rand_vec.push_back((double)rand_value/2.147483647e9);
      if (rand_vec[i] == 0.) {
        rand_vec[i] = 1e-99;
      }
    }
    return rand_vec;
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

    uniform_rand_vec = IOHprofiler_uniform_rand(2 * N, seed);

    for (int i = 0; i < N; i++) {
      rand_vec.push_back(sqrt(-2 * log(uniform_rand_vec[i])) * cos(2 * PI * uniform_rand_vec[N + i]));
      if (rand_vec[i] == 0.) {
        rand_vec[i] = 1e-99;
      }
    }
    return rand_vec;
  }

  double IOHprofiler_uniform_rand() {
    double r;
    long _rand = _lcg_rand(this->_seed[_seed_index]);
    
    this->_seed[this->_seed_index] = _rand;
    this->_seed_index = (int)floor((double)_rand / (double)67108865);
    

    r = (double)_rand/2.147483647e9;
    if (r == 0.) {
      r = 1e-99;
    }
    return r;
  }

  double IOHprofiler_normal_rand() {
    double r;
    double u1, u2;
    
    u1 = IOHprofiler_uniform_rand();
    u2 = IOHprofiler_uniform_rand();

    r = sqrt(-2 * log(u1) * cos(2 * PI * u2));
    return r;
  }

private:
  long _seed[32];
  size_t _seed_index;
};

#endif //_IOHPROFILER_RANDOM_HPP
