/// \file f_one_max_ruggedness1.hpp
/// \brief cpp file for class f_one_max_ruggedness1.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#pragma once
#include "pbo_base.hpp"
#include "ioh/problem/utils.hpp"

namespace ioh
{
	namespace problem
	{
		namespace pbo
		{
			class OneMax_Ruggedness1 : public pbo_base
			{
			public:

				OneMax_Ruggedness1(int instance_id = IOH_DEFAULT_INSTANCE, int dimension = IOH_DEFAULT_DIMENSION)
					: pbo_base("OneMax_Ruggedness1", instance_id)
				{
					set_best_variables(1);
					set_number_of_variables(dimension);
				}

				double internal_evaluate(const std::vector<int>& x) override
				{
					auto n = x.size();
					auto result = 0.0;
					for (auto i = 0; i != n; ++i)
					{
						result += x[i];
					}
					return utils::ruggedness1(result, static_cast<int>(n));
				}

				static OneMax_Ruggedness1* create(int instance_id = IOH_DEFAULT_INSTANCE,
				                                  int dimension = IOH_DEFAULT_DIMENSION)
				{
					return new OneMax_Ruggedness1(instance_id, dimension);
				}
			};
		}
	}
}
