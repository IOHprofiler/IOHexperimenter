/// \file f_lunacek_bi_rastrigin.hpp
/// \brief cpp file for class Lunacek_Bi_Rastrigin.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_lunacek_bi_rastrigin.c"
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
			class Lunacek_Bi_Rastrigin : public bbob_base
			{
			public:
				Lunacek_Bi_Rastrigin(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(24, "Lunacek_Bi_Rastrigin", instance_id)
				{
					set_number_of_variables(dimension);
				}

				void prepare_bbob_problem(std::vector<double>& xopt, std::vector<std::vector<double>>& M,
				                          std::vector<double>& b, std::vector<std::vector<double>>& rot1,
				                          std::vector<std::vector<double>>& rot2,
				                          const long rseed, const long n
				) override
				{
					std::vector<double> tmpvect;
					transformation::coco::bbob2009_compute_xopt(xopt, rseed, n);
					transformation::coco::bbob2009_compute_rotation(rot1, rseed + 1000000, n);
					transformation::coco::bbob2009_compute_rotation(rot2, rseed, n);
					transformation::coco::bbob2009_gauss(tmpvect, n, rseed);
					for (int i = 0; i < n; ++i)
					{
						xopt[i] = 0.5 * 2.5;
						if (tmpvect[i] < 0.0)
						{
							xopt[i] *= -1.0;
						}
					}
					set_best_variables(xopt);
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					size_t n = x.size();
					static const double condition = 100.;
					size_t i, j;
					double penalty = 0.0;
					static const double mu0 = 2.5;
					static const double d = 1.;
					const double s = 1. - 0.5 / (sqrt(static_cast<double>(n + 20)) - 4.1);
					const double mu1 = -sqrt((mu0 * mu0 - d) / s);
					double sum1 = 0., sum2 = 0., sum3 = 0.;
					std::vector<double> tmpvect(n);
					std::vector<double> x_hat(n);
					std::vector<double> z(n);

					for (i = 0; i < n; ++i)
					{
						double tmp;
						tmp = fabs(x[i]) - 5.0;
						if (tmp > 0.0)
							penalty += tmp * tmp;
					}

					/* x_hat */
					for (i = 0; i < n; ++i)
					{
						x_hat[i] = 2. * x[i];
						if (transformation::coco::data::xopt[i] < 0.)
						{
							x_hat[i] *= -1.;
						}
					}
					/* affine transformation */
					for (i = 0; i < n; ++i)
					{
						double c1;
						tmpvect[i] = 0.0;
						c1 = pow(sqrt(condition), static_cast<double>(i) / static_cast<double>(n - 1));
						for (j = 0; j < n; ++j)
						{
							tmpvect[i] += c1 * transformation::coco::data::rot2[i][j] * (x_hat[j] - mu0);
						}
					}
					for (i = 0; i < n; ++i)
					{
						z[i] = 0;
						for (j = 0; j < n; ++j)
						{
							z[i] += transformation::coco::data::rot1[i][j] * tmpvect[j];
						}
					}
					/* Computation core */
					for (i = 0; i < n; ++i)
					{
						sum1 += (x_hat[i] - mu0) * (x_hat[i] - mu0);
						sum2 += (x_hat[i] - mu1) * (x_hat[i] - mu1);
						sum3 += cos(2 * transformation::coco::coco_pi * z[i]);
					}
					return std::min(sum1, d * static_cast<double>(n) + s * sum2)
						+ 10. * (static_cast<double>(n) - sum3) + 1e4 * penalty;
				}

				static Lunacek_Bi_Rastrigin* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                            int dimension = DEFAULT_DIMENSION)
				{
					return new Lunacek_Bi_Rastrigin(instance_id, dimension);
				}
			};
		}
	}
}
