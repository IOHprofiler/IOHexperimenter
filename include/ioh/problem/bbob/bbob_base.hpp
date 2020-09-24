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
			protected:
				long rseed_;
				double fopt_;
				std::vector<double> xopt_;
				std::vector<std::vector<double>> M_;
				std::vector<double> b_;
				std::vector<std::vector<double>> rot1_;
				std::vector<std::vector<double>> rot2_;
				const double lower_bound_ = -5.0;
				const double upper_bound_ = 5.0;
				int n_;
			public:
				bbob_base() = default;

				bbob_base(const int problem_id, const std::string& problem_name,
				          const int instance_id = DEFAULT_INSTANCE, const int dimension = DEFAULT_DIMENSION):
					rseed_((problem_id == 4 || problem_id == 18 ? problem_id - 1 : problem_id) + 10000 * instance_id),
					fopt_(transformation::coco::bbob2009_compute_fopt(problem_id, instance_id)),
					xopt_(dimension),
					M_(dimension, std::vector<double>(dimension)),
					b_(dimension),
					rot1_(dimension, std::vector<double>(dimension)),
					rot2_(dimension, std::vector<double>(dimension))			
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
					const auto n = this->get_number_of_variables();
					// This is the only portion of the function that is variable
					prepare_bbob_problem(xopt_, M_, b_, rot1_, rot2_, rseed_, n);
					
				}

				void objectives_transformation(const std::vector<double>& x, std::vector<double>& y,
				                               const int transformation_id, const int instance_id) override
				{
					transformation::coco::transform_obj_shift_evaluate_function(y, fopt_);
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
