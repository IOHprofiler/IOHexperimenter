/// \file f_griewank_rosenBrock.hpp
/// \brief hpp file for class Griewank_RosenBrock.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_griewank_rosenbrock.c"
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
			class Griewank_RosenBrock : public bbob_base
			{
				double scales = 0.0;

			public:
				Griewank_RosenBrock(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(19, "Griewank_RosenBrock", instance_id)
				{
					set_number_of_variables(dimension);
				}

				void prepare_bbob_problem(std::vector<double>& xopt, std::vector<std::vector<double>>& M,
				                          std::vector<double>& b, std::vector<std::vector<double>>& rot1,
				                          std::vector<std::vector<double>>& rot2,
				                          const long rseed, const long n
				) override
				{
					for (auto i = 0; i < n; ++i)
					{
						xopt[i] = -0.5;
					}
					transformation::coco::bbob2009_compute_rotation(rot1, rseed, n);
					scales = 1. > (sqrt(static_cast<double>(n)) / 8.) ? 1. : (sqrt(static_cast<double>(n)) / 8.);
					for (auto i = 0; i < n; ++i)
					{
						for (auto j = 0; j < n; ++j)
						{
							rot1[i][j] *= scales;
						}
					}
					transformation::coco::bbob2009_copy_rotation_matrix(rot1, M, b, n);
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					double tmp = 0;
					auto result = 0.0;
					auto n = x.size();

					for (size_t i = 0; i < n - 1; ++i)
					{
						const auto c1 = x[i] * x[i] - x[i + 1];
						const auto c2 = 1.0 - x[i];
						tmp = 100.0 * c1 * c1 + c2 * c2;
						result += tmp / 4000. - cos(tmp);
					}
					return 10. + 10. * result / static_cast<double>(n - 1);
				}

				void objectives_transformation(const std::vector<double>& x, std::vector<double>& y,
					const int transformation_id, const int instance_id) override
				{
					transformation::coco::transform_obj_shift_evaluate_function(y, fopt_);
				}

				void variables_transformation(std::vector<double>& x, const int transformation_id,
					const int instance_id) override
				{
					transformation::coco::transform_vars_affine_evaluate_function(x, M_, b_);
					transformation::coco::transform_vars_shift_evaluate_function(x, xopt_);
				}


				static Griewank_RosenBrock* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                           int dimension = DEFAULT_DIMENSION)
				{
					return new Griewank_RosenBrock(instance_id, dimension);
				}
			};
		}
	}
}
