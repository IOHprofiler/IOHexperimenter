#pragma once

#include <optional>

#include "loginfo.hpp"

namespace ioh {
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

        // /** A callback toward something that is not within log::Info. */
        // struct Attribute : public Property{
        //     Attribute(const std::string name) : Property(name) { }
        // };

    } // logger

    namespace watch {

        struct Evaluations : public logger::Property {
            Evaluations() : logger::Property("evaluations") {}
            std::optional<double> operator()(const log::Info& log_info) const
            {
                // This should always be accessible, so the optional will always contain the variable.
                return std::make_optional(static_cast<double>(log_info.evaluations));
            }
        } evaluations;

        struct YBest: public logger::Property {
            YBest() : logger::Property("raw_y_best") {}
            std::optional<double> operator()(const log::Info& log_info) const
            {
                return std::make_optional(log_info.raw_y_best);
            }
        } raw_y_best;

        struct TransformedY: public logger::Property {
            TransformedY() : logger::Property("transformed_y") {}
            std::optional<double> operator()(const log::Info& log_info) const
            {
                return std::make_optional(log_info.transformed_y);
            }
        } transformed_y;

        struct TransformedYBest: public logger::Property {
            TransformedYBest() : logger::Property("transformed_y_best") {}
            std::optional<double> operator()(const log::Info& log_info) const
            {
                return std::make_optional(log_info.transformed_y_best);
            }
        } transformed_y_best;

        /** A property that access a referenced variable.
         *
         * @note The variable must be castable to a double.
         */
        template<class T>
        class Reference: public logger::Property {
            protected:
                const T& _variable;
                
            public:
                Reference(const std::string name, const T& variable)
                : logger::Property(name),
                _variable(variable)
                { }

                std::optional<double> operator()(const log::Info& log_info) const
                {
                    return std::make_optional(static_cast<double>(_variable));
                }
        };

        /** A property that access a variable through a pointer.
         *
         * @note The pointed variable must be castable to a double.
         */
        template<class T>
        class Pointer: public logger::Property {
            protected:
                const T* const _variable;
                
            public:
                Pointer(const std::string name, const T* const variable)
                : logger::Property(name),
                _variable(variable)
                {
                    assert(variable != nullptr);
                }
                
                std::optional<double> operator()(const log::Info& log_info) const
                {
                    return std::make_optional(static_cast<double>(*_variable));
                }
        };

        /** A property that access the variable of a variable through a referente to a pointer.
         *
         * Use this if the variable does not always exists in the logged scope.
         * 
         * @warning It is your responsability to ensure that the referenced pointer is a nullptr if the variable is out of the logged scope.
         * 
         * @note The pointed variable must be castable to a double.
         */
        template<class T>
        class PointerReference: public logger::Property {
            protected:
                const T* const & _variable;
                
            public:
                PointerReference(const std::string name, const T* const & variable)
                : logger::Property(name),
                _variable(variable)
                { }
                
                std::optional<double> operator()(const log::Info& log_info) const
                {
                    if(_variable != nullptr) {
                        return std::make_optional(static_cast<double>(*_variable));
                    } else {
                        return std::nullopt;
                    }
                }
        };

    } // watch
} // ioh
