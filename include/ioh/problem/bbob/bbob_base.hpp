#pragma once

#include "ioh/problem/base.hpp"
#include "ioh/transformation.hpp"

//TODO: move the weird data class, these are just members of this class
//TODO: move create instance here
//TODO: create generic function for similar functions
namespace ioh
{
	namespace problem
	{
		namespace bbob
		{
			class bbob_base : public base<double>
			{
			public:
				bbob_base(int problem_id, std::string problem_name, int instance_id = DEFAULT_INSTANCE)
				{
					set_instance_id(instance_id);
					set_problem_id(problem_id);
					set_problem_name(problem_name);
					set_problem_type("bbob");
					set_number_of_objectives(1);
					set_lowerbound(-5.0);
					set_upperbound(5.0);
					set_best_variables(0);
					set_as_minimization();
				}

				void prepare_problem() override
				{
					using namespace transformation::coco;
					const long rseed = static_cast<long>(this->get_problem_id() + 10000 * this->get_instance_id());
					const int n = this->get_number_of_variables();

					std::vector<double> xopt(n);
					std::vector<std::vector<double>> M(n, std::vector<double>(n));
					std::vector<double> b(n);
					std::vector<std::vector<double>> rot1;
					std::vector<std::vector<double>> rot2;


					// This is the only portion of the function that is variable
					prepare_bbob_problem(xopt, M, b, rot1, rot2, rseed, n);

					// update static data
					data::fopt = bbob2009_compute_fopt(this->get_problem_id(), this->get_instance_id());
					data::xopt = xopt;
					data::M = M;
					data::b = b;
					data::lower_bound = -5.0;
					data::upper_bound = 5.0;
					data::rot1 = rot1;
					data::rot2 = rot2;
					data::rseed = rseed;
				}

				virtual void prepare_bbob_problem(std::vector<double>& xopt, std::vector<std::vector<double>>& M,
				                                  std::vector<double>& b, std::vector<std::vector<double>>& rot1,
				                                  std::vector<std::vector<double>>& rot2,
				                                  const long rseed, const long n
				)
				{
				}
			};
		}
	}
}
