/// \file f_schwefel.hpp
/// \brief cpp file for class Schwefel.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_schwefel.c"
///
/// \author Furong Ye
/// \date 2019-09-12
#ifndef _F_SCHWEFEL_HPP
#define _F_SCHWEFEL_HPP

#include "IOHprofiler_problem.h"
#include "coco_transformation.hpp"

class Schwefel : public IOHprofiler_problem<double> {
public:
  Schwefel(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_id(20);
    IOHprofiler_set_problem_name("Schwefel");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(420.96874633);
    IOHprofiler_set_number_of_variables(dimension);
    IOHprofiler_set_as_minimization();
  }

  ~Schwefel() {}
  
  void prepare_problem() {
    std::vector<double> xopt,tmp1,tmp2;
    double fopt;
    /* compute xopt, fopt*/
    
    int n = this->IOHprofiler_get_number_of_variables();
    const long rseed = (long) (20 + 10000 * this->IOHprofiler_get_instance_id());
    fopt = bbob2009_compute_fopt(20, this->IOHprofiler_get_instance_id());
    xopt = std::vector<double>(n);
    tmp2 = std::vector<double>(n);
    bbob2009_unif(tmp1, n, rseed);
    for (int i = 0; i < n; ++i) {
      xopt[i] = (tmp1[i] < 0.5 ? -1 : 1) * 0.5 * 4.2096874637;
    }
    for (int i = 0; i < n; ++i) {
      tmp1[i] = -2 * fabs(xopt[i]);
      tmp2[i] = 2 * fabs(xopt[i]);
    }
    
    Coco_Transformation_Data::xopt = xopt;
    Coco_Transformation_Data::fopt = fopt;
    Coco_Transformation_Data::tmp1 = tmp1;
    Coco_Transformation_Data::tmp2 = tmp2;
    Coco_Transformation_Data::condition = 10.0;
    Coco_Transformation_Data::rseed = rseed;
  }

  double internal_evaluate(const std::vector<double> &x) {
    int n = x.size();
    size_t i = 0;
    std::vector<double> result(1);
    double penalty, sum;


    /* Boundary handling*/
    penalty = 0.0;
    for (i = 0; i < n; ++i) {
      const double tmp = fabs(x[i]) - 500.0;
      if (tmp > 0.0) {
        penalty += tmp * tmp;
      }
    }

    /* Computation core */
    sum = 0.0;
    for (i = 0; i < n; ++i) {
      sum += x[i] * sin(sqrt(fabs(x[i])));
    }
    result[0] = 0.01 * (penalty + 418.9828872724339 - sum / (double) n);

    return result[0];
  }

  static Schwefel * createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
    return new Schwefel(instance_id, dimension);
  }
};

#endif
