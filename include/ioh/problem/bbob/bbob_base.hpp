#pragma once

#include "ioh/problem/base.hpp"

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
				std::vector<std::vector<double>> m_;
				std::vector<double> b_;
				std::vector<std::vector<double>> rot1_;
				std::vector<std::vector<double>> rot2_;
				const double lower_bound_ = -5.0;
				const double upper_bound_ = 5.0;
				int n_;
			public:
				bbob_base() = default;

				/**
				 * @brief Construct a new bbob base object
				 *
				 * @param problem_id The problem indentifier in range [1..24]
				 * @param problem_name The problem name which will set automatically in the derived classes
				 * @param instance_id The instance number of a problem, which controls the random operation (e.g., translation and rotatio)
				 * performed on the orginal function
				 * @param dimension The dimensionality of the problem to create, 2 by default.
				 */
				bbob_base(const int problem_id, const std::string& problem_name,
				          const int instance_id = IOH_DEFAULT_INSTANCE, const int dimension = IOH_DEFAULT_DIMENSION):
					rseed_((problem_id == 4 || problem_id == 18 ? problem_id - 1 : problem_id) + 10000 * instance_id),
					fopt_(transformation::coco::bbob2009_compute_fopt(problem_id, instance_id)),
					xopt_(dimension),
					m_(dimension, std::vector<double>(dimension)),
					b_(dimension),
					rot1_(dimension, std::vector<double>(dimension)),
					rot2_(dimension, std::vector<double>(dimension)),
					n_(dimension)
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

				/**
				 * @brief Shift the objective value by `this->fopt_`,
				 * which is to ensure the global optimal of this problem is exactly `this->fopt_`.
				 *
				 * @param x do we need this parameter?
				 * @param y
				 * @param transformation_id do we need this parameter?
				 * @param instance_id do we need this parameter?
				 */
				void objectives_transformation(const std::vector<double>& x, std::vector<double>& y,
				                               const int transformation_id, const int instance_id) override
				{
					transformation::coco::transform_obj_shift_evaluate_function(y, fopt_);
				}
			};
		}
	}
}
