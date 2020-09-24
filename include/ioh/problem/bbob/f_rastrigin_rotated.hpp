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
				std::vector<std::vector<double>> M1_;
				std::vector<double> b1_;
			public:
				Rastrigin_Rotated(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(15, "Rastrigin_Rotated", instance_id),
					M1_(dimension, std::vector<double>(dimension)),
					b1_(dimension)
				{
					set_number_of_variables(dimension);
				}

				void prepare_bbob_problem(std::vector<double>& xopt, std::vector<std::vector<double>>& M,
				                          std::vector<double>& b, std::vector<std::vector<double>>& rot1,
				                          std::vector<std::vector<double>>& rot2,
				                          const long rseed, const long n
				) override
				{
					transformation::coco::bbob2009_compute_xopt(xopt, rseed, n);
					transformation::coco::bbob2009_compute_rotation(rot1, rseed + 1000000, n);
					transformation::coco::bbob2009_compute_rotation(rot2, rseed, n);
					for (auto i = 0; i < n; ++i)
					{
						b[i] = 0.0;
						for (auto j = 0; j < n; ++j)
						{
							M[i][j] = 0.0;
							for (auto k = 0; k < n; ++k)
							{
								auto exponent = 1.0 * static_cast<int>(k) / (static_cast<double>(static_cast<long>(n))
									- 1.0);
								M[i][j] += rot1[i][k] * pow(sqrt(10.0), exponent) * rot2[k][j];
							}
						}
					}
					transformation::coco::bbob2009_copy_rotation_matrix(rot1, M1_, b1_, n);
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					std::vector<double> result(1);
					auto n = x.size();
					auto sum1 = 0.0, sum2 = 0.0;

					for (size_t i = 0; i < n; ++i)
					{
						sum1 += cos(2.0 * transformation::coco::coco_pi * x[i]);
						sum2 += x[i] * x[i];
					}
					/* double check std::isinf*/
					if (std::isinf(sum2))
					{
						return sum2;
					}
					return 10.0 * (static_cast<double>(static_cast<long>(n)) - sum1) + sum2;
				}

				void objectives_transformation(const std::vector<double>& x, std::vector<double>& y,
					const int transformation_id, const int instance_id) override
				{
					transformation::coco::transform_obj_shift_evaluate_function(y, fopt_);
				}

				void variables_transformation(std::vector<double>& x, const int transformation_id,
					const int instance_id) override
				{
					transformation::coco::transform_vars_shift_evaluate_function(x, xopt_);
					transformation::coco::transform_vars_affine_evaluate_function(x, M1_, b1_);
					transformation::coco::transform_vars_oscillate_evaluate_function(x);
					transformation::coco::transform_vars_asymmetric_evaluate_function(x, 0.2);
					transformation::coco::transform_vars_affine_evaluate_function(x, M_, b_);
				}


				static Rastrigin_Rotated* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                         int dimension = DEFAULT_DIMENSION)
				{
					return new Rastrigin_Rotated(instance_id, dimension);
				}
			};
		}
	}
}
