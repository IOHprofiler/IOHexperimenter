#pragma once

#include "ioh/problem.hpp"

namespace ioh::suite
{
    template <typename ProblemType>
    class Suite
    {
    public:
        using Problem = std::shared_ptr<ProblemType>;

        struct Iterator
        {
            using ValueType = typename std::vector<Problem>::value_type;
            using PointerType = ValueType *;
            using ReferenceType = ValueType &;
            Suite *suite;

            explicit Iterator(PointerType p, Suite *s, const bool track_problems = true):
                suite(s), ptr(p), begin(s->problems_.data()),
                end(s->problems_.data() + s->problems_.size()),
                track_problems(track_problems)
            {
                track_problem();
            }

            void track_problem() const
            {
                if (track_problems && ptr != end && suite->logger_ != nullptr)
                    (*ptr)->attach_logger(*suite->logger_);
            }

            Iterator &operator++()
            {
                ++ptr;
                track_problem();
                return *this;
            }

            Iterator &operator++(int)
            {
                auto it = *this;
                ++(*this);
                return it;
            }

            ReferenceType operator[](int index)
            {
                return *(ptr + index);
            }

            PointerType operator->()
            {
                return ptr;
            }

            ReferenceType operator*()
            {
                return *ptr;
            }

            bool operator==(const Iterator &other)
            {
                return ptr == other.ptr;
            }

            bool operator!=(const Iterator &other)
            {
                return !(*this == other);
            }

        private:
            PointerType ptr;
            PointerType begin;
            PointerType end;
            bool track_problems;
        };

    private:
        std::vector<Problem> problems_;
        std::vector<int> problem_ids_;
        std::vector<int> instances_;
        std::vector<int> dimensions_;
        logger::Base* logger_{};

        [[nodiscard]]
        int check_parameter(const int parameter, const int ub, const int lb = 1) const
        {
            if (parameter < lb || parameter > ub)
                common::log::error("Parameter value " + std::to_string(parameter) + " is out of bounds");
            return parameter;
        }

    public:
        Suite(const std::vector<int> &problem_ids, const std::vector<int> &instances,
              const std::vector<int> &dimensions, const int max_problem_id = 100,
              const int max_instance = 1000, const int max_dimension = 1000
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

        virtual ~Suite()
        {
            if (logger_ != nullptr)
                logger_->flush();
        }

        void reset()
        {
            if (logger_ != nullptr)
                logger_->flush();
            for (auto &problem : problems_)
                problem.reset();
        }

        void attach_logger(logger::Base& logger)
        {
            logger_ = &logger;
            logger_->track_suite(name());
        }

        void detach_logger()
        {
            if (logger_ != nullptr)
                logger_->flush();
            logger_ = nullptr;
        }

        [[nodiscard]]
        Iterator begin(const bool track_problems = true)
        {
            return Iterator(problems_.data(), this, track_problems);
        }

        [[nodiscard]]
        Iterator end()
        {
            return Iterator(problems_.data() + problems_.size(), this);
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


    template <class Derived, class Parent>
    struct AutomaticSuiteRegistration : common::AutomaticTypeRegistration<
            Derived, common::RegisterWithFactory<Parent, std::vector<int>, std::vector<int>, std::vector<int>>>
    {
    };

    template <class Parent>
    struct SuiteRegistry : common::RegisterWithFactory<Parent, std::vector<int>, std::vector<int>, std::vector<int>>
    {
    };

    struct RealSuite : Suite<problem::RealProblem>
    {
        using Suite<problem::RealProblem>::Suite;
    };

    struct IntegerSuite : Suite<problem::IntegerProblem>
    {
        using Suite<problem::IntegerProblem>::Suite;
    };


    template <class Derived>
    struct RealSuiteBase : RealSuite, AutomaticSuiteRegistration<Derived, RealSuite>
    {
        using RealSuite::RealSuite;
    };

    template <class Derived>
    struct IntegerSuiteBase : IntegerSuite, AutomaticSuiteRegistration<Derived, IntegerSuite>
    {
        using IntegerSuite::IntegerSuite;
    };

    struct BBOB final : RealSuiteBase<BBOB>
    {
        BBOB(const std::vector<int> &problem_ids, const std::vector<int> &instances,
             const std::vector<int> &dimensions) :
            RealSuiteBase(problem_ids, instances, dimensions, 24, 100, 100)
        {
        }
    };

    struct PBO final : IntegerSuiteBase<PBO>
    {
        PBO(const std::vector<int> &problem_ids, const std::vector<int> &instances,
            const std::vector<int> &dimensions) :
            IntegerSuiteBase(problem_ids, instances, dimensions, 25, 100, 20000)
        {
        }
    };
}
