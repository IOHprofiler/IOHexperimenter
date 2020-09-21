/// \file f_sphere.hpp
/// \brief cpp file for class Sphere.
///
/// A detailed file description.
/// Refer "https://github.com/numbbo/coco/blob/master/code-experiments/src/f_sphere.c"
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
			class Sphere : public bbob_base
			{
			public:
				Sphere(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: bbob_base(1, "Sphere", instance_id)
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
				}

				double internal_evaluate(const std::vector<double>& x)
				{
					double result = 0.0;
					for (size_t i = 0; i < x.size(); ++i)
					{
						result += x[i] * x[i];
					}
					return result;
				}

				static Sphere* createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
				{
					return new Sphere(instance_id, dimension);
				}
			};
		}
	}
}

