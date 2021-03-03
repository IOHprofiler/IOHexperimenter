#pragma once

#include "ioh/problem.hpp"

namespace ioh::suite
{
    template <typename ProblemType>
    class Suite
    {
    public:
        using Problem = std::shared_ptr<ProblemType>;
        using Factory = problem::ProblemRegistry<ProblemType>; // TODO: this shou

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
        std::string name_;
        std::vector<Problem> problems_;
        std::vector<int> problem_ids_;
        std::vector<int> instances_;
        std::vector<int> dimensions_;
        logger::Base *logger_{};

        [[nodiscard]]
        int check_parameter(const int parameter, const int ub, const int lb = 1) const
        {
            if (parameter < lb || parameter > ub)
                common::log::error("Parameter value " + std::to_string(parameter) + " is out of bounds");
            return parameter;
        }

    public:
        Suite(const std::vector<int> &problem_ids, const std::vector<int> &instances,
              const std::vector<int> &dimensions, const std::string &name, const int max_problem_id = 1000,
              const int max_instance = 1000, const int max_dimension = 1000
            ) :
            name_(name), problems_(), problem_ids_(problem_ids), instances_(instances), dimensions_(dimensions)
        {
            const auto &factory = Factory::instance();

            for (const auto &problem_id : problem_ids)
                for (const auto &n_variables : dimensions)
                    for (const auto& instance : instances)
                        problems_.emplace_back(factory.create(
                            check_parameter(problem_id, max_problem_id),
                            check_parameter(instance, max_instance),
                            check_parameter(n_variables, max_dimension)
                            ));
        }

        virtual ~Suite() = default;

        void reset()
        {
            if (logger_ != nullptr)
                logger_->flush();
            for (auto &problem : problems_)
                problem.reset();
        }

        void attach_logger(logger::Base &logger)
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
            return name_;
        }
    };

    template <typename ProblemType>
    using SuiteFactoryType = common::RegisterWithFactory<
        Suite<ProblemType>, std::vector<int>, std::vector<int>, std::vector<int>>;


    template <class Derived, class ProblemType>
    using AutomaticSuiteRegistration = common::AutomaticTypeRegistration<Derived, SuiteFactoryType<ProblemType>>;

    template <class ProblemType>
    using SuiteRegistry = SuiteFactoryType<ProblemType>;


    template <class Derived>
    struct RealSuite : Suite<problem::Real>, AutomaticSuiteRegistration<Derived, problem::Real>
    {
        using Suite<problem::Real>::Suite;
    };


    template <class Derived>
    struct IntegerSuite : Suite<problem::Integer>, AutomaticSuiteRegistration<Derived, problem::Integer>
    {
        using Suite<problem::Integer>::Suite;
    };

    struct BBOB final : RealSuite<BBOB>
        // TODO: fix that this somehow use BBOBProblem, instead of Realproblem
    {
        BBOB(const std::vector<int> &problem_ids, const std::vector<int> &instances,
             const std::vector<int> &dimensions) :
            RealSuite(problem_ids, instances, dimensions, "BBOB", 24, 100, 100)
        {
        }
    };

    struct PBO final : IntegerSuite<PBO>
    {
        PBO(const std::vector<int> &problem_ids, const std::vector<int> &instances,
            const std::vector<int> &dimensions) :
            IntegerSuite(problem_ids, instances, dimensions, "PBO", 25, 100, 20000)
        {
        }
    };
}
