/// \file f_rastrigin.hpp
/// \brief cpp file for class Rastrigin.
/// 
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_rastrigin.c"
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
			class Rastrigin : public bbob_base
			{
			public:
				Rastrigin(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(3, "Rastrigin", instance_id, dimension)
				{
					set_number_of_variables(dimension);
				}

				void prepare_problem() override
				{
					transformation::coco::bbob2009_compute_xopt(xopt_, rseed_, n_);
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					auto sum1 = 0.0, sum2 = 0.0;

					for (size_t i = 0; i < n_; ++i)
					{
						sum1 += cos(2.0 * transformation::coco::coco_pi * x[i]);
						sum2 += x[i] * x[i];
					}
					/* double check std::isinf*/
					if (std::isinf(sum2))
					{
						return sum2;
					}
					return 10.0 * (static_cast<double>(static_cast<long>(n_)) - sum1) + sum2;
				}

		

				void variables_transformation(std::vector<double>& x, const int transformation_id,
					const int instance_id) override
				{
					transformation::coco::transform_vars_shift_evaluate_function(x, xopt_);
					transformation::coco::transform_vars_oscillate_evaluate_function(x);
					transformation::coco::transform_vars_asymmetric_evaluate_function(x, 0.2);
					transformation::coco::transform_vars_conditioning_evaluate(x, 10.0);
				}

				static Rastrigin* createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
				{
					return new Rastrigin(instance_id, dimension);
				}
			};
		}
	}
}
