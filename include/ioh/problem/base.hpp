#pragma once

#include <vector>
#include <string>
#include <limits>
#include <numeric>

#include "ioh/common.hpp"
#include "ioh/transformation.hpp"


namespace ioh
{
	namespace problem
	{
		/// \brief A base class for defining problems.
		///
		/// Basic structure for IOHExperimenter, which is used for generating benchmark problems.
		/// To define a new problem, the 'internal_evaluate' method must be defined. The problem
		/// sets as maximization by default. If the 'best_variables' are given, the optimal of
		/// the problem will be calculated with the 'best_variables'; or you can set the optimal
		/// by defining the 'customized_optimal' function; otherwise, the optimal is set as 
		/// min()(max()) for maximization(minimization). If additional calculation is needed by
		/// 'internal_evaluate', you can configure it in 'prepare_problem()'.
		template <class InputType>
		class base
		{
			int problem_id; /// < problem id, assigned as being added into a suite.
			int instance_id;
			/// < evaluate function is validated with instance and dimension. set default to avoid invalid class.

			std::string problem_name;
			std::string problem_type; /// todo. make it as enum.

			common::optimization_type maximization_minimization_flag;

			int number_of_variables;
			/// < evaluate function is validated with instance and dimension. set default to avoid invalid class.
			int number_of_objectives;

			std::vector<InputType> lowerbound;
			std::vector<InputType> upperbound;

			std::vector<InputType> best_variables; /// todo. comments, rename?
			std::vector<InputType> best_transformed_variables;
			std::vector<double> optimal;
			/// todo. How to evluate distance to optima. In global optima case, which optimum to be recorded.
			bool optimalFound;

			std::vector<double> raw_objectives; /// < to record objectives before transformation.
			std::vector<double> transformed_objectives; /// < to record objectives after transformation.
			int transformed_number_of_variables; /// < intermediate variables in evaluate.
			std::vector<InputType> transformed_variables; /// < intermediate variables in evaluate.

			/// todo. constrainted optimization.
			/// std::size_t number_of_constraints;

			int evaluations; /// < to record optimization process. 
			std::vector<double> best_so_far_raw_objectives; /// < to record optimization process.
			int best_so_far_raw_evaluations; /// < to record optimization process.
			std::vector<double> best_so_far_transformed_objectives; /// < to record optimization process.
			int best_so_far_transformed_evaluations; /// < to record optimization process.


		public:
			base(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) :
				problem_id(DEFAULT_PROBLEM_ID),
				instance_id(instance_id),
				maximization_minimization_flag(common::optimization_type::maximization),
				number_of_variables(dimension),
				number_of_objectives(1),
				lowerbound(std::vector<InputType>(number_of_variables)),
				upperbound(std::vector<InputType>(number_of_variables)),
				optimal(
					/* std::vector<double>(number_of_objectives) <- do not initialise, so as to check for the exetrn initialization using size */),
				optimalFound(false),
				raw_objectives(std::vector<double>(number_of_objectives)),
				transformed_objectives(std::vector<double>(number_of_objectives)),
				transformed_number_of_variables(0),
				evaluations(0),
				best_so_far_raw_objectives(std::vector<double>(number_of_objectives)),
				best_so_far_raw_evaluations(0),
				best_so_far_transformed_objectives(std::vector<double>(number_of_objectives)),
				best_so_far_transformed_evaluations(0)
			{
			}

			base(const base&) = delete;
			base& operator=(const base&) = delete;

			/// \todo to support multi-objective optimization
			/// \fn virtual std::vector<double> internal_evaluate_multi(std::vector<InputType> x)
			/// \brief A virtual internal evaluate function.
			///
			/// The internal_evaluate function is to be used in evaluate function.
			/// This function must be decalred in derived function of new problems.
			// virtual std::vector<double> internal_evaluate_multi (const std::vector<InputType> &x) {
			//   std::vector<double> result;
			//   std::cout << "No multi evaluate function defined" << std::endl;
			//   return result;
			// };

