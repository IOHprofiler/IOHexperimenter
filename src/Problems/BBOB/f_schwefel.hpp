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

#include "../../Template/IOHprofiler_problem.hpp"
#include "bbob_common_used_functions/coco_transformation_vars.hpp"
#include "bbob_common_used_functions/coco_transformation_objs.hpp"

class Schwefel : public IOHprofiler_problem<double> {
public:
  Schwefel() {
    IOHprofiler_set_problem_name("Schwefel");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
  }
  Schwefel(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Schwefel");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
    Initilize_problem(dimension);
  }
  ~Schwefel() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };

  std::vector<double> xopt,tmp1,tmp2;
  double fopt;
  const double condition = 10.;
  void prepare_problem() {
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
    
  }

  double internal_evaluate(const std::vector<double> &x) {
    std::vector<double> temp_x = x;
    int n = temp_x.size();
    size_t i = 0;
    std::vector<double> result(1);
    double penalty, sum;
    const long rseed = (long) (20 + 10000 * this->IOHprofiler_get_instance_id());

    transform_vars_scale_evaluate(temp_x,100);
    transform_vars_shift_evaluate_function(temp_x,tmp1);
    transform_vars_conditioning_evaluate(temp_x,condition);
    transform_vars_shift_evaluate_function(temp_x,tmp2);
    transform_vars_z_hat_evaluate(temp_x,xopt);
    transform_vars_scale_evaluate(temp_x,2);
    transform_vars_x_hat_evaluate(temp_x,rseed);
    /* Boundary handling*/
    penalty = 0.0;
    for (i = 0; i < n; ++i) {
      const double tmp = fabs(temp_x[i]) - 500.0;
      if (tmp > 0.0)
        penalty += tmp * tmp;
    }

    /* Computation core */
    sum = 0.0;
    for (i = 0; i < n; ++i) {
      sum += temp_x[i] * sin(sqrt(fabs(temp_x[i])));
    }
    result[0] = 0.01 * (penalty + 418.9828872724339 - sum / (double) n);

    transform_obj_shift_evaluate_function(result,fopt);

    return result[0];
  };
  
  static Schwefel * createInstance() {
    return new Schwefel();
  };

  static Schwefel * createInstance(int instance_id, int dimension) {
    return new Schwefel(instance_id, dimension);
  };
};

#endif