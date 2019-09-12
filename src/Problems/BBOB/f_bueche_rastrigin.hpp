/// \file f_Bueche_Rastrigin.hpp
/// \brief cpp file for class f_Bueche_Rastrigin.
///
/// A detailed file description.
///
/// \author Naama Horesh and Furong Ye
/// \date 2019-06-27
#ifndef _F_BUECHE_RASTRIGIN_H
#define _F_BUECHE_RASTRIGIN_H

#include "../../Template/IOHprofiler_problem.hpp"
#include "bbob_common_used_functions/coco_transformation_vars.hpp"
#include "bbob_common_used_functions/coco_transformation_objs.hpp"

class Bueche_Rastrigin : public IOHprofiler_problem<double> {
public:
  Bueche_Rastrigin() {
    IOHprofiler_set_problem_name("Bueche_Rastrigin");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(1);
  }
  Bueche_Rastrigin(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Bueche_Rastrigin");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(1);
    Initilize_problem(dimension);
  }
  ~Bueche_Rastrigin() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };

  std::vector<double> xopt;
  double fopt;
  const double penalty_factor = 100.0;
  void prepare_problem() {
    /* compute xopt, fopt*/
    
    int n = this->IOHprofiler_get_number_of_variables();
    const long rseed = (long) (3 + 10000 * this->IOHprofiler_get_instance_id());
    bbob2009_compute_xopt(xopt, rseed, n);
    fopt = bbob2009_compute_fopt(4, this->IOHprofiler_get_instance_id());

    for (int i = 0; i < n; i += 2) {
      xopt[i] = fabs(xopt[i]);
    }
  }

  double internal_evaluate(const std::vector<double> &x) {
    double tmp = 0., tmp2 = 0.;
    std::vector<double> result(1);
    std::vector<double> temp_x = x;
    int n = temp_x.size();

    transform_vars_brs_evaluate(temp_x);
    transform_vars_oscillate_evaluate_function(temp_x);
    transform_vars_shift_evaluate_function(temp_x,xopt);

    result[0] = 0.0;
    for (int i = 0; i < n; ++i) {
      tmp += cos(2 * coco_pi * temp_x[i]);
      tmp2 += temp_x[i] * temp_x[i];
    }
    result[0] = 10.0 * ((double) (long) n - tmp) + tmp2 + 0;

    /* ignore large-scale test */
    transform_obj_shift_evaluate_function(result,fopt);
    transform_obj_penalize_evaluate(temp_x,this->IOHprofiler_get_lowerbound(),this->IOHprofiler_get_upperbound(),penalty_factor,result);
    return result[0];
  };
  
  static Bueche_Rastrigin * createInstance() {
    return new Bueche_Rastrigin();
  };

  static Bueche_Rastrigin * createInstance(int instance_id, int dimension) {
    return new Bueche_Rastrigin(instance_id, dimension);
  };
};

#endif