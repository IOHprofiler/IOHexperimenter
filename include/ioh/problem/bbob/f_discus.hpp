/// \file f_Discus.hpp
/// \brief cpp file for class f_Discus.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_discus.c"
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
			class Discus : public bbob_base
			{
			public:
				Discus(int instance_id = IOH_DEFAULT_INSTANCE, int dimension = IOH_DEFAULT_DIMENSION)
					: bbob_base(11, "Discus", instance_id, dimension)
				{
					set_number_of_variables(dimension);
				}

				void prepare_problem() override
				{
					transformation::coco::bbob2009_compute_xopt(xopt_, rseed_, n_);
					transformation::coco::bbob2009_compute_rotation(rot1_, rseed_ + 1000000, n_);
					transformation::coco::bbob2009_copy_rotation_matrix(rot1_, m_, b_, n_);
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					static const auto condition = 1.0e6;
					auto result = condition * x[0] * x[0];
					for (size_t i = 1; i < n_; ++i)
					{
						result += x[i] * x[i];
					}
					return result;
				}

				void variables_transformation(std::vector<double>& x, const int transformation_id,
				                              const int instance_id) override
				{
					transformation::coco::transform_vars_shift_evaluate_function(x, xopt_);
					transformation::coco::transform_vars_affine_evaluate_function(x, m_, b_);
					transformation::coco::transform_vars_oscillate_evaluate_function(x);
				}


				static Discus* create(int instance_id = IOH_DEFAULT_INSTANCE, int dimension = IOH_DEFAULT_DIMENSION)
				{
					return new Discus(instance_id, dimension);
				}
			};
		}
	}
}
