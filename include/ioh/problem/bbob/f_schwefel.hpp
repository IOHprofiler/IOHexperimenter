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
			public:
				Schwefel(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(20, "Schwefel", instance_id)
				{
					set_number_of_variables(dimension);
				}

				void prepare_bbob_problem(std::vector<double>& xopt, std::vector<std::vector<double>>& M,
				                          std::vector<double>& b, std::vector<std::vector<double>>& rot1,
				                          std::vector<std::vector<double>>& rot2,
				                          const long rseed, const long n
				) override
				{
					std::vector<double> tmp1, tmp2;
					tmp2 = std::vector<double>(n);
					transformation::coco::bbob2009_unif(tmp1, n, rseed);
					for (size_t i = 0; i < n; ++i)
					{
						xopt[i] = (tmp1[i] < 0.5 ? -1 : 1) * 0.5 * 4.2096874637;
					}
					for (size_t i = 0; i < n; ++i)
					{
						tmp1[i] = -2 * fabs(xopt[i]);
						tmp2[i] = 2 * fabs(xopt[i]);
					}
					transformation::coco::data::tmp1 = tmp1;
					transformation::coco::data::tmp2 = tmp2;
					transformation::coco::data::condition = 10.0;
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					size_t n = x.size();
					size_t i = 0;
					double penalty, sum;


					/* Boundary handling*/
					penalty = 0.0;
					for (i = 0; i < n; ++i)
					{
						const double tmp = fabs(x[i]) - 500.0;
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

				static Schwefel* createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
				{
					return new Schwefel(instance_id, dimension);
				}
			};
		}
	}
}
