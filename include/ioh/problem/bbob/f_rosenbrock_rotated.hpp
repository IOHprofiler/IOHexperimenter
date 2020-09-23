/// \file f_rosenbrock_rotated.hpp
/// \brief cpp file for class Rosenbrock_Rotated.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_rosenbrock.c"
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
			class Rosenbrock_Rotated : public bbob_base
			{
			public:
				Rosenbrock_Rotated(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(9, "Rosenbrock_Rotated", instance_id)
				{
					set_number_of_variables(dimension);
				}

				void prepare_bbob_problem(std::vector<double>& xopt, std::vector<std::vector<double>>& M,
				                          std::vector<double>& b, std::vector<std::vector<double>>& rot1,
				                          std::vector<std::vector<double>>& rot2,
				                          const long rseed, const long n
				) override
				{
					transformation::coco::data::factor = 1.0 > (sqrt(static_cast<double>(n)) / 8.0)
						                                     ? 1
						                                     : (sqrt(static_cast<double>(n)) / 8.0);
					transformation::coco::bbob2009_compute_rotation(rot1, rseed, n);
					for (auto row = 0; row < n; ++row)
					{
						for (auto column = 0; column < n; ++column)
						{
							M[row][column] = transformation::coco::data::factor * rot1[row][column];
						}
						b[row] = 0.5;
					}
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					double s1 = 0.0, s2 = 0.0, tmp;
					for (size_t i = 0; i < x.size() - 1; ++i)
					{
						tmp = (x[i] * x[i] - x[i + 1]);
						s1 += tmp * tmp;
						tmp = (x[i] - 1.0);
						s2 += tmp * tmp;
					}
					return 100.0 * s1 + s2;
				}

				static Rosenbrock_Rotated* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                          int dimension = DEFAULT_DIMENSION)
				{
					return new Rosenbrock_Rotated(instance_id, dimension);
				}
			};
		}
	}
}
