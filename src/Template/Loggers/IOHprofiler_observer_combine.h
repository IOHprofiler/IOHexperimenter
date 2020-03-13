/// \file IOHprofiler_observer_combine.h
/// \brief Header file for class IOHprofiler_observer_combine.
///
/// \author Johann Dreo
#ifndef _IOHPROFILER_OBSERVER_COMBINE_H_
#define _IOHPROFILER_OBSERVER_COMBINE_H_

#include <vector>

#include "IOHprofiler_observer.h"

/** An observer that can hold several observers and call them all.
 *
 * Useful if one want to use several loggers without having to manage them
 * separately.
 *
 * Example:
 * @code
    BBOB_suite bench({1},{2},{3});
    using Input = BBOB_suite::Input_type;

    IOHprofiler_ecdf_logger<Input> log_ecdf(0,6e7,20, 0,100,20);
    IOHprofiler_csv_logger<Input> log_csv;

    // Combine them all
    IOHprofiler_observer_combine<Input> loggers(log_ecdf);
    loggers.add(log_csv);

    // Now you can single call.
    loggers.target_suite(bench);
    // etc.
 * @endcode
 *
 * @note: Loggers are guaranteed to be called in the order they are added.
 */
template<class T>
class IOHprofiler_observer_combine : public IOHprofiler_observer<T>
{
    public:
        using InputType = T;

        /** Takes at least one mandatory observer,
         * because an empty instance would be illogical.
         */
        IOHprofiler_observer_combine( IOHprofiler_observer<T>& observer );

        /** Handle several loggers at once, but you have to pass pointers.
         *
         * @note: you can use initializer lists to instanciate the given std::vector:
         * @code
            IOHprofiler_observer_combine loggers({log_ecdf, log_csv});
         * @encode
         */
        IOHprofiler_observer_combine( std::vector<IOHprofiler_observer<T>*> observers );

        /** Add another observer to the list.
         */
        void add( IOHprofiler_observer<T>& observer );

        /** Observer interface @{ */

        void track_suite(const IOHprofiler_suite<T>& suite);

        void track_problem(const IOHprofiler_problem<T> & pb);

        void do_log(const std::vector<double> &logger_info);

        /** @} */

    protected:
        //! Store the managed observers.
        std::vector<IOHprofiler_observer<T>*> _observers;
};

#include "IOHprofiler_observer_combine.hpp"

#endif // _IOHPROFILER_OBSERVER_COMBINE_H_

