/// \file observer_combine.h
/// \brief Header file for class observer_combine.
///
/// \author Johann Dreo

#pragma once

#include <vector>
#include "observer.hpp"


namespace ioh
{
	namespace logger
	{
		/** An observer that can hold several observers and call them all.
		 *
		 * Useful if one want to use several loggers without having to manage them
		 * separately.
		 *
		 * Example:
		 * @code
			BBOB_suite bench({1},{2},{3});
			using Input = BBOB_suite::Input_type;

			ecdf_logger<Input> log_ecdf(0,6e7,20, 0,100,20);
			csv_logger<Input> log_csv;

			// Combine them all
			observer_combine<Input> loggers(log_ecdf);
			loggers.add(log_csv);

			// Now you can single call.
			loggers.target_suite(bench);
			// etc.
		 * @endcode
		 *
		 * @note: Loggers are guaranteed to be called in the order they are added.
		 */
		template <class T>
		class observer_combine : public observer<T>
		{
		public:
			using InputType = T;

			/** Takes at least one mandatory observer,
			 * because an empty instance would be illogical.
			 */
			observer_combine(observer<T>& observer) : _observers(1, &observer)
			{
			}

			/** Handle several loggers at once, but you have to pass pointers.
			 *
			 * @note: you can use initializer lists to instanciate the given std::vector:
			 * @code
			    observer_combine loggers({log_ecdf, log_csv});
			 * @encode
			 */
			observer_combine(std::vector<observer<T>*> observers) :
				_observers(observers)
			{
				assert(_observers.size() > 0);
			}

			/** Add another observer to the list.
			 */
			void add(observer<T>& observer)
			{
				_observers.push_back(&observer);
			}

			/** Observer interface @{ */
			void track_suite(const suite::base<T>& suite) override
			{
				for (auto& p : _observers)
					p->track_suite(suite);
			}

			void track_problem(const T& pb) override
			{
				for (auto& p : _observers)
					p->track_problem(pb);
			}

			void do_log(const std::vector<double>& logger_info) override
			{
				for (auto& p : _observers)
					p->do_log(logger_info);
			}

			/** @} */

		protected:
			//! Store the managed observers.
			std::vector<observer<T>*> _observers;
		};
	}
}
