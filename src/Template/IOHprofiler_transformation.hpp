/// \file IOHprofiler_transformation.hpp
/// \brief Head file for class IOHprofiler_transformation.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef IOHPROFILER_TRANSFORMATION_HPP
#define IOHPROFILER_TRANSFORMATION_HPP

#include "IOHprofiler_common.h"
#include "IOHprofiler_random.hpp"
#include "../Problems/BBOB/bbob_common_used_functions/coco_transformation.h"

/// \brief A class consisting of transformation methods.
class IOHprofiler_transformation: public IOHprofiler_random {
/// todo. No inheritance here.
public:

  /// \fn void variables_transformation(std::vector<InputType> &x)
  /// \brief Transformation operations on variables.
  ///
  /// For instance_id in ]1,50], xor operation is applied.
  /// For instance_id in ]50,100], \sigma function is applied.

  void variables_transformation(std::vector<int> &x, const int problem_id, const int instance_id, const std::string problem_type) { 
    if (problem_type == "pseudo_Boolean_problem") {
      if (instance_id > 1 && instance_id <= 50) { 
        this->transform_vars_xor(x,instance_id);
      } else if (instance_id > 50 && instance_id <= 100) {
        this->transform_vars_sigma(x,instance_id);
      }
    }
  };

  void variables_transformation(std::vector<double> &x, const int problem_id, const int instance_id, const std::string problem_type) {
    if (problem_type == "bbob") {
      coco_tranformation_vars(x,problem_id);
    }
  };

  /// \fn void objectives_transformation(std::vector<double> &y)
  /// \brief Transformation operations on objectives (a * f(x) + b).

  void objectives_transformation(const std::vector<int> &x, std::vector<double> &y, const int problem_id, const int instance_id, const std::string problem_type) {
    if (problem_type == "pseudo_Boolean_problem") {
      if (instance_id > 1) {
        this->transform_obj_scale(y,instance_id);
        this->transform_obj_shift(y,instance_id);
      }
    }
  };

  void objectives_transformation(const std::vector<double> &x, std::vector<double> &y, const int problem_id, const int instance_id, const std::string problem_type) {
    if (problem_type == "bbob") {
      coco_tranformation_objs(x,y,problem_id);
    }
  }
  
  /// \fn xor_compute(const int x1, const int x2)
  ///
  /// Exclusive or operation on x1, x2. x1 and x2 should either 0 or 1.
  static int xor_compute(const int x1, const int x2){
    return (int) (x1 != x2);
  };

  /// \fn transform_vars_xor(std::vector<int> &x, const int seed)
  ///
  /// Applying xor operations on x with a uniformly random bit string.
  void transform_vars_xor(std::vector<int> &x, const int seed) {
    std::vector<double> random_x;
    int xor_value;
    int n = x.size();
    IOHprofiler_uniform_rand(n,seed, random_x);
    for (int i = 0; i < n; ++i) {
      xor_value = (int)(2 * floor(1e4 * random_x[i]) / 1e4 / 1);
      x[i] = xor_compute(x[i],xor_value);
    }
  }

  static int sigma_compute(const std::vector<int> &x, const int pos) {
    return x[pos];
  }

  /// \fn transform_vars_sigma(std::vector<int> &x, const int seed)
  ///
  /// Disrupting the order of x.
  void transform_vars_sigma(std::vector<int> &x, const int seed) {
    std::vector<int> copy_x;
    std::vector<int> index;
    std::vector<double> random_x;
    int N = x.size(),t,temp;
    
    copy_x = x;

    index.reserve(N);
    for (int i = 0; i != N; ++i) {
      index[i] = i;
    }

    IOHprofiler_uniform_rand(N,seed,random_x);
    for (int i = 0; i != N; ++i) {
      t = (int)floor(random_x[i] * N);
      temp = index[0];
      index[0] = index[t];
      index[t] = temp;
    }
    for (int i = 0; i < N; ++i) {
      x[i] = sigma_compute(copy_x,index[i]);
    }
  }

  /// \fn void transform_obj_scale(std::vector<double> &y, const int seed)
  /// \brief transformation 'a * f(x)'.
  void transform_obj_scale(std::vector<double> &y, const int seed) {
    std::vector<double> scale;
    IOHprofiler_uniform_rand(1,seed,scale);
    scale[0] = scale[0] * 1e4 / 1e4 * 4.8 + 0.2;
    for (int i = 0; i < y.size(); ++i) {
      y[i] = y[i] * scale[0];
    }
  }

  /// \fn void transform_obj_shift(std::vector<double> &y, const int seed)
  /// \brief transformation 'f(x) + b'.
  void transform_obj_shift(std::vector<double> &y, const int seed) {
    std::vector<double> shift;
    IOHprofiler_uniform_rand(1,seed,shift);
    shift[0] = shift[0] * 1e4 / 1e4 * 2000 - 1000;
    for (int i = 0; i < y.size(); ++i) {
      y[i] = y[i] + shift[0];
    }
  }

  /// \fn void transform_obj_scale(std::vector<double> &y, const int seed)
  /// \brief transformation 'a * f(x)'.
  void transform_obj_scale(double &y, const int seed) {
    std::vector<double> scale;
    IOHprofiler_uniform_rand(1,seed,scale);
    scale[0] = scale[0] * 1e4 / 1e4 * 4.8 + 0.2;
    y = y * scale[0];
  }

  /// \fn void transform_obj_shift(std::vector<double> &y, const int seed)
  /// \brief transformation 'f(x) + b'.
  void transform_obj_shift(double &y, const int seed) {
    std::vector<double> shift;
    IOHprofiler_uniform_rand(1,seed,shift);
    shift[0] = shift[0] * 1e4 / 1e4 * 2000 - 1000;
    y = y + shift[0];
  }
};
#endif //IOHPROFILER_TRANSFORMATION_HPP