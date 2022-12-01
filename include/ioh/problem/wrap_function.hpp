#pragma once

#include "single.hpp"

namespace ioh::problem
{
    /**
     * @brief Shorthand for wrapping function in a problem.
     *
     * @tparam T type of the problem
     * @tparam R the returntype of the problem
     * @param f a function to be wrapped
     * @param name the name for the new function in the registry
     * @param optimization_type the type of optimization
     * @param lb lower bound for the BoxConstraint of the problem
     * @param ub upper bound for the BoxConstraint of the problem
     * @param transform_variables_function function which transforms the variables of the search problem
     * prior to calling f.
     * @param transform_objectives_function a function which transforms the objective value of the search problem
     * after calling f.
     * @param calculate_optimum a function which returns the optimum based on a given problem
     * dimension and instance.
     * @param constraints the constraints
     */
    template <typename T, typename R>
    void wrap_function(ObjectiveFunction<T, R> f, const std::string &name,
                       const common::OptimizationType optimization_type = common::OptimizationType::MIN,
                       const std::optional<T> lb = std::nullopt, const std::optional<T> ub = std::nullopt,
                       std::optional<VariablesTransformationFunction<T>> transform_variables_function = std::nullopt,
                       std::optional<ObjectiveTransformationFunction<R>> transform_objectives_function = std::nullopt,
                       std::optional<CalculateObjectiveFunction<T, R>> calculate_optimum = std::nullopt,
                       ConstraintSet<T> constraints = {})
    {

        if (std::is_same<R, SingleObjective>::value)
        {
            auto &factory = ProblemFactoryType<SingleObjectiveProblem<T>>::instance();

            int id = factory.check_or_get_next_available(1, name);

            auto bounds = BoxConstraint<T>(1, lb.value_or(std::numeric_limits<T>::lowest()),
                                           ub.value_or(std::numeric_limits<T>::max()));

            auto tx = transform_variables_function.value_or(utils::identity<std::vector<T>, int>);
            auto ty = transform_objectives_function.value_or(utils::identity<double, int>);

            factory.include(name, id,
                            [f, name, id, optimization_type, bounds, tx, ty, calculate_optimum,
                             constraints](const int iid, const int dim) {
                                auto optimum = calculate_optimum ? calculate_optimum.value()(iid, dim)
                                                                 : Solution<T, double>(dim, optimization_type);

                                return std::make_unique<SingleObjectiveWrappedProblem<T>>(
                                    f, name, dim, id, iid, optimization_type, bounds, tx, ty, optimum, constraints);
                            });
        }
        else
        {
            throw std::invalid_argument("Not implemented");
        }
    }
}