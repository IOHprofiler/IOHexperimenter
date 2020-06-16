/// \file IOHprofiler_random.hpp
/// \brief Header file for the class IOHprofiler_random.
/// The implementation refer to the work of NumBBO/CoCO team.

#ifndef _IOHPROFILER_RANDOM_H
#define _IOHPROFILER_RANDOM_H

#include "IOHprofiler_common.h"
#define IOHprofiler_NORMAL_POLAR /* Use polar transformation method */

#define IOH_PI 3.14159265358979323846
#define DEFAULT_SEED 1000


#define RND_MULTIPLIER 16807
#define RND_MODULUS 2147483647
#define RND_MODULUS_DIV 127773
#define RND_MOD_MULTIPLIER 2836

#define IOHPROFILER_SHORT_LAG  273
#define IOHPROFILER_LONG_LAG 607

class IOHprofiler_random {
public:
  IOHprofiler_random(uint32_t seed = DEFAULT_SEED);

  void IOHprofiler_random_generate();

  static long _lcg_rand(const long &inseed);

  static void IOHprofiler_uniform_rand(const size_t &N, const long &inseed, std::vector<double> &rand_vec);
  
  static std::vector<double> IOHprofiler_gauss(const size_t N, const long inseed);
  
  double IOHprofiler_uniform_rand();
  
  double IOHprofiler_normal_rand();

private:
  size_t _seed_index;

  static long a; /// < multiplier.
  static long m; /// < modulus.
  static long q; /// < modulusdiv multiplier.
  static long r; /// < modulus mod multiplier.

  static int IOHprofiler_SHORT_LAG;
  static int IOHprofiler_LONG_LAG;
  double x[607];
};

#endif // _IOHPROFILER_RANDOM_H
