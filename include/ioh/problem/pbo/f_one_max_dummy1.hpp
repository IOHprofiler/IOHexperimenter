/// \file f_one_max_dummy1.hpp
/// \brief cpp file for class f_one_max_dummy1.
///
/// This file implements a OneMax problem with reduction of dummy variables.
/// The reduction rate is chosen as 0.5.
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
			class OneMax_Dummy1 : public pbo_base
			{
			public:
				std::vector<int> info;

				OneMax_Dummy1(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
					: pbo_base("OneMax_Dummy1", instance_id)
				{
					set_best_variables(1);
					set_number_of_variables(dimension);
				}

				void prepare_problem() override
				{
					info = utils::dummy(get_number_of_variables(), 0.5, 10000);
				}

				double internal_evaluate(const std::vector<int>& x) override
				{
					auto n = this->info.size();
					auto result = 0.0;
					for (auto i = 0; i != n; ++i)
					{
						result += x[this->info[i]];
					}
					return result;
				}

				static OneMax_Dummy1* create(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION)
				{
					return new OneMax_Dummy1(instance_id, dimension);
				}
			};
		}
	}
}
