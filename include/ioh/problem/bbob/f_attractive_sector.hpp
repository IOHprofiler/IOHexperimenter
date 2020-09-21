/// \file f_Attractive_Sector.hpp
/// \brief cpp file for class f_Attractive_Sector.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_attractive_sector.c"
///
/// \author Furong Ye
/// \date 2019-09-10
#pragma once
#include "bbob_base.hpp"

namespace ioh
{
	namespace problem
	{
		namespace bbob
		{
			class Attractive_Sector : public bbob_base
			{
			public:
				Attractive_Sector(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(6, "Attractive_Sector", instance_id)
				{
					//TODO: This has to be called inside the child class need we to think of something here
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
					for (int i = 0; i < n; ++i)
					{
						b[i] = 0.0;
						for (int j = 0; j < n; ++j)
						{
							M[i][j] = 0.0;
							for (int k = 0; k < n; ++k)
							{
								double exponent = 1.0 * static_cast<int>(k) / (static_cast<double>(static_cast<long>(n))
									- 1.0);
								M[i][j] += rot1[i][k] * pow(sqrt(10.0), exponent) * rot2[k][j];
							}
						}
					}
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					double result = 0.0;
					for (size_t i = 0; i < x.size(); ++i)
					{
						if (transformation::coco::data::xopt[i] * x[i] > 0.0)
						{
							result += 100.0 * 100.0 * x[i] * x[i];
						}
						else
						{
							result += x[i] * x[i];
						}
					}
					return result;
				}

				static Attractive_Sector* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                         int dimension = DEFAULT_DIMENSION)
				{
					return new Attractive_Sector(instance_id, dimension);
				}
			};
		}
	}
}
