/// \file f_griewank_rosenBrock.hpp
/// \brief hpp file for class Griewank_RosenBrock.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_griewank_rosenbrock.c"
///
/// \author Furong Ye
/// \date 2019-09-12
#ifndef _F_GRIEWANK_ROSENBROCK_H
#define _F_GRIEWANK_ROSENBROCK_H

#include "../../Template/IOHprofiler_problem.hpp"
#include "bbob_common_used_functions/coco_transformation_vars.hpp"
#include "bbob_common_used_functions/coco_transformation_objs.hpp"

class Griewank_RosenBrock : public IOHprofiler_problem<double> {
public:
  Griewank_RosenBrock() {
    IOHprofiler_set_problem_name("Griewank_RosenBrock");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
  }
  Griewank_RosenBrock(int instance_id, int dimension) {
    IOHprofiler_set_instance_id(instance_id);
    IOHprofiler_set_problem_name("Griewank_RosenBrock");
    IOHprofiler_set_problem_type("bbob");
    IOHprofiler_set_number_of_objectives(1);
    IOHprofiler_set_lowerbound(-5.0);
    IOHprofiler_set_upperbound(5.0);
    IOHprofiler_set_best_variables(0);
    Initilize_problem(dimension);
  }
  ~Griewank_RosenBrock() {};

  void Initilize_problem(int dimension) {
    IOHprofiler_set_number_of_variables(dimension);
  };

  std::vector<double> shift;
  double fopt,scales;
  std::vector<std::vector<double>> M;
  std::vector<double> b;
  void prepare_problem() {
    /* compute xopt, fopt*/
    
    int n = this->IOHprofiler_get_number_of_variables();
    const long rseed = (long) (19 + 10000 * this->IOHprofiler_get_instance_id());
    fopt = bbob2009_compute_fopt(19, this->IOHprofiler_get_instance_id());
    shift = std::vector<double>(n);
    for (int i = 0; i != n; ++i) {
      shift[i] = -0.5;
    }
    
    /* compute M and b */
    M = std::vector<std::vector<double>> (n);
    for (int i = 0; i != n; i++) {
      M[i] = std::vector<double> (n);
    }
    b = std::vector<double> (n);
    std::vector<std::vector<double>> rot1;
    bbob2009_compute_rotation(rot1, rseed + 1000000, n);
    scales = 1. > (sqrt((double) n) / 8.) ? 1. : (sqrt((double) n) / 8.);
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        rot1[i][j] *= scales;
      }
    }
    bbob2009_copy_rotation_matrix(rot1,M,b,n);
    
  }

  double internal_evaluate(const std::vector<double> &x) {
    std::vector<double> temp_x = x;
    size_t i = 0;
    double tmp = 0;
    std::vector<double> result(1);
    int n = temp_x.size();

    transform_vars_shift_evaluate_function(temp_x,shift);
    /* Computation core */
    result[0] = 0.0;
    for (i = 0; i < n - 1; ++i) {
      const double c1 = temp_x[i] * temp_x[i] - temp_x[i + 1];
      const double c2 = 1.0 - temp_x[i];
      tmp = 100.0 * c1 * c1 + c2 * c2;
      result[0] += tmp / 4000. - cos(tmp);
    }
    result[0] = 10. + 10. * result[0] / (double) (n - 1);

    transform_obj_shift_evaluate_function(result,fopt);

    return result[0];
  };
  
  static Griewank_RosenBrock * createInstance() {
    return new Griewank_RosenBrock();
  };

  static Griewank_RosenBrock * createInstance(int instance_id, int dimension) {
    return new Griewank_RosenBrock(instance_id, dimension);
  };
};

#endif