/// \file f_leading_ones_ruggedness2.hpp
/// \brief cpp file for class f_leading_ones_ruggedness2.
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
			class LeadingOnes_Ruggedness2 : public pbo_base
			{
			public:
				LeadingOnes_Ruggedness2(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: pbo_base("LeadingOnes_Ruggedness2", instance_id)
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
						if (x[i] == 1)
						{
							result = i + 1;
						}
						else
						{
							break;
						}
					}
					return utils::ruggedness2(result, static_cast<int>(n));
				}

				static LeadingOnes_Ruggedness2* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                               int dimension = DEFAULT_DIMENSION)
				{
					return new LeadingOnes_Ruggedness2(instance_id, dimension);
				}
			};
		}
	}
}
