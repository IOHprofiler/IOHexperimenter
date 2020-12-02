/// \file IOHprofiler_transformation.hpp
/// \brief Header file for the class IOHprofiler_transformation.
///
/// \author Furong Ye
#ifndef IOHPROFILER_TRANSFORMATION_H
#define IOHPROFILER_TRANSFORMATION_H

#include "IOHprofiler_common.h"
#include "IOHprofiler_random.h"
#include "coco_transformation.hpp"

/// \brief A class consisting of transformation methods.
class IOHprofiler_transformation {
public:
  
  /// \fn void variables_transformation(std::vector<int> &x, const int problem_id, const int instance_id, const std::string problem_type)
  /// \brief Transformation operations on variables.
  ///
  /// For instance_id in ]1,50], xor operation is applied.
  /// For instance_id in ]50,100], sigma function is applied.
  void variables_transformation(std::vector<int> &x, const int problem_id, const int instance_id, const std::string problem_type);

  void variables_transformation(std::vector<double> &x, const int problem_id, const int instance_id, const std::string problem_type);

  /// \fn void objectives_transformation(const std::vector<int> &x, std::vector<double> &y, const int problem_id, const int instance_id, const std::string problem_type)
  /// \brief Transformation operations on objectives (a * f(x) + b).
  void objectives_transformation(const std::vector<int> &x, std::vector<double> &y, const int problem_id, const int instance_id, const std::string problem_type);

  void objectives_transformation(const std::vector<double> &x, std::vector<double> &y, const int problem_id, const int instance_id, const std::string problem_type);
  
  /// \fn xor_compute(const int x1, const int x2)
  ///
  /// Exclusive or operation on x1, x2. x1 and x2 should either 0 or 1.
  static int xor_compute(const int x1, const int x2);

  /// \fn transform_vars_xor(std::vector<int> &x, const int seed)
  ///
  /// Applying xor operations on x with a uniformly random bit string.
  void transform_vars_xor(std::vector<int> &x, const int seed);

  static int sigma_compute(const std::vector<int> &x, const int pos);

  /// \fn transform_vars_sigma(std::vector<int> &x, const int seed)
  ///
  /// Disrupting the order of x.
  void transform_vars_sigma(std::vector<int> &x, const int seed);

  /// \fn void transform_obj_scale(std::vector<double> &y, const int seed)
  /// \brief transformation 'a * f(x)'.
  void transform_obj_scale(std::vector<double> &y, const int seed);

  /// \fn void transform_obj_shift(std::vector<double> &y, const int seed)
  /// \brief transformation 'f(x) + b'.
  void transform_obj_shift(std::vector<double> &y, const int seed);

  /// \fn void transform_obj_scale(std::vector<double> &y, const int seed)
  /// \brief transformation 'a * f(x)'.
  void transform_obj_scale(double &y, const int seed);

  /// \fn void transform_obj_shift(std::vector<double> &y, const int seed)
  /// \brief transformation 'f(x) + b'.
  void transform_obj_shift(double &y, const int seed);
};

#endif //IOHPROFILER_TRANSFORMATION_H