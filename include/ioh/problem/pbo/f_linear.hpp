/// \file f_linear.hpp
/// \brief cpp file for class f_linear.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#pragma once
#include "pbo_base.hpp"

namespace ioh
{
	namespace problem
	{
		namespace pbo
		{
			class Linear : public pbo_base
			{
			public:
				std::vector<double> info;

				Linear(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: pbo_base("Linear", instance_id)
				{
					set_best_variables(1);
					set_number_of_variables(dimension);
				}

				double internal_evaluate(const std::vector<int>& x) override
				{
					auto n = x.size();
					double result = 0;
					for (auto i = 0; i < n; ++i)
					{
						result += static_cast<double>(x[i]) * static_cast<double>(i + 1);
					}
					return static_cast<double>(result);
				}

				static Linear* create(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
				{
					return new Linear(instance_id, dimension);
				}
			};
		};
	}
}
