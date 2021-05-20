#pragma once

#include <stdexcept>

#include "../problem/utils.hpp"

#include "loginfo.hpp"
#include "triggers.hpp"
#include "properties.hpp"

namespace ioh {

    // This is out of the logger namespace, to avoid name repetition.
    /** Interface of loggers.
     * 
     * A logger is a class that has a `log(log_info)` function called by the problem
     * and hosts a set of Trigger that determine when a log event should be handled (or silently passed).
     * 
     * @warning Using the call functions without having been attached to a problem first will results
     *          in an assert failure.
     * 
     * By default, the Logger calls a log event if any of the triggers are fired,
     * but you can use your own way to combine triggers if necessary.
     */
    class Logger {
    protected:
        trigger::Any _any; // Default combination of triggers instance.
        trigger::Set& _triggers; // High-level interface.
        const problem::MetaData* _problem;
        std::vector<std::reference_wrapper<logger::Property>> _properties;

#ifndef NDEBUG
        bool consistent_properties()
        {
            if(_properties.size() <= 0) {return false;}
            // Check that there is no duplicates.
            for(auto iw = std::begin(_properties); iw != std::end(_properties); iw++) {
                auto inext = iw; inext++;
                if(inext == std::end(_properties)) { break; }
                // Search only in toward the end. 
                if(std::find_if(inext,std::end(_properties),
                                [iw](const auto& rwp){return iw->get().name() == rwp.get().name();}
                               ) != std::end(_properties)) {
                    return false;
                }
            }
            return true;
        }
#endif
        
    public:
        /** Use this constructor if you just need to trigger a log event if ANY of the triggers are fired. */
        Logger(std::initializer_list<std::reference_wrapper<logger::Trigger >> triggers,
               std::initializer_list<std::reference_wrapper<logger::Property>> properties)
        : _any(triggers)
        , _triggers(_any)
        , _problem(nullptr)
        , _properties(properties)
        {
            assert(consistent_properties());
        }

        /** Use this constructor if you want to combine triggers differently.
         *
         * You can for instance combine your triggers in a trigger::All,
         * so that the log event would be triggered only if ALL triggers
         * are fired at once.
         */
        Logger(trigger::Set& triggers,
               std::initializer_list<std::reference_wrapper<logger::Property>> properties               )
        : _any()
        , _triggers(triggers)
        , _problem(nullptr)
        , _properties(properties)
        {
            assert(consistent_properties());
        }

        /** Add the given trigger to the list. */
        void trigger(logger::Trigger when)
        {
            _triggers.push_back(when);
        }

        /** Check if the logger should be triggered and if so, call `call(log_info)`. */
        void log(const log::Info& log_info)
        {
            assert(_problem != nullptr); // For Debug builds.
            if(not _problem) { // For Release builds.
                throw std::runtime_error("Logger has not been attached to a problem.");
            }

            assert(_triggers.size() > 0);
            if(_triggers(log_info, *_problem)) {
                call(log_info);
            }
        }

        /** Starts a new session for the given problem/instance/dimension/run.
         * 
         * This is called automatically by the Problem (or Suite) to which the Logger is attached,
         * each time a change occurs (be it a new run, or a new problem instance).
         * 
         * @warning If you override this function, do not forget to call the base class' one.
         */
        virtual void attach_problem(const problem::MetaData& problem)
        {
            _problem = &problem;
        }

        //! Starts a new session for the given Suite name.
        virtual void attach_suite(const std::string& suite_name) = 0;

        //! Main entry point, called everytime a Trigger is true.
        virtual void call(const log::Info& log_info) = 0;

        /** Optional actions when the logger is detached from a suite/problem or the problem is reset.
         *
         * Useful if you want to flush data, for instance.
         * 
         * @warning If you override this function, do not forget to call the base class' one.
         */
        virtual void reset()
        {
            _triggers.reset();
        }

        virtual ~Logger() = default;
    };

    namespace logger {

        /** A Logger to which the user can add properties to be watched.
         */
        class Watcher: public Logger {
        public:
            /** Use this constructor if you just need to trigger a log event if ANY of the triggers are fired. */
            Watcher(std::initializer_list<std::reference_wrapper<logger::Trigger >> when,
                   std::initializer_list<std::reference_wrapper<logger::Property>> what)
            : Logger(when,what)
            { }

            /** Use this constructor if you want to combine triggers differently.
             *
             * You can for instance combine your triggers in a trigger::All,
             * so that the log event would be triggered only if ALL triggers
             * are fired at once.
             */
            Watcher(trigger::Set& when,
                   std::initializer_list<std::reference_wrapper<logger::Property>> what)
            : Logger(when,what)
            { }

            // This essentially just expose _properties.push_back with some checks.
            void watch(logger::Property& property)
            {
                // Assert that the Property is not already tracked.
                assert(std::find_if(std::begin(_properties),std::end(_properties),
                                    [&property](const auto rwp){return property.name() == rwp.get().name();}
                                   ) == std::end(_properties));
                _properties.push_back(property);
                assert(consistent_properties()); // Double check duplicates, in case the code above would be changed.
            }

            /* Interface from Logger to be implemented:
            virtual void track_problem(const problem::MetaData& problem) = 0;
            virtual void track_suite(const std::string& suite_name) = 0;
            virtual void log(const log::Info& log_info) = 0;
            */
        };
        
    } // logger
}
