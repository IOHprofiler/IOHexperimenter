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
			  /**
				 * \brief Construct a new LeadingOnes_Ruggedness3 object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
				 * 
				 * \param instance_id The instance number of a problem, which controls the transformation
				 * performed on the original problem.
				 * \param dimension The dimensionality of the problem to created, 4 by default.
				 **/
				LeadingOnes_Ruggedness3(int instance_id = IOH_DEFAULT_INSTANCE, int dimension = IOH_DEFAULT_DIMENSION)
					: pbo_base(17, "LeadingOnes_Ruggedness3", instance_id)
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
					auto result = 0;
					for (auto i = 0; i != x.size(); ++i)
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
					return this->info[static_cast<int>(result + 0.5)];
				}

				static LeadingOnes_Ruggedness3* create(int instance_id = IOH_DEFAULT_INSTANCE,
				                                       int dimension = IOH_DEFAULT_DIMENSION)
				{
					return new LeadingOnes_Ruggedness3(instance_id, dimension);
				}
			};
		};
	}
}
