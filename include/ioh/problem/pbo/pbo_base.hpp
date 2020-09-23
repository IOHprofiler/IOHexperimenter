#pragma once

#include "ioh/problem/base.hpp"

namespace ioh
{
	namespace problem
	{
		namespace pbo
		{
			class pbo_base : public base<int>
			{
			public:
				pbo_base() = default;
				pbo_base(std::string problem_name, int instance_id = DEFAULT_INSTANCE)
				{
					set_instance_id(instance_id);
					set_problem_name(problem_name);
					set_problem_type("pseudo_Boolean_problem");
					set_number_of_objectives(1);
					set_lowerbound(0);
					set_upperbound(1);					
				}
			};
		}
	}
}