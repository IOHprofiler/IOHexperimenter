/// \file f_Different_Powers.hpp
/// \brief cpp file for class f_Different_Powers.
///
/// A detailed file description.
///
/// \author Naama Horesh and Furong Ye
/// \date 2019-06-27
#pragma once
#include "bbob_base.hpp"

namespace ioh
{
	namespace problem
	{
		namespace bbob
		{
			class Different_Powers : public bbob_base
			{
			public:
				Different_Powers(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(14, "Different_Powers", instance_id, dimension)
				{
					set_number_of_variables(dimension);
				}

				void prepare_problem() override
				{
					transformation::coco::bbob2009_compute_xopt(xopt_, rseed_, n_);
					transformation::coco::bbob2009_compute_rotation(rot1_, rseed_ + 1000000, n_);
					transformation::coco::bbob2009_copy_rotation_matrix(rot1_, m_, b_, n_);
				}
				
				void variables_transformation(std::vector<double>& x, const int transformation_id,
					const int instance_id) override
				{
					transformation::coco::transform_vars_shift_evaluate_function(x, xopt_);
					transformation::coco::transform_vars_affine_evaluate_function(x, m_, b_);
				}
				

				double internal_evaluate(const std::vector<double>& x) override
				{
					auto sum = 0.0;
					for (size_t i = 0; i < n_; ++i)
					{
						auto exponent = 2.0 + (4.0 * static_cast<double>(static_cast<long>(i))) / (static_cast<double>
							(static_cast<long>(n_)) - 1.0);
						sum += pow(fabs(x[i]), exponent);
					}
					return sqrt(sum);
				}

				static Different_Powers* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                        int dimension = DEFAULT_DIMENSION)
				{
					return new Different_Powers(instance_id, dimension);
				}
			};
		}
	}
}
