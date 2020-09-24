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
					using namespace transformation::coco;
					bbob2009_compute_xopt(xopt, rseed, n);
					bbob2009_compute_rotation(rot1, rseed + 1000000, n);
					bbob2009_compute_rotation(rot2, rseed, n);
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
				}

				void objectives_transformation(const std::vector<double>& x, std::vector<double>& y,
				                               const int transformation_id, const int instance_id) override
				{
					using namespace transformation::coco;
					transform_obj_oscillate_evaluate(y);
					transform_obj_power_evaluate(y, 0.9);
					transform_obj_shift_evaluate_function(y, fopt_);
				}

				void variables_transformation(std::vector<double>& x, const int transformation_id,
				                              const int instance_id) override
				{
					using namespace transformation::coco;
					transform_vars_shift_evaluate_function(x, xopt_);
					transform_vars_affine_evaluate_function(x, M_, b_);
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					auto result = 0.0;
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
