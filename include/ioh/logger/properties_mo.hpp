#pragma once

#include <optional>

#include "loginfo.hpp"


namespace ioh
{
    namespace logger
    {
        struct MultiObjectiveInfo : public Info
        {
            // //! Number of evaluations of the objective function so far.
            // size_t evaluations;

            //! The current transformed objective function value.
            std::vector<double> transformed_y_mo;

            //! The current best transformed objective function value (since the last reset).
            std::vector<std::vector<double>> transformed_y_best_mo;

            //! The current transformed objective function value with constraints applied.
            std::vector<double> y_mo;

            //! The current best transformed objective function value with constraints applied (since the last reset).
            std::vector<problem::MultiObjectiveSolution<double>> pareto_front;

            //! Current search space variables
            // std::vector<double> x;

            //! Constraint violations, first element is total violation by ContraintSet
            // std::vector<double> violations;

            //! Applied penalties by constraint, first element is total penalty applied by ContraintSet
            // std::vector<double> penalties;

            //! Optimum to the current problem instance, with the corresponding transformed objective function value.
            std::vector<problem::MultiObjectiveSolution<double>> optimum_mo;

            //! Single objective check whether state-update has caused an improvement
            // bool has_improved;
        };

        namespace mo
        {
            class Property
            {
            protected:
                //! Unique name for the logged property.
                const std::string name_;

                //! format specification for fmt
                const std::string format_;

            public:
                //! Constructor.
                Property(const std::string &name, const std::string &format = DEFAULT_DOUBLE_FORMAT) :
                    name_(name), format_(format)
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
                virtual std::optional<std::vector<double> > operator()(const logger::MultiObjectiveInfo &log_info) const = 0;

                //! Configured name accessor.
                std::string name() const { return name_; }

                //! Configured format accessor.
                std::string format() const { return format_; }

                //! Destructor.
                virtual ~Property() = default;

                /**
                 * \brief Method to parse the log data into a string
                 * \param log_info The current problem state data.
                 * \param nan The value to log when there is no data.
                 * \return a string representation of the properties' data
                 */
                [[nodiscard]] virtual std::string call_to_string(const logger::MultiObjectiveInfo &log_info,
                                                                const std::string &nan = "") const
                {
                    auto opt = (*this)(log_info);
                    if (opt) {
                        return fmt::format("{}", fmt::join(opt.value(), ","));
                    }
                    return nan;
                }
            };

            /**
             * \brief convenience typedef for a vector of properties
             */
            using Properties = std::vector<std::reference_wrapper<Property>>;

            /** Properties that can be watched by loggers. */
        namespace watch
        {
            /** A property that access the current value so far, with transformation.
         *  A version for the multiobjective optimization.
         *
         * @ingroup Logging
         */
        // struct TransformedY_MO : public logger::mo::Property
        // {
        //     //! Constructor.
        //     //! Main call interface.
        //     TransformedY_MO(const std::string name = "transformed_y_mo",
        //                     const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
        //         logger::Property(name, format)
        //     {
        //     }
        //     std::optional<std::vector<double> > operator()(const logger::MultiObjectiveInfo &log_info) const override
        //     {
        //         return std::make_optional(log_info.transformed_y_mo);
        //     }
        // };
        /** Objective function value for this call, with transformation.
         *  A version for the multiobjetive optimization.
         *
         * @ingroup Properties
         */
        // inline TransformedY_MO transformed_y_mo;

        /** A property that access the best value found so far, with transformation.
         *  A version for the multiobjective optimization
         *
         * @ingroup Logging
         */
        // struct TransformedYBest_MO : public logger::Property
        // {
        //     //! Constructor.
        //     TransformedYBest_MO(const std::string name = "transformed_y_best_mo",
        //                         const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
        //         logger::Property(name, format)
        //     {
        //     }
        //     //! Main call interface.
        //     std::optional<std::vector<std::vector<double>>>
        //     operator()(const logger::MultiObjectiveInfo &log_info) const override
        //     {
        //         return std::make_optional(log_info.transformed_y_best_mo);
        //     }
        // };
        // /** Best objective function value found so far, with transformation.
        //  *  A version for the multiobjective optimization.
        //  * @ingroup Properties
        //  */
        // inline TransformedYBest_MO transformed_y_best_mo;
        /** A property that access the current value so far, with transformation and constraints applied
         *  A version for the multiobjective optimization.
         *
         * @ingroup Logging
         */
        struct CurrentY_MO : public logger::mo::Property
        {
            //! Constructor.
            //! Main call interface.
            CurrentY_MO(const std::string name = "current_y_mo",
                        const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
                logger::mo::Property(name, format)
            {
            }
            std::optional<std::vector<double> > operator()(const logger::MultiObjectiveInfo &log_info) const override
            {
                return std::make_optional(log_info.y_mo);
            }
        };
        /** Objective function value for this call, without transformation.
         *
         * @ingroup Properties
         */
        inline CurrentY_MO current_y_mo;

