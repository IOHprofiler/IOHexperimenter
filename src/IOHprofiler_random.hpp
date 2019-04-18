#ifndef _IOHPROFILER_RANDOM_HPP
#define _IOHPROFILER_RANDOM_HPP

#include "common.h"


class Random_methods{
public:
  void IOHprofiler_unif(std::vector<double> &r, const std::size_t N,  long inseed) {
      /* generates N uniform numbers with starting seed */
      r.reserve(N);
      long aktseed;
      long tmp;
      long rgrand[32];
      long aktrand;
      long i;

      if (inseed < 0)
          inseed = -inseed;
      if (inseed < 1)
          inseed = 1;
      aktseed = inseed;
      for (i = 39; i >= 0; i--) {
          tmp = (int)floor((double)aktseed / (double)127773);
          aktseed = 16807 * (aktseed - tmp * 127773) - 2836 * tmp;
          if (aktseed < 0)
              aktseed = aktseed + 2147483647;
          if (i < 32)
              rgrand[i] = aktseed;
      }
      aktrand = rgrand[0];
      for (i = 0; i < N; i++) {
          tmp = (int)floor((double)aktseed / (double)127773);
          aktseed = 16807 * (aktseed - tmp * 127773) - 2836 * tmp;
          if (aktseed < 0)
              aktseed = aktseed + 2147483647;
          tmp = (int)floor((double)aktrand / (double)67108865);
          aktrand = rgrand[tmp];
          rgrand[tmp] = aktseed;
          r.push_back((double)aktrand / 2.147483647e9);
          if (r[i] == 0.) {
              r[i] = 1e-99;
          }
      }
      return;
  }

   void IOHprofiler_gauss(std::vector<double> &g, const std::size_t N, const long seed) {
      g.reserve(N);
      std::size_t i;

      std::vector<double> uniftmp;
      IOHprofiler_unif(uniftmp, 2 * N, seed);

      for (i = 0; i < N; i++) {
          g.push_back(sqrt(-2 * log(uniftmp[i])) * cos(2 * 3.1415926 * uniftmp[N + i]));
          if (g[i] == 0.)
              g[i] = 1e-99;
      }
      return;
  }
};

#endif
