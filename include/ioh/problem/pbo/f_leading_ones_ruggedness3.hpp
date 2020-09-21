/// \file f_leading_ones_ruggedness3.hpp
/// \brief cpp file for class f_leading_ones_ruggedness3.
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
			class LeadingOnes_Ruggedness3 : public pbo_base
			{
			public:
				std::vector<double> info;

				LeadingOnes_Ruggedness3(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: pbo_base("LeadingOnes_Ruggedness3", instance_id)
				{
					set_best_variables(1);
					set_number_of_variables(dimension);
				}


				void prepare_problem() override
				{
					info = utils::ruggedness3(get_number_of_variables());
				}

				double internal_evaluate(const std::vector<int>& x) override
				{
					int n = x.size();
					int result = 0;
					for (int i = 0; i != n; ++i)
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
					result = this->info[static_cast<int>(result + 0.5)];
					return static_cast<double>(result);
				}

				static LeadingOnes_Ruggedness3* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                               int dimension = DEFAULT_DIMENSION)
				{
					return new LeadingOnes_Ruggedness3(instance_id, dimension);
				}
			};
		};
	}
}
