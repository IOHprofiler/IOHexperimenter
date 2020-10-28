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
				std::vector<std::vector<double>> m1_;
				std::vector<double> b1_;
				std::vector<double> raw_x_;
			public:
				Weierstrass(int instance_id = IOH_DEFAULT_INSTANCE, int dimension = IOH_DEFAULT_DIMENSION)
					: bbob_base(16, "Weierstrass", instance_id, dimension),
					  penalty_factor_(10.0 / dimension),
					  f0(0.0), ak{0.0}, bk{0.0},
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
							m_[i][j] = 0.0;
							for (auto k = 0; k < n_; ++k)
							{
								const auto base = 1.0 / sqrt(condition_);
								const auto exponent = 1.0 * static_cast<int>(k) / (static_cast<double>(static_cast<
										long>(n_)) - 1.0
								);
								m_[i][j] += rot1_[i][k] * pow(base, exponent) * rot2_[k][j];
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
					size_t i, j;
					auto result = 0.0;
					for (i = 0; i < n_; ++i)
					{
						for (j = 0; j < summands; ++j)
						{
							result += cos(2 * transformation::coco::coco_pi * (x[i] + 0.5) * bk[j]) * ak[j];
						}
					}

					return 10.0 * pow(result / static_cast<double>(static_cast<long>(n_)) - f0, 3.0);
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
					transformation::coco::transform_vars_affine_evaluate_function(x, m1_, b1_);
					transformation::coco::transform_vars_oscillate_evaluate_function(x);
					transformation::coco::transform_vars_affine_evaluate_function(x, m_, b_);
				}

				static Weierstrass* create(int instance_id = IOH_DEFAULT_INSTANCE,
				                           int dimension = IOH_DEFAULT_DIMENSION)
				{
					return new Weierstrass(instance_id, dimension);
				}
			};
		}
	}
}
