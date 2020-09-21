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
					: bbob_base(3, "Rastrigin", instance_id)
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

				double internal_evaluate(const std::vector<double>& x) override
				{
					size_t n = x.size();
					double sum1 = 0.0, sum2 = 0.0;

					for (size_t i = 0; i < n; ++i)
					{
						sum1 += cos(2.0 * transformation::coco::coco_pi * x[i]);
						sum2 += x[i] * x[i];
					}
					/* double check std::isinf*/
					if (std::isinf(sum2))
					{
						return sum2;
					}
					return 10.0 * (static_cast<double>(static_cast<long>(n)) - sum1) + sum2;
				}

				static Rastrigin* createInstance(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
				{
					return new Rastrigin(instance_id, dimension);
				}
			};
		}
	}
}