			/// \fn double internal_evaluate(std::vector<InputType> x)
			/// \brief A virtual internal evaluate function.
			///
			/// The internal_evaluate function is to be used in evaluate function.
			/// This function must be decalred in derived function of new problems.
			virtual double internal_evaluate(const std::vector<InputType>& x)
			{
				constexpr auto result = std::numeric_limits<double>::lowest();
				common::log::warning("No evaluate function defined");
				return result;
			}

			virtual void prepare_problem()
			{
			}

			/// \todo to support multi-objective optimization
			/// \fn std::vector<double> evevaluate_multialuate(std::vector<InputType> x)
			/// \brife A common function for evaluating fitness of problems.
			///
			/// Raw evaluate process, tranformation operations, and logging process are excuted 
			/// in this function.
			/// \param x A InputType vector of variables.
			/// \return A double vector of objectives.
			// std::vector<double> evaluate_multi(std::vector<InputType> x) {
			//   ++this->evaluations;

			//   transformation.variables_transformation(x,this->problem_id,this->instance_id,this->problem_type);
			//   this->raw_objectives = internal_evaluate_multi(x);

			//   this->transformed_objectives = this->raw_objectives;
			//   transformation.objectives_transformation(x,this->transformed_objectives,this->problem_id,this->instance_id,this->problem_type);
			//   if (compareObjectives(this->transformed_objectives,this->best_so_far_transformed_objectives,this->maximization_minimization_flag)) {
			//     this->best_so_far_transformed_objectives = this->transformed_objectives;
			//     this->best_so_far_transformed_evaluations = this->evaluations;
			//     this->best_so_far_raw_objectives = this->raw_objectives;
			//     this->best_so_far_raw_evaluations = this->evaluations;

			//   }

			//   if (compareVector(this->transformed_objectives,this->optimal)) {
			//     this->optimalFound = true;
			//   }

			//   return this->transformed_objectives;
			// }

			/// \fn double evaluate(std::vector<InputType> x)
			/// \brife A common function for evaluating fitness of problems.
			///
			/// Raw evaluate process, tranformation operations, and logging process are excuted 
			/// in this function.
			/// \param x A InputType vector of variables.
			/// \return A double vector of objectives.
			double evaluate(std::vector<InputType> x)
			{
				assert(this->raw_objectives.size() >= 1);
				assert(this->transformed_objectives.size() == this->raw_objectives.size());

				++this->evaluations;

				if (x.size() != this->number_of_variables)
				{
					common::log::warning("The dimension of solution is incorrect.");
					if (this->maximization_minimization_flag == common::optimization_type::maximization)
					{
						this->raw_objectives[0] = std::numeric_limits<double>::lowest();
						this->transformed_objectives[0] = std::numeric_limits<double>::lowest();
					}
					else
					{
						this->raw_objectives[0] = std::numeric_limits<double>::max();
						this->transformed_objectives[0] = std::numeric_limits<double>::max();
					}
					return this->transformed_objectives[0];
				}


				
				variables_transformation(x, problem_id, instance_id);

				this->raw_objectives[0] = this->internal_evaluate(x);
				this->transformed_objectives[0] = this->raw_objectives[0];

				objectives_transformation(x, transformed_objectives, problem_id, instance_id);

				if (common::compare_objectives(this->transformed_objectives, this->best_so_far_transformed_objectives,
				                               this->maximization_minimization_flag))
				{
					this->best_so_far_transformed_objectives = this->transformed_objectives;
					this->best_so_far_transformed_evaluations = this->evaluations;
					this->best_so_far_raw_objectives = this->raw_objectives;
					this->best_so_far_raw_evaluations = this->evaluations;
				}

				if (common::compare_vector(this->transformed_objectives, this->optimal))
				{
					this->optimalFound = true;
				}
				return this->transformed_objectives[0];
			}

			virtual void objectives_transformation(const std::vector<InputType>& x, std::vector<double>& y,
			                                       const int transformation_id, const int instance_id)
			{
			}

			virtual void variables_transformation(std::vector<InputType>& x, const int transformation_id,
			                                      const int instance_id)
			{
			}

			/// \fn virtual void customized_optimal()
			///
			/// A virtual function to customize optimal of the problem.
			virtual void customize_optimal()
			{
			}

