#pragma once

#include <optional>
#include <string>
#include "../loginfo.hpp"

namespace ioh
{
    namespace logger
    {
        //! Default format for storing doubles
        inline std::string DEFAULT_DOUBLE_FORMAT = "{:.10f}";

        /** @defgroup Properties Properties
         * Accessors to values that are to be logged.
         *
         * You can directly pass the "variables" to a logger to make it track their values.
         *
         * @note Only some loggers are able to track user-defined properties
         *       (those inheriting from the Watcher interface).
         *
         * @note The loggers only watch variables that may be casted to `double`.
         *       The functions are templated so that you don't need to do the casting by yourself,
         *       but it is nonetheless mandatory.
         *
         * For example:
         * @code
                double my_algo_parameter = 42;

                // Track the number of evaluations and the best value of the objective function
                // (at each calls with the Store logger):
                ioh::logger::Store my_store( // Log everything in-memory.
                    {ioh::trigger::always},                                     // Trigger the logger at each call.
                    {                                                           // Do log:
                        ioh::watch::evaluations,                                // - evaluations,
                        ioh::watch::transformed_y_best,                         // - best values,
                        ioh::watch::reference("my_parameter",my_algo_parameter) // - any change to my extern variable.
                    }
                );
         * @endcode
         *
         * Some properties need to be instantiated with parameters before being passed to a logger,
         * you can use the related free functions (lower-case names) to do so on the heap.
         *
         * @warning Those free functions do allocates on the heap, so you're responsible of freeing memory after them if
         necessary.
         *
         * For example:
         * @code
                auto& p = watch::reference("my", some_variable);
                // [Use p...]
                delete &p;
         * @endcode
         *
         * @ingroup Loggers
         */

        /** Interface for callbacks toward variable to be logged.
         *
         * A class that inherit this interface
         * should be able to indicate if the managed variable
         * is not available at the time of the current logger's call.
         *
         * @ingroup Logging
         */


        /** Interface for callbacks toward variable to be logged.
         *
         * A class that inherits this interface
         * should be able to indicate if the managed variable
         * is not available at the time of the current logger's call.
         */
        class Property
        /**
         * @class Property
         * @brief Abstract base class for managing and logging properties.
         *
         * This class provides an interface for managing properties with a unique name and format.
         * It allows derived classes to implement specific behavior for accessing and formatting
         * property values based on the current problem state data.
         */
        {
        protected:
            //! Unique name for the logged property.
            std::string name_;

            //! Format specification for fmt.
            const std::string format_;

        public:
            /**
             * @brief Constructor to initialize the property with a name and format.
             * @param name The unique name of the property.
             * @param format The format specification for the property (default is DEFAULT_DOUBLE_FORMAT).
             * @throws Assertion if the name is empty.
             */
            Property(const std::string &name, const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) : name_(name), format_(format)
            {
                assert(!name_.empty());
            }

            /**
             * @brief Pure virtual function to retrieve the current value of the managed variable.
             * @param log_info The current problem state data.
             * @return An optional containing a `double` if the variable is available, or `std::nullopt` otherwise.
             */
            virtual std::optional<double> operator()(const logger::Info<problem::SingleObjective> &log_info) const = 0;

            /**
             * @brief Pure virtual function to retrieve the current value of the managed variable.
             * @param log_info The current problem state data.
             * @return An optional containing a `double` if the variable is available, or `std::nullopt` otherwise.
             */
            virtual std::optional<double> operator()(const logger::Info<problem::MultiObjective> &log_info) const = 0;

            /**
             * @brief Accessor for the configured name of the property.
             * @return The name of the property.
             */
            std::string name() const { return name_; }

            /**
             * @brief Accessor for the configured format of the property.
             * @return The format of the property.
             */
            std::string format() const { return format_; }

            /**
             * @brief Mutator to set the name of the property.
             * @param name The new name for the property.
             */
             void set_name(const std::string& name) {name_ = name;}

            /**
             * @brief Virtual destructor for the Property class.
             */
            virtual ~Property() = default;

            /**
             * @brief Converts the property data to a string representation.
             * @param log_info The current problem state data.
             * @param nan The value to log when there is no data (default is an empty string).
             * @return A string representation of the property's data.
             */
            [[nodiscard]] virtual std::string call_to_string(const logger::Info<problem::SingleObjective> &log_info,
                                                             const std::string &nan = "") const
            {
                auto opt = (*this)(log_info);
                if (opt)
                    return fmt::format(format(), opt.value());
                return nan;
            }

            /**
             * @brief Converts the property data to a string representation.
             * @param log_info The current problem state data.
             * @param nan The value to log when there is no data (default is an empty string).
             * @return A string representation of the property's data.
             */
            [[nodiscard]] virtual std::string call_to_string(const logger::Info<problem::MultiObjective> &log_info,
                                                             const std::string &nan = "") const
            {
                auto opt = (*this)(log_info);
                if (opt)
                    return fmt::format(format(), opt.value());
                return nan;
            }
        };
        
        /**
         * \brief convenience typedef for a vector of properties
         */
        using Properties = std::vector<std::reference_wrapper<Property>>;
    } // namespace logger
} // namespace ioh


template <>
struct fmt::formatter<std::reference_wrapper<ioh::logger::Property>> : formatter<std::string>
{
    template <typename FormatContext>
    //! Format call interface
    auto format(const std::reference_wrapper<ioh::logger::Property> &a, FormatContext &ctx) const
    {
        return formatter<std::string>::format(a.get().name(), ctx);
    }
};
