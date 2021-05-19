#pragma once

#include <optional>

#include "api.hpp"

namespace ioh::watch {

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

    /** A property that access the value of a referenced variable.
     *
     * @note The variable must be castable to a double.
     */
    template<class T>
    class Reference: public logger::Property {
        protected:
            const T& _value;
            
        public:
            Reference(const std::string name, const T& value)
            : logger::Property(name),
            _value(value)
            { }

            std::optional<double> operator()(const log::Info& log_info) const
            {
                return std::make_optional(static_cast<double>(_value));
            }
    };

    /** A property that access the value of a variable through a pointer.
     *
     * @note The pointed variable must be castable to a double.
     */
    template<class T>
    class Pointer: public logger::Property {
        protected:
            const T* const _value;
            
        public:
            Pointer(const std::string name, const T* const value)
            : logger::Property(name),
            _value(value)
            {
                assert(value != nullptr);
            }
            
            std::optional<double> operator()(const log::Info& log_info) const
            {
                return std::make_optional(static_cast<double>(*_value));
            }
    };

    /** A property that access the value of a variable through a referente to a pointer.
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
            const T* const & _value;
            
        public:
            PointerReference(const std::string name, const T* const & value)
            : logger::Property(name),
            _value(value)
            { }
            
            std::optional<double> operator()(const log::Info& log_info) const
            {
                if(_value != nullptr) {
                    return std::make_optional(static_cast<double>(*_value));
                } else {
                    return std::nullopt;
                }
            }
    };

} // ioh
