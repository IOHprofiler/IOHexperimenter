/// \file f_rastrigin.hpp
/// \brief cpp file for class Rastrigin.
/// 
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_rastrigin.c"
///
/// \author Furong Ye
/// \date 2019-09-12
#ifndef _F_RASTRIGIN_HPP
#define _F_RASTRIGIN_HPP

#include "../../Template/IOHprofiler_problem.hpp"
#include "bbob_common_used_functions/coco_transformation_vars.hpp"
#include "bbob_common_used_functions/coco_transformation_objs.hpp"

class Rastrigin : public IOHprofiler_problem<double> {
public:
  Rastrigin() {
    IOHprofiler_set_problem_name("Rastrigin");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
  }
  Rastrigin(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Rastrigin");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
    Initilize_problem(dimension);
  }
  ~Rastrigin() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };

  std::vector<double> xopt;
  double fopt;
  void prepare_problem() {
    /* compute xopt, fopt*/
    
    int n = this->IOHprofiler_get_number_of_variables();
    const long rseed = (long) (3 + 10000 * this->IOHprofiler_get_instance_id());
    bbob2009_compute_xopt(xopt, rseed, n);
    fopt = bbob2009_compute_fopt(3, this->IOHprofiler_get_instance_id());
  }

  double internal_evaluate(const std::vector<double> &x) {
    size_t i = 0;
    std::vector<double> result(1);
    std::vector<double> temp_x = x;
    int n = temp_x.size();
    double sum1 = 0.0, sum2 = 0.0;
      
    transform_vars_conditioning_evaluate(temp_x,10.0);
    transform_vars_asymmetric_evaluate_function(temp_x,0.2);
    transform_vars_oscillate_evaluate_function(temp_x);
    transform_vars_shift_evaluate_function(temp_x,xopt);
    for (i = 0; i < n; ++i) {
      sum1 += cos(2.0 * coco_pi * temp_x[i]);
      sum2 += temp_x[i] * temp_x[i];
    }
    /* double check isinf*/
    if (isinf(sum2)) {
      return sum2;
    }
    result[0] = 10.0 * ((double) (long) n - sum1) + sum2;
    transform_obj_shift_evaluate_function(result,fopt);
    return result[0];
  };
  
  static Rastrigin * createInstance() {
    return new Rastrigin();
  };

  static Rastrigin * createInstance(int instance_id, int dimension) {
    return new Rastrigin(instance_id, dimension);
  };
};

#endif