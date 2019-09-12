/// \file f_Discus.hpp
/// \brief cpp file for class f_Discus.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_discus.c"
///
/// \author Furong Ye
/// \date 2019-09-10
#ifndef _F_DISCUS_H
#define _F_DISCUS_H

#include "../../Template/IOHprofiler_problem.hpp"
#include "bbob_common_used_functions/coco_transformation_vars.hpp"
#include "bbob_common_used_functions/coco_transformation_objs.hpp"

class Discus : public IOHprofiler_problem<double> {
public:
  Discus() {
    IOHprofiler_set_problem_name("Discus");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0.0);
  }
  Discus(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Discus");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0.0);
    Initilize_problem(dimension);
  }
  ~Discus() {};

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
    const long rseed = (long) (11 + 10000 * this->IOHprofiler_get_instance_id());
    bbob2009_compute_xopt(xopt, rseed, n);
    fopt = bbob2009_compute_fopt(11, this->IOHprofiler_get_instance_id());
    
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
    std::vector<double> temp_x = x;
    static const double condition = 1.0e6;
    std::vector<double> result(1);
    int n = temp_x.size();

    transform_vars_oscillate_evaluate_function(temp_x);
    transform_vars_affine_evaluate_function(temp_x,M,b);
    transform_vars_shift_evaluate_function(temp_x,xopt);
    result[0] = condition * temp_x[0] * temp_x[0];
    for (int i = 1; i < n; ++i) {
      result[0] += temp_x[i] * temp_x[i];
    }

    transform_obj_shift_evaluate_function(result,fopt);

    return result[0];
  };
  
  static Discus * createInstance() {
    return new Discus();
  };

  static Discus * createInstance(int instance_id, int dimension) {
    return new Discus(instance_id, dimension);
  };
};

#endif