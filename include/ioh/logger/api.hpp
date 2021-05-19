#pragma once

#include <stdexcept>

#include "../problem/utils.hpp"

namespace ioh {
    namespace log {

        /** Information about the current log.
         * 
         * @note The properties for bests values holds a state since the first start or the last call to reset.
         * 
         * @note "transformed" indicates that a monotonic transformation is applied,
         *       which is specific to the currently configured problem _instance_.
         * 
         * @note If there is an improvement, then fields holding "best" values will have the same value than the others.
        */
        struct Info
        {
            //! Number of evaluations of the objective function so far.
            size_t evaluations;

            //! The current best internal objective function value (since the last reset).
            double raw_y_best; // was y_best
            
            //! The current transformed objective function value.
            double transformed_y;
            
            //! The current best transformed objective function value (since the last reset).
            double transformed_y_best;
            
            //! Currently considered solution with the corresponding transformed objective function value.
            problem::Solution<double> current;
            
            //! Optimum to the current problem instance, with the corresponding transformed objective function value.
            problem::Solution<double> optimum; // was objective
        };

    } // log

    namespace logger {
        
        /** Triggering a log event. */
        struct Trigger {

            /** @returns true if a log event is to be triggered given the passed state. */
            virtual bool operator()(const log::Info& log_info, const problem::MetaData& pb_info) = 0;

            /** Reset any internal state.
             * 
             * @note This is called when the logger is attached to a new problem/run/etc.
             * 
             * Useful if, for instance, the trigger maintain its own "best value so far" (@see logger::OnImprovement).
             */
            virtual void reset() { }
        };
        
    } // logger

    // This is out of the logger namespace, to avoid name repetition.
    /** Interface of loggers.
     * 
     * A logger is a class that has a `log(log_info)` function called by the problem
     * and hosts a set of Trigger that determine when a log event should be handled (or silently passed).
     * 
     * @warning Using the call functions without having been attached to a problem first will results
     *          in an assert failur
     */
    class Logger {
    protected:
        std::vector<std::reference_wrapper<logger::Trigger>> _triggers;
        const problem::MetaData* _problem;

    public:
        Logger( std::initializer_list<std::reference_wrapper<logger::Trigger>> triggers)
        : _triggers(triggers),
        _problem(nullptr)
        {
            assert(_triggers.size() > 0);
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
            
            bool any = false;
            for(auto& trigger : _triggers) {
                if(trigger(log_info, *_problem)) {
                    any = true;
                    break;
                }
            }
            if(any) {
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
            for(auto& trigger : _triggers) {
                trigger.get().reset();
            }
        }

        virtual ~Logger() = default;
    };

    namespace logger {

        /** Interface for callbacks toward variable to be logged.
         * 
         * A class that inherit this interface
         * should be able to indicate if the managed variable
         * is not available at the time of the current logger's call.
         */
        class Property{
        protected:
            const std::string _name;
        public:
            Property(const std::string name)
            : _name(name)
            {
                assert(_name.size() > 0);
            }

            /** Returns the current value of the managed variable
             * 
             * The call interface should return an optional,
             * which should be set to `std::nullopt`
             * if the managed variable cannot be accessed
             * (for instance if it does not exists during the call scope,
             *  for example if it's a dynamic algorithm parameter that
             *  is not currently configured).
             * 
             * @param log_info The current problem state data.
             * @returns An optional that holds a `double` if the variable is available, `std::nullopt` else.
             */
            virtual std::optional<double> operator()(const log::Info& log_info) const = 0;

            std::string name() const { return _name; }
        };

        /** A callback toward something that is not within log::Info. */
        struct Attribute : public Property{
            Attribute(const std::string name) : Property(name) { }
        };

        /** A Logger which tracks variables. */
        class Watcher: public Logger {
        protected:
            std::vector<std::reference_wrapper<logger::Property>> _properties;
            
        public:
            /** The logger::Tracker should at least have one logger::Trigger
             *  and one logger::Property,
             *  or else it makes no sense to use it.
             */
            Watcher(std::initializer_list<std::reference_wrapper<logger::Trigger >> triggers,
                    std::initializer_list<std::reference_wrapper<logger::Property>> properties)
            : Logger(triggers), _properties(properties)
            {
#ifndef NDEBUG
                assert(_properties.size() > 0);
                // Assert that there is no duplicates.
                for(auto iw = std::begin(_properties); iw != std::end(_properties); iw++) {
                    auto inext = iw; inext++;
                    if(inext == std::end(_properties)) { break; }
                    // Search only in toward the end. 
                    assert(std::find_if(inext,std::end(_properties),
                                        [iw](const auto& rwp){return iw->get().name() == rwp.get().name();}
                                       ) == std::end(_properties));
                }
#endif
            }

            void watch(logger::Property property)
            {
                // Assert that the Property is not already tracked.
                assert(std::find_if(std::begin(_properties),std::end(_properties),
                                    [&property](const auto rwp){return property.name() == rwp.get().name();}
                                   ) == std::end(_properties));
                _properties.push_back(property);
            }

            /* Interface from Logger to be implemented:
            virtual void track_problem(const problem::MetaData& problem) = 0;
            virtual void track_suite(const std::string& suite_name) = 0;
            virtual void log(const log::Info& log_info) = 0;
            */
        };
        
    } // logger
}
