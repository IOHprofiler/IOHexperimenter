/// \file f_bent_cigar.hpp
/// \brief cpp file for class f_Bent_Cigar.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_bent_cigar.c"
///
/// \author Furong Ye
/// \date 2019-09-10
#ifndef _F_BENT_CIGAR_H
#define _F_BENT_CIGAR_H

#include "IOHprofiler_problem.h"
#include "coco_transformation.hpp"

class Bent_Cigar : public IOHprofiler_problem<double> {
public:
  Bent_Cigar(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_id(12);   
    IOHprofiler_set_problem_name("Bent_Cigar");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
    IOHprofiler_set_number_of_variables(dimension);
    IOHprofiler_set_as_minimization();
  }
  
  ~Bent_Cigar() {};

  void prepare_problem() {
    std::vector<double> xopt;
    double fopt;
    std::vector<std::vector<double> > M;
    std::vector<double> b;
    /* compute xopt, fopt*/
    
    int n = this->IOHprofiler_get_number_of_variables();
    const long rseed = (long) (12 + 10000 * this->IOHprofiler_get_instance_id());
    bbob2009_compute_xopt(xopt, rseed + 1000000, n);
    fopt = bbob2009_compute_fopt(12, this->IOHprofiler_get_instance_id());
    
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
    
    static const double condition = 1.0e6;
    size_t i;
    int n = x.size();
    std::vector<double> result(1);


    result[0] = x[0] * x[0];
    for (i = 1; i < n; ++i) {
      result[0] += condition * x[i] * x[i];
    }

    return result[0];
  }
  
  static Bent_Cigar * createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) {
    return new Bent_Cigar(instance_id, dimension);
  }
};

#endif
