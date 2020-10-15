/// \file f_rastrigin_rotated.hpp
/// \brief cpp file for class Rastrigin_Rotated.
/// 
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_rastrigin.c"
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
			class Rastrigin_Rotated : public bbob_base
			{
				std::vector<std::vector<double>> m1_;
				std::vector<double> b1_;
			public:
				Rastrigin_Rotated(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(15, "Rastrigin_Rotated", instance_id, dimension),
					  m1_(dimension, std::vector<double>(dimension)),
					  b1_(dimension)
				{
					set_number_of_variables(dimension);
				}

				void prepare_problem() override
				{
					transformation::coco::bbob2009_compute_xopt(xopt_, rseed_, n_);
					transformation::coco::bbob2009_compute_rotation(rot1_, rseed_ + 1000000, n_);
					transformation::coco::bbob2009_compute_rotation(rot2_, rseed_, n_);
					for (auto i = 0; i < n_; ++i)
					{
						b_[i] = 0.0;
						for (auto j = 0; j < n_; ++j)
						{
							m_[i][j] = 0.0;
							for (auto k = 0; k < n_; ++k)
							{
								auto exponent = 1.0 * static_cast<int>(k) / (static_cast<double>(static_cast<long>(n_))
									- 1.0);
								m_[i][j] += rot1_[i][k] * pow(sqrt(10.0), exponent) * rot2_[k][j];
							}
						}
					}
					transformation::coco::bbob2009_copy_rotation_matrix(rot1_, m1_, b1_, n_);
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					std::vector<double> result(1);
					auto sum1 = 0.0, sum2 = 0.0;

					for (size_t i = 0; i < n_; ++i)
					{
						sum1 += cos(2.0 * transformation::coco::coco_pi * x[i]);
						sum2 += x[i] * x[i];
					}
					/* double check std::isinf*/
					if (std::isinf(sum2))
					{
						return sum2;
					}
					return 10.0 * (static_cast<double>(static_cast<long>(n_)) - sum1) + sum2;
				}

				void variables_transformation(std::vector<double>& x, const int transformation_id,
				                              const int instance_id) override
				{
					transformation::coco::transform_vars_shift_evaluate_function(x, xopt_);
					transformation::coco::transform_vars_affine_evaluate_function(x, m1_, b1_);
					transformation::coco::transform_vars_oscillate_evaluate_function(x);
					transformation::coco::transform_vars_asymmetric_evaluate_function(x, 0.2);
					transformation::coco::transform_vars_affine_evaluate_function(x, m_, b_);
				}


				static Rastrigin_Rotated* create(int instance_id = DEFAULT_INSTANCE,
				                                 int dimension = DEFAULT_DIMENSION)
				{
					return new Rastrigin_Rotated(instance_id, dimension);
				}
			};
		}
	}
}
