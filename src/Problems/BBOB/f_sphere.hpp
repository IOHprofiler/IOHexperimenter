/// \file f_sphere.hpp
/// \brief cpp file for class Sphere.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_sphere.c"
///
/// \author Furong Ye
/// \date 2019-09-12
#ifndef _F_SPHERE_HPP_
#define _F_SPHERE_HPP_

#include "../../Template/IOHprofiler_problem.hpp"
#include "bbob_common_used_functions/coco_transformation_vars.hpp"
#include "bbob_common_used_functions/coco_transformation_objs.hpp"

class Sphere : public IOHprofiler_problem<double> {
public:
  Sphere() {
    IOHprofiler_set_problem_name("Sphere");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
  }
  Sphere(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Sphere");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
    Initilize_problem(dimension);
  }
  ~Sphere() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };

  std::vector<double> xopt;
  double fopt;
  void prepare_problem() {
    /* compute xopt, fopt*/
    
    int n = this->IOHprofiler_get_number_of_variables();
    const long rseed = (long) (13 + 10000 * this->IOHprofiler_get_instance_id());
    bbob2009_compute_xopt(xopt, rseed, n);
    fopt = bbob2009_compute_fopt(13, this->IOHprofiler_get_instance_id());
  }

  double internal_evaluate(const std::vector<double> &x) {
    std::vector<double> temp_x = x;
    int n = temp_x.size();
    size_t i = 0;
    std::vector<double> result(1);
      
    transform_vars_shift_evaluate_function(temp_x,xopt);

    result[0] = 0.0;
    for (i = 0; i < n; ++i) {
      result[0] += temp_x[i] * temp_x[i];
    }

    transform_obj_shift_evaluate_function(result,fopt);
    
    return result[0];
  };
  
  static Sphere * createInstance() {
    return new Sphere();
  };

  static Sphere * createInstance(int instance_id, int dimension) {
    return new Sphere(instance_id, dimension);
  };
};

#endif