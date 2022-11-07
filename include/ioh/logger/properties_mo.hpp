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
                        std::string s = "";
                        for (auto p = opt->begin(); p != opt->end(); ++p)
                        {
                            s += fmt::format(format());
                        }
                        return s;
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
        }

        } // namespace mo

        
    }
}