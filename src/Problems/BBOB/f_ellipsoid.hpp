/// \file f_ellipsoid.hpp
/// \brief cpp file for class f_ellipsoid.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_ellipsoid.c"
///
/// \author Furong Ye
/// \date 2019-09-12
#ifndef _F_ELLIPSOID_H
#define _F_ELLIPSOID_H

#include "../../Template/IOHprofiler_problem.hpp"
#include "bbob_common_used_functions/coco_transformation_vars.hpp"
#include "bbob_common_used_functions/coco_transformation_objs.hpp"

class Ellipsoid : public IOHprofiler_problem<double> {
public:
  Ellipsoid() {
    IOHprofiler_set_problem_name("Ellipsoid");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
  }
  Ellipsoid(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Ellipsoid");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
    Initilize_problem(dimension);
  }
  ~Ellipsoid() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };

  std::vector<double> xopt;
  double fopt;
  void prepare_problem() {
    /* compute xopt, fopt*/
    
    int n = this->IOHprofiler_get_number_of_variables();
    const long rseed = (long) (2 + 10000 * this->IOHprofiler_get_instance_id());
    bbob2009_compute_xopt(xopt, rseed, n);
    fopt = bbob2009_compute_fopt(2, this->IOHprofiler_get_instance_id());
  }


  double internal_evaluate(const std::vector<double> &x) {
    static const double condition = 1.0e6;
    size_t i = 0;
    std::vector<double> temp_x = x;
    int n = temp_x.size();
    std::vector<double> result(1);

    transform_vars_oscillate_evaluate_function(temp_x);
    transform_vars_shift_evaluate_function(temp_x,xopt);

    result[0] = temp_x[i] * temp_x[i];
    for (i = 1; i < n; ++i) {
      const double exponent = 1.0 * (double) (long) i / ((double) (long) n - 1.0);
      result[0] += pow(condition, exponent) * temp_x[i] * temp_x[i];
    }

    transform_obj_shift_evaluate_function(result,fopt);

    return result[0];
  };
  
  static Ellipsoid * createInstance() {
    return new Ellipsoid();
  };

  static Ellipsoid * createInstance(int instance_id, int dimension) {
    return new Ellipsoid(instance_id, dimension);
  };
};

#endif