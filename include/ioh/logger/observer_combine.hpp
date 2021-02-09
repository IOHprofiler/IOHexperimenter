#pragma once

#include "observer.hpp"
#include <utility>
#include <vector>


namespace ioh {
    namespace logger {
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
                LoggerCombine<Input> loggers(log_ecdf);
                loggers.add(log_csv);
        
                // Now you can single call.
                loggers.target_suite(bench);
                // etc.
         * @endcode
         *
         * @note: Loggers are guaranteed to be called in the order they are added.
         */
        template <class T>
        class LoggerCombine final : public Base<T> {
        public:
            using input_type = T;

            /** Takes at least one mandatory observer,
             * because an empty instance would be illogical.
             */
            explicit LoggerCombine(Base<T> &logger)
                : loggers_(1, &logger) {
            }

            /** Handle several loggers at once, but you have to pass pointers.
             *
             * @note: you can use initializer lists to instanciate the given std::vector:
             * @code
                LoggerCombine loggers({log_ecdf, log_csv});
             * @encode
             */
            explicit LoggerCombine(std::vector<Base<T> *> loggers)
                : loggers_(std::move(loggers)) {
                assert(!loggers_.empty());
            }

            /** Add another observer to the list.
             */
            void add(Base<T> &observer) {
                loggers_.push_back(&observer);
            }

            /** Base interface @{ */
            void track_suite(const suite::base<T> &suite) override {
                for (auto &p : loggers_)
                    p->track_suite(suite);
            }

            void track_problem(const T &pb) override {
                for (auto &p : loggers_)
                    p->track_problem(pb);
            }

            void do_log(const std::vector<double> &logger_info) override {
                for (auto &p : loggers_)
                    p->do_log(logger_info);
            }

            /** @} */

        protected:
            //! Store the managed observers.
            std::vector<Base<T> *> loggers_;
        };
    }
}
