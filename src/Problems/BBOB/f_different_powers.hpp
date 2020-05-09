/// \file f_Different_Powers.hpp
/// \brief cpp file for class f_Different_Powers.
///
/// A detailed file description.
///
/// \author Naama Horesh and Furong Ye
/// \date 2019-06-27
#ifndef _F_DIFFERENT_POWERS_HPP
#define _F_DIFFERENT_POWERS_HPP

#include "IOHprofiler_problem.h"
#include "coco_transformation.hpp"

class Different_Powers : public IOHprofiler_problem<double> {
public:
  Different_Powers(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_id(14);
    IOHprofiler_set_problem_name("Different_Powers");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
    IOHprofiler_set_number_of_variables(dimension);
    IOHprofiler_set_as_minimization();
  }
  
  ~Different_Powers() {}

  void prepare_problem() {
    std::vector<double> xopt;
    double fopt;
    std::vector<std::vector<double> > M;
    std::vector<double> b;
    /* compute xopt, fopt*/
    
    int n = this->IOHprofiler_get_number_of_variables();
    const long rseed = (long) (14 + 10000 * this->IOHprofiler_get_instance_id());
    bbob2009_compute_xopt(xopt, rseed, n);
    fopt = bbob2009_compute_fopt(14, this->IOHprofiler_get_instance_id());
    
    /* compute M and b */
    M = std::vector<std::vector<double> > (n);
    for (int i = 0; i != n; i++) {
      M[i] = std::vector<double> (n);
    }
    b = std::vector<double> (n);
    std::vector<std::vector<double> > rot1;
    bbob2009_compute_rotation(rot1, rseed + 1000000, n);
    bbob2009_copy_rotation_matrix(rot1,M,b,n);
    
    Coco_Transformation_Data::fopt = fopt;
    Coco_Transformation_Data::xopt = xopt;
    Coco_Transformation_Data::M = M;
    Coco_Transformation_Data::b = b;
  }

  double internal_evaluate(const std::vector<double> &x) {
    size_t i;
    double sum = 0.0;
    int n = x.size();
    std::vector<double> result(1);

   for (i = 0; i < n; ++i) {
      double exponent = 2.0 + (4.0 * (double) (long) i) / ((double) (long) n - 1.0);
      sum += pow(fabs(x[i]), exponent);
    }
    result[0] = sqrt(sum);

    return result[0];
  }

  static Different_Powers * createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
    return new Different_Powers(instance_id, dimension);
  }
};

#endif
