/// \file f_rosenbrock.hpp
/// \brief cpp file for class Rosenbrock.
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
			class Rosenbrock : public bbob_base
			{
				std::vector<double> minus_one_;
				double factor_;
				
			public:
				Rosenbrock(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(8, "Rosenbrock", instance_id, dimension),
					minus_one_(dimension, -1),
					factor_(std::max(1.0, std::sqrt(dimension) / 8.0))
				{
					set_number_of_variables(dimension);
				}

				void prepare_problem() override
				{
					transformation::coco::bbob2009_compute_xopt(xopt_, rseed_, n_);
					for (auto i = 0; i < n_; ++i)
						xopt_[i] *= 0.75;
				}

				double internal_evaluate(const std::vector<double>& x) override
				{
					double s1 = 0.0, s2 = 0.0, tmp;
					for (size_t i = 0; i < n_ - 1; ++i)
					{
						tmp = (x[i] * x[i] - x[i + 1]);
						s1 += tmp * tmp;
						tmp = (x[i] - 1.0);
						s2 += tmp * tmp;
					}
					return 100.0 * s1 + s2;
				}

				void variables_transformation(std::vector<double>& x, const int transformation_id,
					const int instance_id) override
				{
					transformation::coco::transform_vars_shift_evaluate_function(x, xopt_);
					transformation::coco::transform_vars_scale_evaluate(x, factor_);
					transformation::coco::transform_vars_shift_evaluate_function(x, minus_one_);
				}

				static Rosenbrock* create(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
				{
					return new Rosenbrock(instance_id, dimension);
				}
			};
		}
	}
}
