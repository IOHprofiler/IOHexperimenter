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
					: bbob_base(24, "Lunacek_Bi_Rastrigin", instance_id, dimension)
				{
					set_number_of_variables(dimension);
				}

				void prepare_problem() override
				{
					std::vector<double> tmpvect;
					transformation::coco::bbob2009_compute_xopt(xopt_, rseed_, n_);
					transformation::coco::bbob2009_compute_rotation(rot1_, rseed_ + 1000000, n_);
					transformation::coco::bbob2009_compute_rotation(rot2_, rseed_, n_);
					transformation::coco::bbob2009_gauss(tmpvect, n_, rseed_);
					for (auto i = 0; i < n_; ++i)
					{
						xopt_[i] = 0.5 * 2.5;
						if (tmpvect[i] < 0.0)
						{
							xopt_[i] *= -1.0;
						}
					}
					set_best_variables(xopt_);
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					static const auto condition = 100.;
					size_t i, j;
					auto penalty = 0.0;
					static const auto mu0 = 2.5;
					static const auto d = 1.;
					const auto s = 1. - 0.5 / (sqrt(static_cast<double>(n_ + 20)) - 4.1);
					const auto mu1 = -sqrt((mu0 * mu0 - d) / s);
					auto sum1 = 0., sum2 = 0., sum3 = 0.;
					std::vector<double> tmpvect(n_);
					std::vector<double> x_hat(n_);
					std::vector<double> z(n_);

					for (i = 0; i < n_; ++i)
					{
						double tmp;
						tmp = fabs(x[i]) - 5.0;
						if (tmp > 0.0)
							penalty += tmp * tmp;
					}

					/* x_hat */
					for (i = 0; i < n_; ++i)
					{
						x_hat[i] = 2. * x[i];
						if (xopt_[i] < 0.)
						{
							x_hat[i] *= -1.;
						}
					}
					/* affine transformation */
					for (i = 0; i < n_; ++i)
					{
						double c1;
						tmpvect[i] = 0.0;
						c1 = pow(sqrt(condition), static_cast<double>(i) / static_cast<double>(n_ - 1));
						for (j = 0; j < n_; ++j)
						{
							tmpvect[i] += c1 * rot2_[i][j] * (x_hat[j] - mu0);
						}
					}
					for (i = 0; i < n_; ++i)
					{
						z[i] = 0;
						for (j = 0; j < n_; ++j)
						{
							z[i] += rot1_[i][j] * tmpvect[j];
						}
					}
					/* Computation core */
					for (i = 0; i < n_; ++i)
					{
						sum1 += (x_hat[i] - mu0) * (x_hat[i] - mu0);
						sum2 += (x_hat[i] - mu1) * (x_hat[i] - mu1);
						sum3 += cos(2 * transformation::coco::coco_pi * z[i]);
					}
					return std::min(sum1, d * static_cast<double>(n_) + s * sum2)
						+ 10. * (static_cast<double>(n_) - sum3) + 1e4 * penalty;
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
