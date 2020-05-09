/// \file f_Attractive_Sector.hpp
/// \brief cpp file for class f_Attractive_Sector.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_attractive_sector.c"
///
/// \author Furong Ye
/// \date 2019-09-10
#ifndef _F_ATTRACTIVE_SECTOR_HPP
#define _F_ATTRACTIVE_SECTOR_HPP

#include "IOHprofiler_problem.h"
#include "coco_transformation.hpp"

class Attractive_Sector : public IOHprofiler_problem<double> {
public:
  Attractive_Sector(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_id(6);
    IOHprofiler_set_problem_name("Attractive_Sector");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
    IOHprofiler_set_number_of_variables(dimension);
    IOHprofiler_set_as_minimization();
  }
  
  ~Attractive_Sector() {};

  void prepare_problem() {
    std::vector<double> xopt;
    double fopt;
    std::vector<std::vector<double> > M;
    std::vector<double> b;
    /* compute xopt, fopt*/
    
    int n = this->IOHprofiler_get_number_of_variables();
    const long rseed = (long) (6 + 10000 * this->IOHprofiler_get_instance_id());
    bbob2009_compute_xopt(xopt, rseed, n);
    fopt = bbob2009_compute_fopt(6, this->IOHprofiler_get_instance_id());
    
    /* compute M and b */
    M = std::vector<std::vector<double> > (n);
    for (int i = 0; i != n; i++) {
      M[i] = std::vector<double> (n);
    }
    b = std::vector<double> (n);
    std::vector<std::vector<double> > rot1, rot2;
    bbob2009_compute_rotation(rot1, rseed + 1000000, n);
    bbob2009_compute_rotation(rot2, rseed, n);
    for (int i = 0; i < n; ++i) {
      b[i] = 0.0;
      for (int j = 0; j < n; ++j) {
        M[i][j] = 0.0;
        for (int k = 0; k < n; ++k) {
          double exponent = 1.0 * (int) k / ((double) (long) n - 1.0);
          M[i][j] += rot1[i][k] * pow(sqrt(10.0), exponent) * rot2[k][j];
        }
      }
    }
    
    Coco_Transformation_Data::fopt = fopt;
    Coco_Transformation_Data::xopt = xopt;
    Coco_Transformation_Data::M = M;
    Coco_Transformation_Data::b = b;
  }

  double internal_evaluate(const std::vector<double> &x) {
    std::vector<double> result(1,0.0);

    int n = x.size();
    
    for (int i = 0; i < n; ++i) {
      if (Coco_Transformation_Data::xopt[i] * x[i] > 0.0) {
        result[0] += 100.0 * 100.0 * x[i] * x[i];
      } else {
        result[0] += x[i] * x[i];
      }
    }

    return result[0];
  }
  
  static Attractive_Sector * createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
    return new Attractive_Sector(instance_id, dimension);
  }
};

#endif
