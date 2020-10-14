/// \file f_one_max_ruggedness3.hpp
/// \brief cpp file for class f_one_max_ruggedness3.
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
			class OneMax_Ruggedness3 : public pbo_base
			{
			public:
				std::vector<double> info;

				OneMax_Ruggedness3(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: pbo_base("OneMax_Ruggedness3", instance_id)
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
					auto n = x.size();
					auto result = 0;
					for (auto i = 0; i != n; ++i)
					{
						result += x[i];
					}
					return this->info[static_cast<int>(result + 0.5)];
				}

				static OneMax_Ruggedness3* create(int instance_id = DEFAULT_INSTANCE,
				                                          int dimension = DEFAULT_DIMENSION)
				{
					return new OneMax_Ruggedness3(instance_id, dimension);
				}
			};
		}
	}
}
