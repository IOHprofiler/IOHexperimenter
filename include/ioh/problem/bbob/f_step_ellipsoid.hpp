/// \file f_step_ellipsoid.hpp
/// \brief cpp file for class Step_Ellipsoid.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_step_ellipsoid.c"
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
			class Step_Ellipsoid : public bbob_base
			{
				std::vector<double> datax_;
				std::vector<double> dataxx_;
				
			public:
				Step_Ellipsoid(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(7, "Step_Ellipsoid", instance_id),
					datax_(dimension),
					dataxx_(dimension)
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
					set_best_variables(xopt);
				}


				double internal_evaluate(const std::vector<double>& x) override
				{
					using namespace transformation::coco;
					auto n = x.size();
					static const double condition = 100;
					static const auto alpha = 10.0;
					size_t i, j;
					double penalty = 0.0, x1;
					std::vector<double> result(1);

					for (i = 0; i < n; ++i)
					{
						double tmp;
						tmp = fabs(x[i]) - 5.0;
						if (tmp > 0.0)
							penalty += tmp * tmp;
					}

					for (i = 0; i < n; ++i)
					{
						double c1;
						datax_[i] = 0.0;
						c1 = sqrt(pow(condition / 10., static_cast<double>(i) / static_cast<double>(n - 1)));
						for (j = 0; j < n; ++j)
						{
							datax_[i] += c1 * rot2_[i][j] * (x[j] - xopt_[j]);
						}
					}
					x1 = datax_[0];

					for (i = 0; i < n; ++i)
					{
						if (fabs(datax_[i]) > 0.5) /* TODO: Documentation: no fabs() in documentation */
							datax_[i] = static_cast<double>(floor(datax_[i] + 0.5));
						else
							datax_[i] = static_cast<double>(floor(alpha * datax_[i] + 0.5)) / alpha;
					}

					for (i = 0; i < n; ++i)
					{
						dataxx_[i] = 0.0;
						for (j = 0; j < n; ++j)
						{
							dataxx_[i] += rot1_[i][j] * datax_[j];
						}
					}

					/* Computation core */
					result[0] = 0.0;
					for (i = 0; i < n; ++i)
					{
						double exponent;
						exponent = static_cast<double>(static_cast<long>(i)) / (static_cast<double>(static_cast<long>(n)
						) - 1.0);
						result[0] += pow(condition, exponent) * dataxx_[i] * dataxx_[i];
					}
					result[0] = 0.1 * ((fabs(x1) * 1.0e-4) > result[0] ? (fabs(x1) * 1.0e-4) : result[0]) + penalty +
						fopt_;

					return result[0];
				}



				void objectives_transformation(const std::vector<double>& x, std::vector<double>& y,
					const int transformation_id, const int instance_id) override
				{
					// Needs to override default beviour
				}
				
				static Step_Ellipsoid* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                      int dimension = DEFAULT_DIMENSION)
				{
					return new Step_Ellipsoid(instance_id, dimension);
				}
			};
		}
	}
}
