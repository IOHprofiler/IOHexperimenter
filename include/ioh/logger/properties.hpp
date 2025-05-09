#pragma once

#include <optional>

#include "loginfo.hpp"


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
        template <typename R>
        class Property
        {
        protected:
            //! Unique name for the logged property.
            std::string name_;

            //! format specification for fmt
            const std::string format_;

        public:
            //! Constructor.
            Property(const std::string &name, const std::string &format = DEFAULT_DOUBLE_FORMAT) : name_(name), format_(format)
            {
                assert(!name_.empty());
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
            virtual std::optional<double> operator()(const logger::Info<R> &log_info) const = 0;

            //! Configured name accessor.
            std::string name() const { return name_; }

            //! Configured format accessor.
            std::string format() const { return format_; }
            
            //! Accessor for name
            void set_name(const std::string& name) {name_ = name;}

            //! Destructor.
            virtual ~Property() = default;

            /**
             * \brief Method to parse the log data into a string
             * \param log_info The current problem state data.
             * \param nan The value to log when there is no data.
             * \return a string representation of the properties' data
             */
            [[nodiscard]] virtual std::string call_to_string(const logger::Info<R> &log_info,
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
        template <typename R>
        using Properties = std::vector<std::reference_wrapper<Property<R>>>;
    } // namespace logger

    /** Properties that can be watched by loggers. */
    namespace watch
    {

        /** A property that access the number of evaluations so far.
         *
         * @ingroup Logging
         */
        template <typename R>
        struct Evaluations : public logger::Property<R>
        {
            //! Constructor.
            Evaluations(const std::string& name = "evaluations", const std::string &format = "{:d}") :
                logger::Property<R>(name, format)
            {
            }
            //! Main call interface.
            std::optional<double> operator()(const logger::Info<R> &log_info) const override
            {
                // This should always be accessible, so the optional will always contain the variable.
                return std::make_optional(static_cast<double>(log_info.evaluations));
            }

            [[nodiscard]] std::string call_to_string(const logger::Info<R> &log_info,
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
         * @ingroup Properties
         */
        template <typename R>
        inline Evaluations<R> evaluations; 

        /** A property that access the cyrrent objective value, without transformation.
         *
         * @ingroup Logging
         */
        struct RawY : public logger::Property<problem::SingleObjective>
        {
            //! Constructor.
            RawY(const std::string name = "raw_y", const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                logger::Property<problem::SingleObjective>(name, format)
            {
            }
            //! Main call interface.
            std::optional<double> operator()(const logger::Info<problem::SingleObjective> &log_info) const override
            {
                if(abs(log_info.raw_y) == std::numeric_limits<double>::infinity()){
                    return {};
                }                
                return std::make_optional(log_info.raw_y);
            }
        };

        /** Current function value, without transformation.
         *
         * @ingroup Properties
         */
        
        inline RawY raw_y; 

        /** A property that access the best value so far, without transformation.
         *
         * @ingroup Logging
         */
        struct RawYBest : public logger::Property<problem::SingleObjective>
        {
            //! Constructor.
            RawYBest(const std::string name = "raw_y_best", const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                logger::Property<problem::SingleObjective>(name, format)
            {
            }
            //! Main call interface.
            std::optional<double> operator()(const logger::Info<problem::SingleObjective> &log_info) const override
            {
                return std::make_optional(log_info.raw_y_best);
            }
        };
        /** Best objective function value found so far, without transformation.
         *
         * @ingroup Properties
         */
        inline RawYBest raw_y_best;         

        /** A property that access the current value so far, with transformation.
         *
         * @ingroup Logging
         */
        struct TransformedY : public logger::Property<problem::SingleObjective>
        {
            //! Constructor.
            //! Main call interface.
            TransformedY(const std::string name = "transformed_y", const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                logger::Property<problem::SingleObjective>(name, format)
            {
            }
            std::optional<double> operator()(const logger::Info<problem::SingleObjective> &log_info) const override
            {
                return std::make_optional(log_info.transformed_y);
            }
        };
        /** Objective function value for this call, with transformation.
         *
         * @ingroup Properties
         */
        inline TransformedY transformed_y; 

        /** A property that access the best value found so far, with transformation.
         *
         * @ingroup Logging
         */
        struct TransformedYBest : public logger::Property<problem::SingleObjective>
        {
            //! Constructor.
            TransformedYBest(const std::string name = "transformed_y_best", const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                logger::Property<problem::SingleObjective>(name, format)
            {
            }
            //! Main call interface.
            std::optional<double> operator()(const logger::Info<problem::SingleObjective> &log_info) const override
            {
                return std::make_optional(log_info.transformed_y_best);
            }
        };
        /** Best objective function value found so far, with transformation.
         *
         * @ingroup Properties
         */
        inline TransformedYBest transformed_y_best; 

        /** A property that access the current value so far, with transformation and constraints applied
         *
         * @ingroup Logging
         */
        struct CurrentY : public logger::Property<problem::SingleObjective>
        {
            //! Constructor.
            //! Main call interface.
            CurrentY(const std::string name = "current_y", const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                logger::Property<problem::SingleObjective>(name, format)
            {
            }
            std::optional<double> operator()(const logger::Info<problem::SingleObjective> &log_info) const override
            {
                return std::make_optional(log_info.y);
            }
        };
        /** Objective function value for this call, without transformation.
         *
         * @ingroup Properties
         */
        inline CurrentY current_y; 


         /** A property that access the current best value so far, with transformation and constraints applied
         *
         * @ingroup Logging
         */
        struct CurrentBestY : public logger::Property<problem::SingleObjective>
        {
            //! Constructor.
            //! Main call interface.
            CurrentBestY(const std::string name = "current_y_best",
                         const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                logger::Property<problem::SingleObjective>(name, format)
            {
            }
            std::optional<double> operator()(const logger::Info<problem::SingleObjective> &log_info) const override
            {
                return std::make_optional(log_info.y_best);
            }
        };
        /** Objective function value for this call, without transformation.
         *
         * @ingroup Properties
         */
        inline CurrentBestY current_y_best;


        /** A property that accesses the current constraint violation
         *
         * @ingroup Logging
         */
        template <typename R>
        struct Violation : public logger::Property<R>
        {
            //! Constructor.
            //! Main call interface.
            Violation(const std::string name = "violation",
                      const std::string &format = logger::DEFAULT_DOUBLE_FORMAT,
                      const size_t ci = 0
                ) :
                logger::Property<R>(name, format),
                ci(ci)
            {
            }
            std::optional<double> operator()(const logger::Info<R> &log_info) const override
            {
                return std::make_optional(log_info.violations[ci]);
            }

            private:
                //! Index of the constraint to be logged
                size_t ci;
        };
        
        /** Objective function value for this call, without transformation.
         *
         * @ingroup Properties
         */
        template <typename R>
        inline Violation<R> violation;


        /** A property that accesses the current constraint penalty
         *
         * @ingroup Logging
         */
        template <typename R>
        struct Penalty : public logger::Property<R>
        {
            //! Constructor.
            //! Main call interface.
            Penalty(const std::string name = "penalty", 
                    const std::string &format = logger::DEFAULT_DOUBLE_FORMAT,
                    const size_t ci = 0
                ) :
                logger::Property<R>(name, format),
                ci(ci)
            {
            }
            std::optional<double> operator()(const logger::Info<R> &log_info) const override
            {

                return std::make_optional(log_info.penalties[ci]);
            }

        private:
            //! Index of the constraint to be logged
            size_t ci;
        };

        /** Objective function value for this call, without transformation.
         *
         * @ingroup Properties
         */
        template <typename R>
        inline Penalty<R> penalty;

        /** A property that access a referenced variable.
         *
         * @note The variable must be castable to a double.
         *
         * @ingroup Logging
         */
        template <class T, typename R>
        class Reference : public logger::Property<R>
        {
        protected:
            //! The managed reference.
            const T &_variable;

        public:
            /** Constructor.
             *
             * @param name the name of the property.
             * @param variable a reference to the logged variable.
             * @param format a fmt::format specification
             */
            Reference(const std::string name, const T &variable, const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                logger::Property<R>(name, format), _variable(variable)
            {
            }

            //! Main call interface.
            std::optional<double> operator()(const logger::Info<R> &) const override
            {
                return std::make_optional(static_cast<double>(_variable));
            }
        };
        /** The value of an extern variable (captured by reference).
         *
         * @param name the name of the property.
         * @param variable a reference to the logged variable.
         * @param format a fmt::format specification
         *
         * @ingroup Properties
         */
        template <class T, typename R>
        Reference<T,R> &reference(const std::string name, const T &variable, const std::string &format = logger::DEFAULT_DOUBLE_FORMAT)
        {
            auto p = new Reference<T,R>(name, variable, format);
            return *p;
        }

        /** A property that access a variable through a pointer.
         *
         * @note The pointed variable must be castable to a double.
         *
         * @ingroup Logging
         */
        template <class T, typename R>
        class Pointer : public logger::Property<R>
        {
        protected:
            //! The managed variable.
            const T *const _variable;

        public:
            /** Constructor.
             *
             * @param name the name of the property.
             * @param variable a pointer to the logged variable.
             * @param format a fmt::format specification
             */
            Pointer(const std::string name, const T *const variable, const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                logger::Property<R>(name, format), _variable(variable)
            {
                assert(variable != nullptr);
            }

            //! Main call interface.
            std::optional<double> operator()(const logger::Info<R> &) const override
            {
                return std::make_optional(static_cast<double>(*_variable));
            }
        };
        /** The value of an extern variable (captured by address).
         *
         * @param name the name of the property.
         * @param variable a pointer to the logged variable.
         * @param format a string to format the variable when logging. 
         *
         * @ingroup Properties
         */
        template <class T, typename R>
        Pointer<T,R> &address(const std::string name, const T *const variable, const std::string &format = logger::DEFAULT_DOUBLE_FORMAT)
        {
            auto p = new Pointer<T,R>(name, variable, format);
            return *p;
        }

        /** A property that access the variable of a variable through a reference to a pointer.
         *
         * Use this if the variable does not always exists in the logged scope.
         *
         * @warning It is your responsability to ensure that the referenced pointer is a nullptr if the variable is out
         * of the logged scope.
         *
         * @note The pointed variable must be castable to a double.
         *
         * @ingroup Logging
         */
        template <class T, typename R>
        class PointerReference : public logger::Property<R>
        {
        public:
            //! Typedef for the ptr
            using PtrType = T*;
            //! Typedef for the const ptr
            using ConstPtrType = PtrType const;
            //! Typedef for the const ptr ref
            using RefType = ConstPtrType &;
            //! Typedef for the const const ptr ref
            using ConstRefType = RefType; 

            // using Type = const T *const &;
            // We failed to make the above direct declaration work under g++-8 and clang++-9.
            // It would silently generate a code that fails to have the correct behaviour.
            
        protected:
            //! The managed reference to a pointer.
            ConstRefType _ref_ptr_var;

#ifndef NDEBUG
        public:
            //! Accessor for the internal ptr
            RefType ref_ptr_var() const {return const_cast<RefType>(_ref_ptr_var);} // g++-8 issues a warning for the const having no effect.           
#endif
        public:
            /** Constructor.
             *
             * @param name the name of the property.
             * @param ref_ptr_var a reference to a pointer to the logged variable.
             * @param format a fmt::format specification
             */
            PointerReference(const std::string name, ConstRefType ref_ptr_var, const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                logger::Property<R>(name, format), _ref_ptr_var(ref_ptr_var)
            {
                if (_ref_ptr_var != nullptr)
                {
                    // IOH_DBG(debug, "PointerReference "  name << " @ " << ref_ptr_var << " == " << static_cast<double>(*ref_ptr_var));
                }
                // IOH_DBG(debug, "PointerReference " << name << " @ " << ref_ptr_var << " == nullptr");
            }

            //! Main call interface.
            std::optional<double> operator()(const logger::Info<R> &) const override
            {
                if (_ref_ptr_var != nullptr)
                {
                    // IOH_DBG(debug, "PointerReference " << name() << " @ " << _ref_ptr_var << " == " << static_cast<double>(*_ref_ptr_var));
                    return std::make_optional(static_cast<double>(*_ref_ptr_var));
                }
                // IOH_DBG(debug, "PointerReference " << name() << " @ " << _ref_ptr_var << " == nullopt");
                return std::nullopt;
            }

        };
        /** The value of an extern variable, which may not exists.
         *
         * Useful for variables that doesn't exists at some point during the solver run.
         * In that case, if the referenced pointer is a `std::nullptr`,
         * the value will be indicated as invalid in the logs.
         *
         * To do so, this captures a reference to a pointer toward your variable.
         * If you update the value of the pointed variables, it will change in the logs.
         * If you need to invalidate the variable, you can set the referenced pointer itself to `nullptr`.
         *
         * @warning It is your responsability to ensure that the referenced pointer is a nullptr if the variable is out
         * of the logged scope.
         *
         * @param name the name of the property.
         * @param variable a reference to a pointer to the logged variable.
         * @param format a fmt::format specification
         *
         * @ingroup Properties
         */
        template <class T, typename R>
        PointerReference<T,R> &pointer(const std::string name, const T *const &variable,
                                     const std::string &format = logger::DEFAULT_DOUBLE_FORMAT)
        {
            auto p = new PointerReference<T,R>(name, variable, format);
            return *p;
        }
    } // namespace watch
} // namespace ioh


//! formatter for properties
template <typename R>
struct fmt::formatter<std::reference_wrapper<ioh::logger::Property<R>>> : formatter<std::string>
{
    template <typename FormatContext>
    //! Format call interface
    auto format(const std::reference_wrapper<ioh::logger::Property<R>> &a, FormatContext &ctx) const
    {
        return formatter<std::string>::format(a.get().name(), ctx);
    }
};