			/// \fn void calc_optimal()
			///
			/// A function to calculate optimal of the problem.
			/// It will be invoked after setting dimension (number_of_variables) or instance_id.
			void calc_optimal()
			{
				if (this->best_variables.size() == this->number_of_variables)
				{
					/// todo. Make Exception.
					/// Do not apply transformation on best_variables as calculating optimal
					if (this->number_of_objectives == 1)
					{
						/// This only works for F4, F16-18, and F23 of BBOB suite.
						// if (this->problem_type == "bbob")
						// {
						// 	transformation::coco::data::raw_x.clear();
						// 	for (std::size_t i = 0; i != this->best_variables.size(); ++i)
						// 	{
						// 		transformation::coco::data::raw_x.push_back(this->best_variables[i]);
						// 	}
						// }
						this->optimal[0] = internal_evaluate(this->best_variables);
					}
					else
					{
						common::log::warning("Multi-objectives optimization is not supported now.");
					}
					objectives_transformation(this->best_variables, this->optimal,
						this->problem_id, this->instance_id);
				}
				else
				{
					this->optimal.clear();
					for (std::size_t i = 0; i < this->number_of_objectives; ++i)
					{
						if (this->maximization_minimization_flag == common::optimization_type::maximization)
						{
							this->optimal.push_back(std::numeric_limits<double>::max());
						}
						else
						{
							this->optimal.push_back(std::numeric_limits<double>::lowest());
						}
					}
					customize_optimal();
				}
			}

			/// \todo  To support constrained optimization.
			// virtual std::vector<double> constraints() {
			//   std::vector<double> con;
			//   printf("No constraints function defined\n");
			//   return con;
			// };

			/// \fn void reset_problem()
			///
			/// \brief Reset problem as the default condition before doing evaluating.
			void reset_problem()
			{
				this->evaluations = 0;
				this->best_so_far_raw_evaluations = 0;
				this->best_so_far_transformed_evaluations = 0;
				this->optimalFound = false;
				for (size_t i = 0; i != this->number_of_objectives; ++i)
				{
					if (this->maximization_minimization_flag == common::optimization_type::maximization)
					{
						this->best_so_far_raw_objectives[i] = std::numeric_limits<double>::lowest();
						this->best_so_far_transformed_objectives[i] = std::numeric_limits<double>::lowest();
					}
					else
					{
						this->best_so_far_raw_objectives[i] = std::numeric_limits<double>::max();
						this->best_so_far_transformed_objectives[i] = std::numeric_limits<double>::max();
					}
				}
				this->prepare_problem();
				this->calc_optimal();
			}

			/// \fn std::vector<std::variant<int,double,std::string>> loggerInfo()
			///
			/// Return a vector logger_info may be used by loggers.
			/// logger_info[0] evaluations
			/// logger_info[1] precision
			/// logger_info[2] best_so_far_precision
			/// logger_info[3] transformed_objective
			/// logger_info[4] best_so_far_transformed_objectives
			std::vector<double> loggerCOCOInfo() const
			{
				std::vector<double> logger_info(5);
				logger_info[0] = static_cast<double>(this->evaluations);
				logger_info[1] = this->transformed_objectives[0] - this->optimal[0];
				logger_info[2] = this->best_so_far_transformed_objectives[0] - this->optimal[0];
				logger_info[3] = this->transformed_objectives[0];
				logger_info[4] = this->best_so_far_transformed_objectives[0];
				return logger_info;
			}

			/// \fn std::vector<std::variant<int,double,std::string>> loggerInfo()
			///
			/// Return a vector logger_info may be used by loggers.
			/// logger_info[0] evaluations
			/// logger_info[1] raw_objectives
			/// logger_info[2] best_so_far_raw_objectives
			/// logger_info[3] transformed_objective
			/// logger_info[4] best_so_far_transformed_objectives
			std::vector<double> loggerInfo() const
			{
				std::vector<double> logger_info(5);
				logger_info[0] = static_cast<double>(this->evaluations);
				logger_info[1] = this->raw_objectives[0];
				logger_info[2] = this->best_so_far_raw_objectives[0];
				logger_info[3] = this->transformed_objectives[0];
				logger_info[4] = this->best_so_far_transformed_objectives[0];
				return logger_info;
			}

