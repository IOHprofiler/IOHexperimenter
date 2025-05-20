
#pragma once

#include <optional>
#include <string>
#include "property.hpp"
#include "../loginfo.hpp"
namespace ioh
{
    namespace watch
    {
        /** A property that accesses the number of evaluations so far.
         *
         * This property retrieves the number of evaluations performed during the optimization process.
         * It supports both single-objective and multi-objective problems.
         *
         * @ingroup Logging
         */
        struct Evaluations : public logger::Property
        {
            /** Constructor.
             *
             * Initializes the property with a name and a format string.
             *
             * @param name The name of the property (default: "evaluations").
             * @param format The format string for output (default: "{:d}").
             */
            Evaluations(const std::string& name = "evaluations", const std::string &format = "{:d}") :
                Property(name, format)
            {
            }

            /** Main call interface for single-objective problems.
             *
             * Retrieves the number of evaluations from the log information.
             *
             * @param log_info The logging information for a single-objective problem.
             * @return The number of evaluations as an optional double.
             */
            std::optional<double> operator()(const logger::Info<problem::SingleObjective> &log_info) const override
            {
                return std::make_optional(static_cast<double>(log_info.evaluations));
            }

            /** Main call interface for multi-objective problems.
             *
             * Retrieves the number of evaluations from the log information.
             *
             * @param log_info The logging information for a multi-objective problem.
             * @return The number of evaluations as an optional double.
             */
            std::optional<double> operator()(const logger::Info<problem::MultiObjective> &log_info) const override
            {
                return std::make_optional(static_cast<double>(log_info.evaluations));
            }

            /** Converts the property value to a string for single-objective problems.
             *
             * Formats the number of evaluations as a string using the specified format.
             * If the value is not available, returns the provided `nan` string.
             *
             * @param log_info The logging information for a single-objective problem.
             * @param nan The string to return if the value is not available (default: "").
             * @return The formatted string representation of the number of evaluations.
             */
            [[nodiscard]] std::string call_to_string(const logger::Info<problem::SingleObjective> &log_info,
                                                     const std::string &nan = "") const override
            {
                auto opt = (*this)(log_info);
                if (opt)
                    return fmt::format(this->format(), static_cast<int>(opt.value()));
                return nan;
            }

            /** Converts the property value to a string for multi-objective problems.
             *
             * Formats the number of evaluations as a string using the specified format.
             * If the value is not available, returns the provided `nan` string.
             *
             * @param log_info The logging information for a multi-objective problem.
             * @param nan The string to return if the value is not available (default: "").
             * @return The formatted string representation of the number of evaluations.
             */
            [[nodiscard]] std::string call_to_string(const logger::Info<problem::MultiObjective> &log_info,
                                                     const std::string &nan = "") const override
            {
                auto opt = (*this)(log_info);
                if (opt)
                    return fmt::format(this->format(), static_cast<int>(opt.value()));
                return nan;
            }
        };

        /** Number of evaluations of the objective function called by the solver.
         *
         * This is an inline instance of the `Evaluations` property, which can be used
         * directly to track the number of evaluations during the optimization process.
         *
         * @ingroup Properties
         */
        inline Evaluations evaluations; 
    } // namespace logger
} // namespace ioh