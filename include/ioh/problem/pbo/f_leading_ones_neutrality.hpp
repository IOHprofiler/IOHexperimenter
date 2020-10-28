/// \file f_leading_ones_neutrality.hpp
/// \brief cpp file for class f_leading_ones_neutrality.
///
/// This file implements a LeadingOnes problem with neutrality transformation method from w-model.
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
			class LeadingOnes_Neutrality : public pbo_base
			{
			public:
				LeadingOnes_Neutrality(int instance_id = IOH_DEFAULT_INSTANCE, int dimension = IOH_DEFAULT_DIMENSION)
					: pbo_base("LeadingOnes_Neutrality", instance_id)
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
						if (new_variables[i] == 1)
						{
							result = i + 1;
						}
						else
						{
							break;
						}
					}
					return static_cast<double>(result);
				}

				static LeadingOnes_Neutrality* create(int instance_id = IOH_DEFAULT_INSTANCE,
				                                      int dimension = IOH_DEFAULT_DIMENSION)
				{
					return new LeadingOnes_Neutrality(instance_id, dimension);
				}
			};
		}
	}
}
