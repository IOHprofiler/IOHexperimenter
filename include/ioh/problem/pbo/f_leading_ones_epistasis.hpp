/// \file f_leading_ones_epistasis.hpp
/// \brief cpp file for class f_leading_ones_epistasis.
///
/// This file implements a LeadingOnes problem with epistasis transformation method from w-model.
/// The parameter v is chosen as 4.
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
			class LeadingOnes_Epistasis : public pbo_base
			{
			public:
				LeadingOnes_Epistasis(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: pbo_base("LeadingOnes_Epistasis", instance_id)
				{
					set_number_of_variables(dimension);
				}

				double internal_evaluate(const std::vector<int>& x) override
				{
					std::vector<int> new_variables = utils::epistasis(x, 4);
					int n = new_variables.size();
					int result = 0;
					for (int i = 0; i != n; ++i)
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

				static LeadingOnes_Epistasis* createInstance(int instance_id = DEFAULT_INSTANCE,
				                                             int dimension = DEFAULT_DIMENSION)
				{
					return new LeadingOnes_Epistasis(instance_id, dimension);
				}
			};
		}
	}
}
