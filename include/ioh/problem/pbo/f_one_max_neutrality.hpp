/// \file f_one_max_neutrality.hpp
/// \brief cpp file for class f_one_max_neutrality.
///
/// This file implements a OneMax problem with neutrality transformation method from w-model.
/// The parameter mu is chosen as 3.
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
			class OneMax_Neutrality : public pbo_base
			{
			public:

				OneMax_Neutrality(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: pbo_base("OneMax_Neutrality", instance_id)
				{
					set_best_variables(1);
					set_number_of_variables(dimension);
				}

				double internal_evaluate(const std::vector<int>& x) override
				{
					auto new_variables = utils::neutrality(x, 3);
					auto n = new_variables.size();
					auto result = 0;
					for (auto i = 0; i != n; ++i)
					{
						result += new_variables[i];
					}
					return static_cast<double>(result);
				}

				static OneMax_Neutrality* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                         int dimension = DEFAULT_DIMENSION)
				{
					return new OneMax_Neutrality(instance_id, dimension);
				}
			};
		}
	}
}
