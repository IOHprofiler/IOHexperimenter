#pragma once

#include <any>

#include "ioh/problem.hpp"

namespace ioh::suite
{
    struct SuiteBase
    {
        [[nodiscard]] virtual std::vector<int> problem_ids() const = 0;
        [[nodiscard]] virtual std::vector<int> dimensions() const = 0;
        [[nodiscard]] virtual std::vector<int> instances() const = 0;
    };


    template <typename ProblemType>
    class Suite : public SuiteBase
    {
        using Problem = std::shared_ptr<ProblemType>;

        std::vector<Problem> problems_;

        std::vector<int> problem_ids_;
        std::vector<int> instances_;
        std::vector<int> dimensions_;


        [[nodiscard]]
        int check_parameter(const int parameter, const int ub, const int lb = 1) const
        {
            if (parameter < lb || parameter > ub)
                common::log::error("Parameter value " + std::to_string(parameter) + " is out of bounds");
            return parameter;
        }

    public:
        Suite(const std::vector<int> &problem_ids, const std::vector<int> &instances,
              const std::vector<int> &dimensions,
              const int max_problem_id = 100, const int max_instance = 1000, const int max_dimension = 1000
            ) :
            problems_(), problem_ids_(problem_ids), instances_(instances), dimensions_(dimensions)
        {
            const auto &factory = problem::ProblemRegistry<ProblemType>::instance();

            for (const auto &problem_id : problem_ids)
                for (const auto &instance : instances)
                    for (const auto &n_variables : dimensions)
                        problems_.emplace_back(factory.create(
                            check_parameter(problem_id, max_problem_id),
                            check_parameter(instance, max_instance),
                            check_parameter(n_variables, max_dimension)
                            ));
        }

        void reset()
        {
            for (auto &problem : problems_)
                problem.reset();
        }

        typename std::vector<Problem>::iterator begin()
        {
            return problems_.begin();
        }

        typename std::vector<Problem>::iterator end()
        {
            return problems_.end();
        }

        [[nodiscard]]
        std::vector<Problem> problems() const
        {
            return problems_;
        }

        [[nodiscard]]
        std::vector<int> problem_ids() const override
        {
            return problem_ids_;
        }

        [[nodiscard]]
        std::vector<int> dimensions() const override
        {
            return dimensions_;
        }

        [[nodiscard]]
        std::vector<int> instances() const override
        {
            return instances_;
        }

        [[nodiscard]]
        std::string name() const
        {
            return common::class_name<ProblemType>();
        }
    };


    template <class Derived>
    struct AutomaticSuiteRegistration : common::RegistrationInvoker<
            Derived, common::RegisterWithFactory<SuiteBase, std::vector<int>, std::vector<int>, std::vector<int>>>
    {
    };

    struct SuiteRegistry : common::RegisterWithFactory<SuiteBase, std::vector<int>, std::vector<int>, std::vector<int>>
    {
    };

    template <typename ProblemType>
    class AbstractSuite : public Suite<ProblemType>, AutomaticSuiteRegistration<Suite<ProblemType>>
    {
    public:
        using Suite<ProblemType>::Suite;
    };


    class BBOBSuite final : public AbstractSuite<problem::BBOB>
    {
    public:
        BBOBSuite(const std::vector<int> &problem_ids, const std::vector<int> &instances,
                  const std::vector<int> &dimensions) :
            AbstractSuite(problem_ids, instances, dimensions, 24, 100, 100)
        {
        }
    };

    class PBOSuite final : public AbstractSuite<problem::PBO>
    {
    public:
        PBOSuite(const std::vector<int> &problem_ids, const std::vector<int> &instances,
                 const std::vector<int> &dimensions) :
            AbstractSuite(problem_ids, instances, dimensions, 25, 100, 20000)
        {
        }
    };
}
