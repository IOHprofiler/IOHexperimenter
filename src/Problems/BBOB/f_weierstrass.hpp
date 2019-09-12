/// \file f_weierstrass.hpp
/// \brief cpp file for class Weierstrass.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-09-12
#ifndef _F_WEIERSTRASS_HPP
#define _F_WEIERSTRASS_HPP

#include "../../Template/IOHprofiler_problem.hpp"
#include "bbob_common_used_functions/coco_transformation_vars.hpp"
#include "bbob_common_used_functions/coco_transformation_objs.hpp"

/** @brief Number of summands in the Weierstrass problem. */
#define F_WEIERSTRASS_SUMMANDS 12

class Weierstrass : public IOHprofiler_problem<double> {
public:
  Weierstrass() {
    IOHprofiler_set_problem_name("Weierstrass");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
  }
  Weierstrass(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Weierstrass");
    IOHprofiler_set_problem_type("pseudo_Boolean_problem");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(0);
    IOHprofiler_set_upperbound(1);
    IOHprofiler_set_best_variables(1);
    Initilize_problem(dimension);
  }
  ~Weierstrass() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };

  std::vector<double> xopt;
  double fopt;
  std::vector<std::vector<double>> M;
  std::vector<double> b;
  std::vector<std::vector<double>> M1;
  std::vector<double> b1;
  const double condition = 100.0;
  double f0;
  double ak[F_WEIERSTRASS_SUMMANDS];
  double bk[F_WEIERSTRASS_SUMMANDS];
  void prepare_problem() {
    /* compute xopt, fopt*/
    
    int n = this->IOHprofiler_get_number_of_variables();
    const long rseed = (long) (16 + 10000 * this->IOHprofiler_get_instance_id());
    bbob2009_compute_xopt(xopt, rseed, n);
    fopt = bbob2009_compute_fopt(16, this->IOHprofiler_get_instance_id());
    
    /* compute M and b */
    M = std::vector<std::vector<double>> (n);
    M1 = std::vector<std::vector<double>> (n);
    for (int i = 0; i != n; i++) {
      M[i] = std::vector<double> (n);
      M1[i] = std::vector<double> (n);
    }
    b = std::vector<double> (n);
    b1 = std::vector<double> (n);
    std::vector<std::vector<double>> rot1;
    std::vector<std::vector<double>> rot2;
    bbob2009_compute_rotation(rot1, rseed + 1000000, n);
    bbob2009_compute_rotation(rot2, rseed, n);
    for (int i = 0; i < n; ++i) {
      b[i] = 0.0;
      for (int j = 0; j < n; ++j) {
        M[i][j] = 0.0;
        for (int k = 0; k < n; ++k) {
          const double base = 1.0 / sqrt(condition);
          const double exponent = 1.0 * (int) k / ((double) (long) n - 1.0);
          M[i][j] += rot1[i][k] * pow(base, exponent) * rot2[k][j];
        }
      }
    }
    bbob2009_copy_rotation_matrix(rot1, M1, b1, n);

    f0 = 0.0;
    for (int i = 0; i < F_WEIERSTRASS_SUMMANDS; ++i) {
      ak[i] = pow(0.5, (double) i);
      bk[i] = pow(3., (double) i);
      f0 += ak[i] * cos(2 * coco_pi * bk[i] * 0.5);
    }
  }

  double internal_evaluate(const std::vector<double> &x) {
    std::vector<double> temp_x = x;
    int n = temp_x.size();
    size_t i, j;

    const double penalty_factor = 10.0 / (double) n;
    std::vector<double> result(1);

    transform_vars_affine_evaluate_function(temp_x,M,b);
    transform_vars_oscillate_evaluate_function(temp_x);
    transform_vars_affine_evaluate_function(temp_x,M1,b1);
    transform_vars_shift_evaluate_function(temp_x,xopt);

    result[0] = 0.0;
    for (i = 0; i < n; ++i) {
      for (j = 0; j < F_WEIERSTRASS_SUMMANDS; ++j) {
        result[0] += cos(2 * coco_pi * (temp_x[i] + 0.5) * bk[j]) * ak[j];
      }
    }
    result[0] = 10.0 * pow(result[0] / (double) (long) n - f0, 3.0);

    transform_obj_shift_evaluate_function(result,fopt);
    transform_obj_penalize_evaluate(temp_x,this->IOHprofiler_get_lowerbound(),this->IOHprofiler_get_upperbound(),penalty_factor,result);

    return result[0];
  };
  
  static Weierstrass * createInstance() {
    return new Weierstrass();
  };

  static Weierstrass * createInstance(int instance_id, int dimension) {
    return new Weierstrass(instance_id, dimension);
  };
};

#endif