        /** A property that access the current best value so far, with transformation and constraints applied
         *  A version for the multiobjective optimization.
         *
         * @ingroup Logging
         */
        // struct CurrentBestY_MO : public logger::Property
        // {
        //     //! Constructor.
        //     //! Main call interface.
        //     CurrentBestY_MO(const std::string name = "current_y_best_mo",
        //                     const std::string &format = logger::DEFAULT_DOUBLE_FORMAT) :
        //         logger::Property(name, format)
        //     {
        //     }
        //     std::optional<std::vector<std::vector<double>>>
        //     operator()(const logger::MultiObjectiveInfo &log_info) const override
        //     {
        //         std::vector<std::vector<double>> y_best_mo(log_info.pareto_front.size());
        //         for (auto i = 0; i != log_info.pareto_front.size(); ++i)
        //         {
        //             y_best_mo[i] = std::vector<double>(log_info.pareto_front[i].y);
        //         }
        //         return std::make_optional(y_best_mo);
        //     }
        // };
        /** Objective function value for this call, without transformation.
         *  A version for the multiobjective optimization.
         * @ingroup Properties
         */
        // inline CurrentBestY_MO current_y_best_mo;

        /** A property that access a referenced variable.
         *
         * @note The variable must be castable to a double.
         *
         * @ingroup Logging
         */
        template <class T>
        class Reference : public logger::mo::Property
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
                logger::mo::Property(name, format), _variable(variable)
            {
            }

            //! Main call interface.
            std::optional<std::vector<double> > operator()(const logger::MultiObjectiveInfo &) const override
            {
                return std::make_optional(static_cast<std::vector<double> >(_variable));
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
        template <class T>
        Reference<T> &reference(const std::string name, const T &variable, const std::string &format = logger::DEFAULT_DOUBLE_FORMAT)
        {
            auto p = new Reference<T>(name, variable, format);
            return *p;
        }

        /** A property that access a variable through a pointer.
         *
         * @note The pointed variable must be castable to a double.
         *
         * @ingroup Logging
         */
        template <class T>
        class Pointer : public logger::mo::Property
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
                logger::mo::Property(name, format), _variable(variable)
            {
                assert(variable != nullptr);
            }

            //! Main call interface.
            std::optional<std::vector<double> > operator()(const logger::MultiObjectiveInfo &) const override
            {
                return std::make_optional(static_cast<std::vector<double> >(*_variable));
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
        template <class T>
        Pointer<T> &address(const std::string name, const T *const variable, const std::string &format = logger::DEFAULT_DOUBLE_FORMAT)
        {
            auto p = new Pointer<T>(name, variable, format);
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
        template <class T>
        class PointerReference : public logger::mo::Property
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
                logger::mo::Property(name, format), _ref_ptr_var(ref_ptr_var)
            {
                if (_ref_ptr_var != nullptr)
                {
                    // IOH_DBG(debug, "PointerReference "  name << " @ " << ref_ptr_var << " == " << static_cast<double>(*ref_ptr_var));
                }
                // IOH_DBG(debug, "PointerReference " << name << " @ " << ref_ptr_var << " == nullptr");
            }

            //! Main call interface.
            std::optional<std::vector<double> > operator()(const logger::MultiObjectiveInfo &) const override
            {
                if (_ref_ptr_var != nullptr)
                {
                    // IOH_DBG(debug, "PointerReference " << name() << " @ " << _ref_ptr_var << " == " << static_cast<double>(*_ref_ptr_var));
                    return std::make_optional(static_cast<std::vector<double> >(*_ref_ptr_var));
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
        template <class T>
        PointerReference<T> &pointer(const std::string name, const T *const &variable,
                                     const std::string &format = logger::DEFAULT_DOUBLE_FORMAT)
        {
            auto p = new PointerReference<T>(name, variable, format);
            return *p;
        }


        }

        } // namespace mo

        
    }
}

//! formatter for properties
template <>
struct fmt::formatter<std::reference_wrapper<ioh::logger::mo::Property>> : formatter<std::string>
{
    template <typename FormatContext>
    //! Format call interface
    auto format(const std::reference_wrapper<ioh::logger::mo::Property> &a, FormatContext &ctx)
    {
        return formatter<std::string>::format(a.get().name(), ctx);
    }
};