			/// \fn hit_optimal()
			///
			/// \brief Detect if the optimal have been found.
			bool hit_optimal() const
			{
				return this->optimalFound;
			};

			int get_problem_id() const
			{
				return this->problem_id;
			}

			void set_problem_id(int problem_id)
			{
				this->problem_id = problem_id;
			}

			int get_instance_id() const
			{
				return this->instance_id;
			}

			/// \fn set_instance_id(int instance_id)
			///
			/// To set instance_id of the problem. Since the optimal will be updated
			/// as instanced_id updated, calc_optimal() is revoked here.
			/// \param instance_id 
			void set_instance_id(int instance_id)
			{
				this->instance_id = instance_id;
				this->prepare_problem();
				this->calc_optimal();
			}

			std::string get_problem_name() const
			{
				return this->problem_name;
			}

			void set_problem_name(std::string problem_name)
			{
				this->problem_name = problem_name;
			}

			std::string get_problem_type() const
			{
				return this->problem_type;
			}

			void set_problem_type(std::string problem_type)
			{
				this->problem_type = problem_type;
			}

			std::vector<InputType> get_lowerbound() const
			{
				return this->lowerbound;
			}

			void set_lowerbound(InputType lowerbound)
			{
				std::vector<InputType>().swap(this->lowerbound);
				this->lowerbound.reserve(this->number_of_variables);
				for (std::size_t i = 0; i < this->number_of_variables; ++i)
				{
					this->lowerbound.push_back(lowerbound);
				}
			}

			void set_lowerbound(const std::vector<InputType>& lowerbound)
			{
				this->lowerbound = lowerbound;
			}

			std::vector<InputType> get_upperbound() const
			{
				return this->upperbound;
			}

			void set_upperbound(InputType upperbound)
			{
				std::vector<InputType>().swap(this->upperbound);
				this->upperbound.reserve(this->number_of_variables);
				for (std::size_t i = 0; i < this->number_of_variables; ++i)
				{
					this->upperbound.push_back(upperbound);
				}
			}

			void set_upperbound(const std::vector<InputType>& upperbound)
			{
				this->upperbound = upperbound;
			}

			int get_number_of_variables() const
			{
				return this->number_of_variables;
			}

			/// \fn set_number_of_variables(int number_of_variables)
			/// 
			/// To set number_of_variables of the problem. When the number_of_variables is updated,
			/// best_variables, lowerbound, upperbound, and optimal need to be updated as well.
			///
			/// \param number_of_variables
			void set_number_of_variables(int number_of_variables)
			{
				this->number_of_variables = number_of_variables;
				if (this->best_variables.size() != 0)
				{
					this->set_best_variables(this->best_variables[0]);
				}
				if (this->lowerbound.size() != 0)
				{
					this->set_lowerbound(this->lowerbound[0]);
				}
				if (this->upperbound.size() != 0)
				{
					this->set_upperbound(this->upperbound[0]);
				}
				this->prepare_problem();
				this->calc_optimal();
			}

			/// \fn set_number_of_variables(int number_of_variables)
			/// 
			/// To set number_of_variables of the problem. When the number_of_variables is updated,
			/// best_variables, lowerbound, upperbound, and optimal need to be updated as well. In case 
			/// the best value for each bit is not staic, another input 'best_variables' is supplied.
			///
			/// \param number_of_variables, best_variables
			void set_number_of_variables(int number_of_variables,
			                             const std::vector<InputType>& best_variables)
			{
				this->number_of_variables = number_of_variables;
				this->best_variables = best_variables;
				if (this->lowerbound.size() != 0)
				{
					this->set_lowerbound(this->lowerbound[0]);
				}
				if (this->upperbound.size() != 0)
				{
					this->set_upperbound(this->upperbound[0]);
				}
				this->prepare_problem();
				this->calc_optimal();
			}

			int get_number_of_objectives() const
			{
				return this->number_of_objectives;
			}

