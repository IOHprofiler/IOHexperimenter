/// \file f_one_max_dummy2.hpp
/// \brief cpp file for class f_one_max_dummy2.
///
/// This file implements a OneMax problem with reduction of dummy variables.
/// The reduction rate is chosen as 0.9.
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
			class OneMax_Dummy2 : public pbo_base
			{
			public:
				std::vector<int> info;
			  /**
				 * \brief Construct a new OneMax_Dummy2 object. Definition refers to https://doi.org/10.1016/j.asoc.2019.106027
				 * 
				 * \param instance_id The instance number of a problem, which controls the transformation
				 * performed on the original problem.
				 * \param dimension The dimensionality of the problem to created, 4 by default.
				 **/
				OneMax_Dummy2(int instance_id = IOH_DEFAULT_INSTANCE, int dimension = IOH_DEFAULT_DIMENSION)
					: pbo_base(5, "OneMax_Dummy2", instance_id)
				{
					set_best_variables(1);
					set_number_of_variables(dimension);
				}

				void prepare_problem() override
				{
					info = utils::dummy(get_number_of_variables(), 0.9, 10000);
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

				static OneMax_Dummy2* create(int instance_id = IOH_DEFAULT_INSTANCE, int dimension = IOH_DEFAULT_DIMENSION)
				{
					return new OneMax_Dummy2(instance_id, dimension);
				}
			};
		}
	}
}
