/// \file f_ellipsoid_rotated.hpp
/// \brief cpp file for class Ellipsoid_Rotated.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_ellipsoid.c"
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
			class Ellipsoid_Rotated : public bbob_base
			{
			public:
				Ellipsoid_Rotated(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(10, "Ellipsoid_Rotated", instance_id)
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
					transformation::coco::bbob2009_copy_rotation_matrix(rot1, M, b, n);
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					static const auto condition = 1.0e6;
					auto n = x.size();

					auto result = x[0] * x[0];
					for (size_t i = 1; i < n; ++i)
					{
						const auto exponent = 1.0 * static_cast<double>(static_cast<long>(i)) / (static_cast<double>(
							static_cast<long>(n)) - 1.0);
						result += pow(condition, exponent) * x[i] * x[i];
					}
					return result;
				}

				void objectives_transformation(const std::vector<double>& x, std::vector<double>& y,
					const int transformation_id, const int instance_id) override
				{
					transformation::coco::transform_obj_shift_evaluate_function(y, fopt_);
				}
				

				void variables_transformation(std::vector<double>& x, const int transformation_id,
					const int instance_id) override
				{
					transformation::coco::transform_vars_shift_evaluate_function(x, xopt_);
					transformation::coco::transform_vars_affine_evaluate_function(x, M_, b_);
					transformation::coco::transform_vars_oscillate_evaluate_function(x);
				}

				static Ellipsoid_Rotated* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                         int dimension = DEFAULT_DIMENSION)
				{
					return new Ellipsoid_Rotated(instance_id, dimension);
				}
			};
		}
	}
}