			void set_number_of_objectives(int number_of_objectives)
			{
				this->number_of_objectives = number_of_objectives;
				this->raw_objectives = std::vector<double>(this->number_of_objectives);
				this->transformed_objectives = std::vector<double>(this->number_of_objectives);
				if (this->maximization_minimization_flag == common::optimization_type::maximization)
				{
					this->best_so_far_raw_objectives = std::vector<double>(
						this->number_of_objectives, std::numeric_limits<double>::lowest());
					this->best_so_far_transformed_objectives = std::vector<double>(
						this->number_of_objectives, std::numeric_limits<double>::lowest());
				}
				else
				{
					this->best_so_far_raw_objectives = std::vector<double>(
						this->number_of_objectives, std::numeric_limits<double>::max());
					this->best_so_far_transformed_objectives = std::vector<double>(
						this->number_of_objectives, std::numeric_limits<double>::max());
				}
				this->optimal = std::vector<double>(this->number_of_objectives);
			}

			std::vector<double> get_raw_objectives() const
			{
				return this->raw_objectives;
			}

			std::vector<double> get_transformed_objectives() const
			{
				return this->transformed_objectives;
			}

			int get_transformed_number_of_variables() const
			{
				return this->transformed_number_of_variables;
			}

			std::vector<InputType> get_transformed_variables() const
			{
				return this->transformed_variables;
			}

			std::vector<InputType> get_best_variables() const
			{
				return this->best_variables;
			}

			void set_best_variables(InputType best_variables)
			{
				this->best_variables.clear();
				for (std::size_t i = 0; i < this->number_of_variables; ++i)
				{
					this->best_variables.push_back(best_variables);
				}
			}

			void set_best_variables(const std::vector<InputType>& best_variables)
			{
				this->best_variables = best_variables;
			}

			bool has_optimal() const
			{
				return this->optimal.size() == this->get_number_of_objectives();
			}

			std::vector<double> get_optimal() const
			{
				// FIXME unsure if one want to raise an exception in Release mode also?
				// Assert that the optimum have been initialized.
				assert(this->has_optimal());
				return this->optimal;
			}

			void set_optimal(double optimal)
			{
				std::vector<double>().swap(this->optimal);
				this->optimal.reserve(this->number_of_objectives);
				for (std::size_t i = 0; i < this->number_of_objectives; ++i)
				{
					this->optimal.push_back(optimal);
				}
			}

			void set_optimal(const std::vector<double>& optimal)
			{
				this->optimal = optimal;
			}

			void evaluate_optimal(std::vector<InputType> best_variables)
			{
				this->optimal[0] = this->evaluate(best_variables);
			}

			void evaluate_optimal()
			{
				this->optimal[0] = this->evaluate(this->best_variables);
			}

			int get_evaluations() const
			{
				return this->evaluations;
			}

			std::vector<double> get_best_so_far_raw_objectives() const
			{
				return this->best_so_far_raw_objectives;
			}

			int get_best_so_far_raw_evaluations() const
			{
				return this->best_so_far_raw_evaluations;
			}

			std::vector<double> get_best_so_far_transformed_objectives() const
			{
				return this->best_so_far_transformed_objectives;
			}

			int get_best_so_far_transformed_evaluations() const
			{
				return this->best_so_far_transformed_evaluations;
			}

			common::optimization_type get_optimization_type() const
			{
				return this->maximization_minimization_flag;
			}

			void set_as_maximization()
			{
				this->maximization_minimization_flag = common::optimization_type::maximization;
				for (std::size_t i = 0; i != this->number_of_objectives; ++i)
				{
					this->best_so_far_raw_objectives[i] = std::numeric_limits<double>::lowest();
					this->best_so_far_transformed_objectives[i] = std::numeric_limits<double>::lowest();
				}
			}

			void set_as_minimization()
			{
				this->maximization_minimization_flag = common::optimization_type::minimization;
				for (std::size_t i = 0; i != this->number_of_objectives; ++i)
				{
					this->best_so_far_raw_objectives[i] = std::numeric_limits<double>::max();
					this->best_so_far_transformed_objectives[i] = std::numeric_limits<double>::max();
				}
			}
		};
	}
}
