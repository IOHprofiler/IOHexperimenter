/// \file f_Bent_Ciger.hpp
/// \brief cpp file for class f_Bent_Ciger.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_bent_cigar.c"
///
/// \author Furong Ye
/// \date 2019-09-10
#ifndef _F_BENT_CIGAR_H
#define _F_BENT_CIGAR_H

#include "../../Template/IOHprofiler_problem.hpp"
#include "bbob_common_used_functions/coco_transformation_vars.hpp"
#include "bbob_common_used_functions/coco_transformation_objs.hpp"

class Bent_Ciger : public IOHprofiler_problem<double> {
public:
  Bent_Ciger() {
    IOHprofiler_set_problem_name("Bent_Ciger");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
  }
  Bent_Ciger(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Bent_Ciger");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
    Initilize_problem(dimension);
  }
  ~Bent_Ciger() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };

  std::vector<double> xopt;
  double fopt;
  std::vector<std::vector<double>> M;
  std::vector<double> b;
  void prepare_problem() {
    /* compute xopt, fopt*/
    
    int n = this->IOHprofiler_get_number_of_variables();
    const long rseed = (long) (12 + 10000 * this->IOHprofiler_get_instance_id());
    bbob2009_compute_xopt(xopt, rseed, n);
    fopt = bbob2009_compute_fopt(12, this->IOHprofiler_get_instance_id());
    
    /* compute M and b */
    M = std::vector<std::vector<double>> (n);
    for (int i = 0; i != n; i++) {
      M[i] = std::vector<double> (n);
    }
    b = std::vector<double> (n);
    std::vector<std::vector<double>> rot1;
    bbob2009_compute_rotation(rot1, rseed + 1000000, n);
    bbob2009_copy_rotation_matrix(rot1,M,b,n);
    
  }

  double internal_evaluate(const std::vector<double> &x) {
    
    static const double condition = 1.0e6;
    size_t i;
    std::vector<double> temp_x;
    int n = temp_x.size();
    std::vector<double> result(1);

    transform_vars_affine_evaluate_function(temp_x,M,b);
    transform_vars_asymmetric_evaluate_function(temp_x,0.5);
    transform_vars_affine_evaluate_function(temp_x,M,b);
    transform_vars_shift_evaluate_function(temp_x,xopt);


    result[0] = temp_x[0] * temp_x[0];
    for (i = 1; i < n; ++i) {
      result[0] += condition * temp_x[i] * temp_x[i];
    }

    transform_obj_shift_evaluate_function(result,fopt);
    return result[0];
  };
  
  static Bent_Ciger * createInstance() {
    return new Bent_Ciger();
  };

  static Bent_Ciger * createInstance(int instance_id, int dimension) {
    return new Bent_Ciger(instance_id, dimension);
  };
};

#endif