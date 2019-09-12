/// \file f_rosenbrock_rotated.hpp
/// \brief cpp file for class Rosenbrock_Rotated.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_rosenbrock.c"
///
/// \author Furong Ye
/// \date 2019-09-12
#ifndef _F_ROSENBROCK_ROTATED_HPP
#define _F_ROSENBROCK_ROTATED_HPP

#include "../../Template/IOHprofiler_problem.hpp"
#include "bbob_common_used_functions/coco_transformation_vars.hpp"
#include "bbob_common_used_functions/coco_transformation_objs.hpp"

class Rosenbrock_Rotated : public IOHprofiler_problem<double> {
public:
  Rosenbrock_Rotated() {
    IOHprofiler_set_problem_name("Rosenbrock_Rotated");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
  }
  Rosenbrock_Rotated(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Rosenbrock_Rotated");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
    Initilize_problem(dimension);
  }
  ~Rosenbrock_Rotated() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };


  double fopt, factor;
  std::vector<std::vector<double>> M;
  std::vector<double> b;
  void prepare_problem() {
    /* compute xopt, fopt*/
    
    int n = this->IOHprofiler_get_number_of_variables();
    const long rseed = (long) (9 + 10000 * this->IOHprofiler_get_instance_id());
    fopt = bbob2009_compute_fopt(9, this->IOHprofiler_get_instance_id());

    /* compute M and b */
    M = std::vector<std::vector<double>> (n);
    for (int i = 0; i != n; i++) {
      M[i] = std::vector<double> (n);
    }
    b = std::vector<double> (n);
    std::vector<std::vector<double>> rot1;
    bbob2009_compute_rotation(rot1, rseed, n);
    for (int row = 0; row < n; ++row) {
      for (int column = 0; column < n; ++column) {
        M[row][column] = factor * rot1[row][column];
      }
      b[row] = 0.5;
    }

    factor = 1.0 > (sqrt((double) n) / 8.0) ? 1 : (sqrt((double) n) / 8.0);
  }

  double internal_evaluate(const std::vector<double> &x) {
    std::vector<double> temp_x = x;
    int n = temp_x.size();
    size_t i = 0;
    std::vector<double> result(1);
    double s1 = 0.0, s2 = 0.0, tmp;

    transform_vars_affine_evaluate_function(temp_x,M,b);

    for (i = 0; i < n - 1; ++i) {
      tmp = (temp_x[i] * temp_x[i] - temp_x[i + 1]);
      s1 += tmp * tmp;
      tmp = (temp_x[i] - 1.0);
      s2 += tmp * tmp;
    }
    result[0] = 100.0 * s1 + s2;

    transform_obj_shift_evaluate_function(result,fopt);

    return result[0];
  };
  
  static Rosenbrock_Rotated * createInstance() {
    return new Rosenbrock_Rotated();
  };

  static Rosenbrock_Rotated * createInstance(int instance_id, int dimension) {
    return new Rosenbrock_Rotated(instance_id, dimension);
  };
};

#endif