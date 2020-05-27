/// \file f_ellipsoid_rotated.hpp
/// \brief cpp file for class Ellipsoid_Rotated.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_ellipsoid.c"
///
/// \author Furong Ye
/// \date 2019-09-12
#ifndef _F_ELLIPSOID_ROTATED_H
#define _F_ELLIPSOID_ROTATED_H

#include "IOHprofiler_problem.h"
#include "coco_transformation.hpp"

class Ellipsoid_Rotated : public IOHprofiler_problem<double> {
public:
  Ellipsoid_Rotated(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_id(10);
    IOHprofiler_set_problem_name("Ellipsoid_Rotated");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
    IOHprofiler_set_number_of_variables(dimension);
    IOHprofiler_set_as_minimization();
  }

  ~Ellipsoid_Rotated() {}

  void prepare_problem() {
    /* compute xopt, fopt*/
    std::vector<double> xopt;
    double fopt;
    std::vector<std::vector<double> > M;
    std::vector<double> b;

    int n = this->IOHprofiler_get_number_of_variables();
    const long rseed = (long) (10 + 10000 * this->IOHprofiler_get_instance_id());
    bbob2009_compute_xopt(xopt, rseed, n);
    fopt = bbob2009_compute_fopt(10, this->IOHprofiler_get_instance_id());
    
    /* compute M and b */
    M = std::vector<std::vector<double> > (n);
    for (int i = 0; i != n; i++) {
      M[i] = std::vector<double> (n);
    }
    b = std::vector<double> (n);
    std::vector<std::vector<double> > rot1;
    bbob2009_compute_rotation(rot1, rseed + 1000000, n);
    bbob2009_copy_rotation_matrix(rot1, M, b, n);

    Coco_Transformation_Data::fopt = fopt;
    Coco_Transformation_Data::xopt = xopt;
    Coco_Transformation_Data::M = M;
    Coco_Transformation_Data::b = b;
  }


  double internal_evaluate(const std::vector<double> &x) {
    static const double condition = 1.0e6;
    size_t i = 0;
    int n = x.size();
    std::vector<double> result(1);


    result[0] = x[i] * x[i];
    for (i = 1; i < n; ++i) {
      const double exponent = 1.0 * (double) (long) i / ((double) (long) n - 1.0);
      result[0] += pow(condition, exponent) * x[i] * x[i];
    }

    return result[0];
  }
  
  static Ellipsoid_Rotated * createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
    return new Ellipsoid_Rotated(instance_id, dimension);
  }
};

#endif
