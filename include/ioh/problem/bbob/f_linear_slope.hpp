/// \file f_linear_slope.hpp
/// \brief cpp file for class Linear_Slope.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_linear_slope.c"
///
/// \author Furong Ye
/// \date 2019-06-27
#pragma once
#include "bbob_base.hpp"

namespace ioh
{
	namespace problem
	{
		namespace bbob
		{
			class Linear_Slope : public bbob_base
			{
			public:
				Linear_Slope(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(5, "Linear_Slope", instance_id)
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
					const auto tmp_best_variables = std::make_unique<std::vector<double>>(n);
					for (auto i = 0; i < n; ++i)
					{
						if (xopt[i] < 0.0)
						{
							(*tmp_best_variables)[i] = lower_bound_;
						}
						else
						{
							(*tmp_best_variables)[i] = upper_bound_;
						}
					}
					this->set_best_variables(*tmp_best_variables);
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					const auto n = x.size();
					static const auto alpha = 100.0;
					auto result = 0.0;

					for (size_t i = 0; i < n; ++i)
					{
						double base, exponent, si;

						base = sqrt(alpha);
						exponent = static_cast<double>(static_cast<long>(i)) / (static_cast<double>(static_cast<long>(n)
						) - 1);
						if (xopt_[i] > 0.0)
							si = pow(base, exponent);
						else
							si = -pow(base, exponent);
						/* boundary handling */
						if (x[i] * xopt_[i] < 25.0)
							result += 5.0 * fabs(si) - si * x[i];
						else
							result += 5.0 * fabs(si) - si * xopt_[i];
					}
					return result;
				}
							
				static Linear_Slope* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                    int dimension = DEFAULT_DIMENSION)
				{
					return new Linear_Slope(instance_id, dimension);
				}
			};
		}
	}
}
