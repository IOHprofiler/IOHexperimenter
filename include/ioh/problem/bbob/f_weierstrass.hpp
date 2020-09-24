/// \file f_weierstrass.hpp
/// \brief cpp file for class Weierstrass.
///
/// A detailed file description.
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
			class Weierstrass : public bbob_base
			{
				static constexpr int summands = 10;
				const double condition_ = 100.0;
				double penalty_factor_;
				double f0;
				double ak[summands];
				double bk[summands];
				std::vector<std::vector<double>> M1_;
				std::vector<double> b1_;
				std::vector<double> raw_x_;
			public:
				Weierstrass(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(16, "Weierstrass", instance_id),
					penalty_factor_(10.0 / dimension),
					f0(0.0), ak{0.0}, bk{0.0},
					M1_(dimension, std::vector<double>(dimension)),
					b1_(dimension),
					raw_x_(dimension)		
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
					transformation::coco::bbob2009_copy_rotation_matrix(rot1, M1_, b1_, n);
					for (auto i = 0; i < n; ++i)
					{
						b[i] = 0.0;
						for (auto j = 0; j < n; ++j)
						{
							M[i][j] = 0.0;
							for (auto k = 0; k < n; ++k)
							{
								const auto base = 1.0 / sqrt(condition_);
								const auto exponent = 1.0 * static_cast<int>(k) / (static_cast<double>(static_cast<
										long>(n)) - 1.0
								);
								M[i][j] += rot1[i][k] * pow(base, exponent) * rot2[k][j];
							}
						}
					}
					f0 = 0.0;
					for (auto i = 0; i < summands; ++i)
					{
						ak[i] = pow(0.5, static_cast<double>(i));
						bk[i] = pow(3., static_cast<double>(i));
						f0 += ak[i] * cos(2 * transformation::coco::coco_pi * bk[i] * 0.5);
					}
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					auto n = x.size();
					size_t i, j;
					auto result = 0.0;
					for (i = 0; i < n; ++i)
					{
						for (j = 0; j < summands; ++j)
						{
							result += cos(2 * transformation::coco::coco_pi * (x[i] + 0.5) * bk[j]) * ak[j];
						}
					}

					return 10.0 * pow(result / static_cast<double>(static_cast<long>(n)) - f0, 3.0);
				}

				void objectives_transformation(const std::vector<double>& x, std::vector<double>& y,
					const int transformation_id, const int instance_id) override
				{
					transformation::coco::transform_obj_shift_evaluate_function(y, fopt_);
					transformation::coco::transform_obj_penalize_evaluate(raw_x_, lower_bound_, upper_bound_,
					                                                      penalty_factor_, y);
				}

				void variables_transformation(std::vector<double>& x, const int transformation_id,
					const int instance_id) override
				{
					raw_x_ = x;
					transformation::coco::transform_vars_shift_evaluate_function(x, xopt_);
					transformation::coco::transform_vars_affine_evaluate_function(x, M1_, b1_);
					transformation::coco::transform_vars_oscillate_evaluate_function(x);
					transformation::coco::transform_vars_affine_evaluate_function(x, M_, b_);
				}

				static Weierstrass* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                   int dimension = DEFAULT_DIMENSION)
				{
					return new Weierstrass(instance_id, dimension);
				}
			};
		}
	}
}
