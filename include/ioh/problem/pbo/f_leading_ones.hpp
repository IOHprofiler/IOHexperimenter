/// \file f_leading_ones.hpp
/// \brief cpp file for class f_leading_ones.
///
/// A detailed file description.
///
/// \author Furong Ye
#pragma once
#include "pbo_base.hpp"

namespace ioh
{
	namespace problem
	{
		namespace pbo
		{
			class LeadingOnes : public pbo_base
			{
			public:
				LeadingOnes(int instance_id = IOH_DEFAULT_INSTANCE, int dimension = IOH_DEFAULT_DIMENSION)
					: pbo_base("LeadingOnes", instance_id)
				{
					set_best_variables(1);
					set_number_of_variables(dimension);
				}

				double internal_evaluate(const std::vector<int>& x) override
				{
					auto n = x.size();
					auto result = 0;
					for (auto i = 0; i != n; ++i)
					{
						if (x[i] == 1)
							result = i + 1;
						else
							break;
					}
					return static_cast<double>(result);
				}

				static LeadingOnes* create(int instance_id = IOH_DEFAULT_INSTANCE, int dimension = IOH_DEFAULT_DIMENSION)
				{
					return new LeadingOnes(instance_id, dimension);
				}
			};
		}
	}
}
