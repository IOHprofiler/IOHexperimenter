#pragma once

#include "ioh/problem.hpp"

namespace ioh::suite
{

    template<typename ProblemType>
    class Suite
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
        Suite(const std::vector<int>& problem_ids, const std::vector<int>& instances, const std::vector<int>& dimensions,
              const int max_problem_id, const int max_instance, const int max_dimension
            ) :
            problems_(), problem_ids_(problem_ids), instances_(instances), dimensions_(dimensions)
        {
            const auto& factory = problem::Factory<ProblemType>::instance();

            for (const auto& problem_id : problem_ids)
                for (const auto& instance : instances)
                    for (const auto& n_variables : dimensions)
                        problems_.emplace_back(factory.create(
                            check_parameter(problem_id, max_problem_id),
                            check_parameter(instance, max_instance),
                            check_parameter(n_variables, max_dimension)
                        ));
        }

        void reset()
        {
            for (auto& problem : problems_)
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
        std::vector<int> problem_ids() const
        {
            return problem_ids_;
        }

        [[nodiscard]]
        std::vector<int> dimensions() const 
        {
            return dimensions_;
        }

        [[nodiscard]]
        std::vector<int> instances() const
        {
            return instances_;
        }

        [[nodiscard]]
        std::string name() const
        {
            return common::class_name<ProblemType>();
        }
    };
    
    class BBOBSuite final: public Suite<problem::BBOB>
    {
    public:
        BBOBSuite(const std::vector<int>& problem_ids, const std::vector<int>& instances, const std::vector<int>& dimensions):
            Suite(problem_ids, instances, dimensions, 24, 100, 100)
        {
        }
    };

    class PBOSuite final : public Suite<problem::PBO>
    {
    public:
        PBOSuite(const std::vector<int>& problem_ids, const std::vector<int>& instances, const std::vector<int>& dimensions) :
            Suite(problem_ids, instances, dimensions, 25, 100, 20000)
        {
        }
    };
}

