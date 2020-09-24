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
				double lower_bound_;
				double upper_bound_;
			public:
				bbob_base() = default;
				bbob_base(const int problem_id, const std::string& problem_name, const int instance_id = DEFAULT_INSTANCE):
				fopt_(0.0)
				
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
					rseed_ = (problem_id == 4 || problem_id == 18 ? problem_id - 1 : problem_id) + 10000 * instance_id;
				}

				void prepare_problem() override
				{
					using namespace transformation::coco;
					const auto n = this->get_number_of_variables();

					std::vector<double> xopt(n);
					std::vector<std::vector<double>> M(n, std::vector<double>(n));
					std::vector<double> b(n);
					std::vector<std::vector<double>> rot1;
					std::vector<std::vector<double>> rot2;


					// This is the only portion of the function that is variable
					prepare_bbob_problem(xopt, M, b, rot1, rot2, rseed_, n);

					// update static data
					data::fopt = bbob2009_compute_fopt(this->get_problem_id(), this->get_instance_id());
					data::xopt = xopt;
					data::M = M;
					data::b = b;
					data::lower_bound = -5.0;  
					data::upper_bound = 5.0;
					data::rot1 = rot1;
					data::rot2 = rot2;
					data::rseed = rseed_;

					fopt_ = data::fopt;
					xopt_ = xopt;
					M_ = M;
					b_ = b;
					lower_bound_ = data::lower_bound;
					upper_bound_ = data::upper_bound;
					rot1_ = rot1;
					rot2_ = rot2;				
				}

				void objectives_transformation(const std::vector<double>& x, std::vector<double>& y,
					const int transformation_id, const int instance_id) override
				{
					transformation::coco::coco_tranformation_objs(x, y, transformation_id);
				}

				void variables_transformation(std::vector<double>& x, const int transformation_id,
					const int instance_id) override
				{
					transformation::coco::coco_tranformation_vars(x, transformation_id);
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
