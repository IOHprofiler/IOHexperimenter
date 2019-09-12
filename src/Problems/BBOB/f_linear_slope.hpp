/// \file f_linear_slope.hpp
/// \brief cpp file for class Linear_Slope.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_linear_slope.c"
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _F_LINEAR_SLOPE_H
#define _F_LINEAR_SLOPE_H

#include "../../Template/IOHprofiler_problem.hpp"
#include "bbob_common_used_functions/coco_transformation_vars.hpp"
#include "bbob_common_used_functions/coco_transformation_objs.hpp"

class Linear_Slope : public IOHprofiler_problem<double> {
public:
  Linear_Slope() {
    IOHprofiler_set_problem_name("Linear_Slope");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
  }
  Linear_Slope(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Linear_Slope");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
    Initilize_problem(dimension);
  }
  ~Linear_Slope() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };

  std::vector<double> xopt;
  double fopt;
  void prepare_problem() {
    /* compute xopt, fopt*/
    
    int n = this->IOHprofiler_get_number_of_variables();
    const long rseed = (long) (5 + 10000 * this->IOHprofiler_get_instance_id());
    bbob2009_compute_xopt(xopt, rseed, n);
    fopt = bbob2009_compute_fopt(5, this->IOHprofiler_get_instance_id());
  }
  
  double internal_evaluate(const std::vector<double> &x) {
    int n = x.size();
    static const double alpha = 100.0;
    size_t i;
    std::vector<double> result(1);
    
    for (i = 0; i < n; ++i) {
      double base, exponent, si;

      base = sqrt(alpha);
      exponent = (double) (long) i / ((double) (long) n - 1);
      if (xopt[i] > 0.0) {
        si = pow(base, exponent);
      } else {
        si = -pow(base, exponent);
      }
      /* boundary handling */
      if (x[i] * xopt[i] < 25.0) {
        result[0] += 5.0 * fabs(si) - si * x[i];
      } else {
        result[0] += 5.0 * fabs(si) - si * xopt[i];
      }
    }
    transform_obj_shift_evaluate_function(result,fopt);
    return result[0];
  };
  
  static Linear_Slope * createInstance() {
    return new Linear_Slope();
  };

  static Linear_Slope * createInstance(int instance_id, int dimension) {
    return new Linear_Slope(instance_id, dimension);
  };
};

#endif