/// \file f_schaffers10.hpp
/// \brief cpp file for class Schaffers10.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_schaffers.c"
///
/// \author Furong Ye
/// \date 2019-09-12
#pragma once
#include "bbob_base.hpp"

namespace ioh
{
	namespace problem
	{
		namespace bbob
		{
			class Schaffers10 : public bbob_base
			{
				const double conditioning_ = 10.0;
				const double penalty_factor_ = 10.0;
				std::vector<std::vector<double>> m1_;
				std::vector<double> b1_;
				std::vector<double> raw_x_;
			public:
				Schaffers10(int instance_id = IOH_DEFAULT_INSTANCE, int dimension = IOH_DEFAULT_DIMENSION)
					: bbob_base(17, "Schaffers10", instance_id, dimension),
					  m1_(dimension, std::vector<double>(dimension)),
					  b1_(dimension),
					  raw_x_(dimension)
				{
					set_number_of_variables(dimension);
				}

				void prepare_problem() override
				{
					transformation::coco::bbob2009_compute_xopt(xopt_, rseed_, n_);
					transformation::coco::bbob2009_compute_rotation(rot1_, rseed_ + 1000000, n_);
					transformation::coco::bbob2009_compute_rotation(rot2_, rseed_, n_);
					transformation::coco::bbob2009_copy_rotation_matrix(rot1_, m1_, b1_, n_);
					for (auto i = 0; i < n_; ++i)
					{
						b_[i] = 0.0;
						for (auto j = 0; j < n_; ++j)
						{
							auto exponent = 1.0 * static_cast<int>(i) / (static_cast<double>(static_cast<long>(n_)) -
								1.0);
							m_[i][j] = rot2_[i][j] * pow(sqrt(conditioning_), exponent);
						}
					}
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					auto result = 0.0;
					for (size_t i = 0; i < n_ - 1; ++i)
					{
						const auto tmp = x[i] * x[i] + x[i + 1] * x[i + 1];
						if (std::isinf(tmp) && std::isnan(sin(50.0 * pow(tmp, 0.1)))) /* sin(inf) -> nan */
							/* the second condition is necessary to pass the integration tests under Windows and Linux */
							return tmp;
						result += pow(tmp, 0.25) * (1.0 + pow(sin(50.0 * pow(tmp, 0.1)), 2.0));
					}
					return pow(result / (static_cast<double>(static_cast<long>(n_)) - 1.0), 2.0);
				}


				void objectives_transformation(const std::vector<double>& x, std::vector<double>& y,
				                               const int transformation_id, const int instance_id) override
				{
					transformation::coco::transform_obj_shift_evaluate_function(y, fopt_);
					transformation::coco::transform_obj_penalize_evaluate(
						raw_x_, lower_bound_, upper_bound_, penalty_factor_, y);
				}

				void variables_transformation(std::vector<double>& x, const int transformation_id,
				                              const int instance_id) override
				{
					raw_x_ = x;
					transformation::coco::transform_vars_shift_evaluate_function(x, xopt_);
					transformation::coco::transform_vars_affine_evaluate_function(x, m1_, b1_);
					transformation::coco::transform_vars_asymmetric_evaluate_function(x, 0.5);
					transformation::coco::transform_vars_affine_evaluate_function(x, m_, b_);
				}

				static Schaffers10* create(int instance_id = IOH_DEFAULT_INSTANCE,
				                           int dimension = IOH_DEFAULT_DIMENSION)
				{
					return new Schaffers10(instance_id, dimension);
				}
			};
		}
	}
}
