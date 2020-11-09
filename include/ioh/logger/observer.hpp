#pragma once

#include "ioh/common.hpp"
#include "ioh/suite.hpp"


namespace ioh
{
	namespace logger
	{
		/// \brief A class of methods of setting triggers recording evaluations.
		///
		/// Four methods is introduced here:
		///   1. Recording evaluations by a static interval.
		///   2. Recording complete evaluations.
		///   3. Recording evaluations as the best found solution is updated.
		///   4. Recording evaluations at pre-defined points or/and with a static number for each exponential bucket.
		template <typename ProblemType>
		class Observer
		{
			int observer_interval; /// < variable for recording by a static interval.
			bool observer_complete_flag; /// < variable for recording complete optimization process. 
			bool observer_update_flag; /// < variable for recording when a better solution is found.

			std::vector<int> observer_time_points; /// < variables for recording at pre-defined points.
			size_t evaluations_value1; /// < intermediate variables for calculating points with 'observer_time_points'.
			size_t time_points_index; /// < intermediate variables for calculating points with 'observer_time_points'.
			int time_points_expi; /// < intermediate variables for calculating points with 'observer_time_points'.
			int observer_time_points_exp_base1;

			int observer_number_of_evaluations;
			/// < variables for recording with a pre-defined times in each exponential boxplot.
			size_t evaluations_value2;
			/// < intermediate variables for calculating points with 'observer_number_of_evaluations'.
			int evaluations_expi;
			/// < intermediate variables for calculating points with 'observer_number_of_evaluations'.
			int observer_time_points_exp_base2;

			/// \todo Currently this is only for single objective optimization.
			double current_best_fitness;

		public:
			/** API for subclasses @{ */
			using input_type = ProblemType;

			virtual void track_problem(const ProblemType& problem) = 0;

			virtual void track_suite(const suite::base<ProblemType>& suite) = 0;

			virtual void do_log(const std::vector<double>& log_info) = 0;
			/** }@ */


			Observer() :
				observer_interval(0),
				observer_complete_flag(false),
				observer_update_flag(true),
				observer_time_points({0}),
				evaluations_value1(1),
				time_points_index(0),
				time_points_expi(0),
				observer_time_points_exp_base1(10),
				observer_number_of_evaluations(0),
				evaluations_value2(1),
				evaluations_expi(0),
				observer_time_points_exp_base2(10)
			{
			}

			Observer(
				const bool observer_complete_flag,
				const int interval,
				const std::vector<int>& time_points,
				const int number_of_evaluations,
				const bool update_flag
			) :
				observer_interval(interval),
				observer_complete_flag(observer_complete_flag),
				observer_update_flag(update_flag),
				observer_time_points(time_points),
				evaluations_value1(1),
				time_points_index(0),
				time_points_expi(0),
				observer_time_points_exp_base1(10),
				observer_number_of_evaluations(number_of_evaluations),
				evaluations_value2(1),
				evaluations_expi(0),
				observer_time_points_exp_base2(10)
			{
			}

			virtual ~Observer()
			{
			}

			// Observer(const Observer&) = delete;
			// Observer& operator =(const Observer&) = delete;

			void set_complete_flag(const bool complete_flag)
			{
				observer_complete_flag = complete_flag;
			}

			bool complete_status() const
			{
				return observer_complete_flag;
			}

			bool complete_trigger() const
			{
				return observer_complete_flag;
			}

			void set_interval(const int interval)
			{
				observer_interval = interval;
			}

			bool interval_status() const
			{
				return observer_interval != 0;
			}

			bool interval_trigger(const size_t evaluations) const
			{
				return observer_interval != 0 && (evaluations == 1 || evaluations % observer_interval == 0);
			}

			void set_update_flag(const bool update_flag)
			{
				observer_update_flag = update_flag;
			}

			bool update_status() const
			{
				return observer_update_flag;
			}

			bool update_trigger(const double fitness, const common::OptimizationType optimization_type)
			{
				if (observer_update_flag && compare_objectives(fitness, current_best_fitness, optimization_type))
				{
					current_best_fitness = fitness;
					return true;
				}
				return false;
			}

			void set_time_points(const std::vector<int>& time_points, const int number_of_evaluations,
			                     const int time_points_exp_base1 = 10, const int time_points_exp_base2 = 10)
			{
				observer_time_points = time_points;
				observer_number_of_evaluations = number_of_evaluations;
				observer_time_points_exp_base1 = time_points_exp_base1;
				observer_time_points_exp_base2 = time_points_exp_base2;
			}

			bool time_points_status() const
			{
				return observer_time_points.size() > 0
					&& !(observer_time_points.size() == 1 && observer_time_points[0] == 0)
					|| observer_number_of_evaluations > 0;
			}

			bool time_points_trigger(const size_t evaluations)
			{
				if (!time_points_status())
				{
					return false;
				}
				auto result = false;

				/// evaluations_values is to be set by 10^n * elements of observer_time_points.
				/// For example, observer_time_points = {1,2,5}, the trigger returns true at 1, 2, 5, 10*1, 10*2, 10*5, 100*1, 100*2, 100*5,... 
				if (evaluations == evaluations_value1)
				{
					result = true;
					if (time_points_index < observer_time_points.size() - 1)
					{
						++time_points_index;
					}
					else
					{
						time_points_index = 0;
						++time_points_expi;
					}
					this->evaluations_value1 = static_cast<size_t>(observer_time_points[time_points_index] *
						pow(
							static_cast<double>(observer_time_points_exp_base1),
							static_cast<double>(time_points_expi)));
					while (evaluations_value1 <= evaluations)
					{
						if (time_points_index < observer_time_points.size() - 1)
						{
							++time_points_index;
						}
						else
						{
							time_points_index = 0;
							++time_points_expi;
						}
						evaluations_value1 = static_cast<size_t>(observer_time_points[
							time_points_index] * pow(
							static_cast<double>(observer_time_points_exp_base1),
							static_cast<double>(time_points_expi)));
					}
				}

				/// evaluations_value2 = floor(10^(i/n), n is observer_number_of_evaluations.
				/// It maintains that in each [10^m, 10^(m+1)], there will be observer_number_of_evaluations evaluations are stored.
				if (evaluations == this->evaluations_value2)
				{
					while (static_cast<size_t>(floor(pow(observer_time_points_exp_base2,
					                                     static_cast<double>(evaluations_expi) / static_cast<
						                                     double>(observer_number_of_evaluations)))) <=
						evaluations_value2)
					{
						evaluations_expi++;
					}
					evaluations_value2 = static_cast<size_t>(floor(pow(observer_time_points_exp_base2,
					                                                   static_cast<double>(evaluations_expi) /
					                                                   static_cast<double>(
						                                                   observer_number_of_evaluations))));
					result = true;
				}
				return result;
			}

			void reset_observer(const common::OptimizationType optimization_type)
			{
				if (optimization_type == common::OptimizationType::maximization)
				{
					current_best_fitness = -std::numeric_limits<double>::infinity();
				}
				else
				{
					current_best_fitness = std::numeric_limits<double>::infinity();
				}
				evaluations_value1 = 1;
				time_points_index = 0;
				time_points_expi = 0;
				evaluations_value2 = 1;
				evaluations_expi = 0;
			}
		};
	}
}
