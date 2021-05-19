#pragma once

#include <cassert>

#include "api.hpp"


namespace ioh::logger
{
    /** An logger that can hold several loggers and call them all.
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
            LoggerCombine<Input> loggers(log_ecdf);
            loggers.add(log_csv);
    
            // Now you can single call.
            loggers.track_suite(bench);
            // etc.
     * @endcode
     *
     * @note: Loggers are guaranteed to be called in the order they are added.
     */
    class LoggerCombine final : public Base
    {
    public:

        /** Takes at least one mandatory logger,
         * because an empty instance would be illogical.
         */
        explicit LoggerCombine(Base &logger) :
            loggers_(1, &logger)
        {
        }

        /** Handle several loggers at once, but you have to pass pointers.
         *
         * @note: you can use initializer lists to instantiate the given std::vector:
         * @code
            LoggerCombine loggers({log_ecdf, log_csv});
         * @encode
         */
        explicit LoggerCombine(std::vector<Base*> loggers) :
            loggers_(std::move(loggers))
        {
            assert(!loggers_.empty());
        }

        /** Add another logger to the list.
         */
        void add(Base &logger)
        {
            loggers_.push_back(&logger);
        }

        /** Base interface @{ */
        void track_suite(const std::string &suite_name) override
        {
            for (auto &l : loggers_)
                l->track_suite(suite_name);
        }

        void track_problem(const problem::MetaData &problem) override
        {
            for (auto &l : loggers_)
                l->track_problem(problem);
        }

        void log(const LogInfo &logger_info) override
        {
            for (auto &l : loggers_)
                l->log(logger_info);
        }

        void flush() override
        {
            for (auto& l : loggers_)
                l->flush();
        }

        /** @} */

    protected:
        //! Store the managed loggers.
        std::vector<Base *> loggers_;
    };
}
