/// \file f_schwefel.hpp
/// \brief cpp file for class Schwefel.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_schwefel.c"
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
			class Schwefel : public bbob_base
			{
				const double condition_ = 10.0;
				std::vector<double> negative_offset_;
				std::vector<double> positive_offset_;
				
			public:
				Schwefel(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(20, "Schwefel", instance_id),
					negative_offset_(dimension),
					positive_offset_(dimension)
				{
					set_number_of_variables(dimension);
				}

				void prepare_bbob_problem(std::vector<double>& xopt, std::vector<std::vector<double>>& M,
				                          std::vector<double>& b, std::vector<std::vector<double>>& rot1,
				                          std::vector<std::vector<double>>& rot2,
				                          const long rseed, const long n
				) override
				{
					transformation::coco::bbob2009_unif(negative_offset_, n, rseed);
					for (size_t i = 0; i < n; ++i)
						xopt[i] = (negative_offset_[i] < 0.5 ? -1 : 1) * 0.5 * 4.2096874637;

					for (size_t i = 0; i < n; ++i)
					{
						negative_offset_[i] = -2 * fabs(xopt[i]);
						positive_offset_[i] = 2 * fabs(xopt[i]);
					}
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					auto n = x.size();
					size_t i = 0;
					double penalty, sum;

					/* Boundary handling*/
					penalty = 0.0;
					for (i = 0; i < n; ++i)
					{
						const auto tmp = fabs(x[i]) - 500.0;
						if (tmp > 0.0)
						{
							penalty += tmp * tmp;
						}
					}

					/* Computation core */
					sum = 0.0;
					for (i = 0; i < n; ++i)
					{
						sum += x[i] * sin(sqrt(fabs(x[i])));
					}

					return 0.01 * (penalty + 418.9828872724339 - sum / static_cast<double>(n));
				}

				void objectives_transformation(const std::vector<double>& x, std::vector<double>& y,
					const int transformation_id, const int instance_id) override
				{
					transformation::coco::transform_obj_shift_evaluate_function(y, fopt_);
					
				}

				void variables_transformation(std::vector<double>& x, const int transformation_id,
					const int instance_id) override
				{
					using namespace transformation::coco;
					transform_vars_x_hat_evaluate(x, rseed_);
					transform_vars_scale_evaluate(x, 2);
					transform_vars_z_hat_evaluate(x, xopt_);
					transform_vars_shift_evaluate_function(x, positive_offset_);
					transform_vars_conditioning_evaluate(x, 10.0);
					transform_vars_shift_evaluate_function(x, negative_offset_);
					transform_vars_scale_evaluate(x, 100);
				}

				static Schwefel* createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
				{
					return new Schwefel(instance_id, dimension);
				}
			};
		}
	}
}
