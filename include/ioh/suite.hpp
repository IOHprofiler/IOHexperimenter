#pragma once

#include "ioh/problem.hpp"


//! Suite namespace
namespace ioh::suite
{
    //! Suite for ProblemType problems
    template <typename ProblemType>
    class Suite
    {
    public:
        //! Typedef to ProblemType ptr
        using Problem = std::shared_ptr<ProblemType>;

        //! //! Typedef to ProblemType Factory
        using Factory = problem::ProblemFactoryType<ProblemType>;

        //! Iterator for problems
        struct Iterator
        {
            //! Problem type
            using ValueType = typename std::vector<Problem>::value_type;

            //! Problem type *
            using PointerType = ValueType *;

            //! Problem type &
            using ReferenceType = ValueType &;

            //! Suite ptr
            Suite *suite;

            /**
             * @brief Construct a new Iterator object
             *
             * @param p current ptr
             * @param s suite ptr
             * @param track_problems whether or not to track problems
             */
            explicit Iterator(PointerType p, Suite *s, const bool track_problems = true) :
                suite(s), ptr(p), begin(s->problems_.data()), end(s->problems_.data() + s->problems_.size()),
                track_problems(track_problems)
            {
                track_problem();
            }


            //! Track the next problem
            void track_problem() const
            {
                if (track_problems && ptr != end && suite->logger_ != nullptr)
                    (*ptr)->attach_logger(*suite->logger_);
            }

            //! Advance the iterator
            Iterator &operator++()
            {
                ++ptr;
                track_problem();
                return *this;
            }

            //! Advance the iterator
            Iterator &operator++(int)
            {
                Iterator it(*this);
                ++(*this);
                return it;
            }

            //! Index
            ReferenceType operator[](int index) { return *(ptr + index); }

            //! call
            PointerType operator->() { return ptr; }

            //! call
            ReferenceType operator*() { return *ptr; }

            //! Comparison operator
            bool operator==(const Iterator &other) { return ptr == other.ptr; }

            //! Comparison operator
            bool operator!=(const Iterator &other) { return !(*this == other); }

        private:
            PointerType ptr;
            PointerType begin;
            PointerType end;
            bool track_problems;
        };

    private:
        //! Name of the suite
        std::string name_;

        //! List of problems
        std::vector<Problem> problems_;

        //! List of problem ids
        std::vector<int> problem_ids_;

        //! List of problem instances
        std::vector<int> instances_;

        //! List of problem dimensions
        std::vector<int> dimensions_;

        //! Attached logger
        Logger *logger_{};

        //! Check if parameter is within bounds
        [[nodiscard]] int check_parameter(const int parameter, const int ub, const int lb = 1) const
        {
            if (parameter < lb || parameter > ub)
            {
                IOH_DBG(error, "Parameter value " << parameter << " is out of bounds")
                assert(lb <= parameter and parameter <= ub);
            }
            return parameter;
        }

    public:
        /**
         * @brief Construct a new Suite object
         *
         * @param problem_ids List of problem ids
         * @param instances List of problem ids
         * @param dimensions List of problem instances
         * @param name The name of the suite
         * @param max_instance the maximum instance
         * @param max_dimension the maximum dimension
         * @param factory factory instance
         */
        Suite(const std::vector<int> &problem_ids, const std::vector<int> &instances,
              const std::vector<int> &dimensions, const std::string &name, const int max_instance = 1000,
              const int max_dimension = 1000, Factory &factory = Factory::instance()) :
            name_(name),
            problems_(), problem_ids_(problem_ids), instances_(instances), dimensions_(dimensions)
        {
            const auto available_ids = factory.ids();
            const int max_problem_id = *std::max_element(available_ids.begin(), available_ids.end());
            const int min_problem_id = *std::min_element(available_ids.begin(), available_ids.end());
            problem_ids_ = !problem_ids.empty() ? problem_ids : available_ids;
            

            for (const auto &problem_id : problem_ids_)
                for (const auto &n_variables : dimensions)
                    for (const auto &instance : instances)
                        problems_.emplace_back(factory.create(
                            check_parameter(problem_id, max_problem_id, min_problem_id),
                            check_parameter(instance, max_instance), check_parameter(n_variables, max_dimension)));
        }

        virtual ~Suite() = default;


        //! reset all problems in the suite
        void reset()
        {
            if (logger_ != nullptr)
                logger_->reset();
            for (auto &problem : problems_)
                problem.reset();
        }

        //! Attach a logger
        void attach_logger(Logger &logger)
        {
            logger_ = &logger;
            logger_->attach_suite(name());
        }


        //! Detach a logger
        void detach_logger()
        {
            if (logger_ != nullptr)
                logger_->reset();
            logger_ = nullptr;
        }

        //! start iteration
        [[nodiscard]] Iterator begin(const bool track_problems = true)
        {
            return Iterator(problems_.data(), this, track_problems);
        }

        //! end iteration
        [[nodiscard]] Iterator end() { return Iterator(problems_.data() + problems_.size(), this); }

        //! Accessor for problem_ids_
        [[nodiscard]] std::vector<int> problem_ids() const { return problem_ids_; }

        //! Accessor for dimensions_
        [[nodiscard]] std::vector<int> dimensions() const { return dimensions_; }

        //! Accessor for instances_
        [[nodiscard]] std::vector<int> instances() const { return instances_; }

        //! Accessor for name_
        [[nodiscard]] std::string name() const { return name_; }

        //! Accessor for size
        [[nodiscard]] size_t size() const { return problem_ids_.size() * instances_.size() * dimensions_.size(); }
    };

    
    //! Typedef for Suite registry type
    template <typename ProblemType>
    using SuiteRegistryType =
        common::RegisterWithFactory<Suite<ProblemType>, std::vector<int>, std::vector<int>, std::vector<int>>;


    //! Typedef for Suite factory type
    template <typename ProblemType>
    using SuiteFactoryType = common::Factory<Suite<ProblemType>, std::vector<int>, std::vector<int>, std::vector<int>>;


    //! Typedef for Suite Registration helper
    template <class Derived, class ProblemType>
    using AutomaticSuiteRegistration = common::AutomaticTypeRegistration<Derived, SuiteRegistryType<ProblemType>>;

    //! Typedef for Suite factory 
    template <class ProblemType>
    using SuiteRegistry = SuiteRegistryType<ProblemType>;


    //! Base class for Real suites
    template <class Derived>
    struct RealSuite : Suite<problem::Real>, AutomaticSuiteRegistration<Derived, problem::Real>
    {
        using Suite<problem::Real>::Suite;
    };

    //! Base class for Integer suites
    template <class Derived>
    struct IntegerSuite : Suite<problem::Integer>, AutomaticSuiteRegistration<Derived, problem::Integer>
    {
        using Suite<problem::Integer>::Suite;
    };

    //! Real suite
    struct Real final : RealSuite<Real>
    {
        /**
         * @brief Construct a new Real object
         * 
         * @param problem_ids List of problem ids
         * @param instances List of problem instances
         * @param dimensions List of problem dimensions
         */
        Real(const std::vector<int> &problem_ids, const std::vector<int> &instances,
             const std::vector<int> &dimensions) :
            RealSuite(problem_ids, instances, dimensions, "Real")
        {
        }
    };  
    
    
    //! Integer suite
    struct Integer final : IntegerSuite<Integer>
    {
        /**
         * @brief Construct a new Integer object
         * 
         * @param problem_ids List of problem ids
         * @param instances List of problem instances
         * @param dimensions List of problem dimensions
         */
        Integer(const std::vector<int> &problem_ids, const std::vector<int> &instances,
                const std::vector<int> &dimensions) :
            IntegerSuite(problem_ids, instances, dimensions, "Integer")
        {
        }
    };

    //! BBOB suite
    struct BBOB final : RealSuite<BBOB>
    {
        /**
         * @brief Construct a new BBOB object
         * 
         * @param problem_ids List of problem ids
         * @param instances List of problem instances (defaults to first instance)
         * @param dimensions List of problem dimensions (defaults to 5D)
         */
        BBOB(const std::vector<int> &problem_ids = {}, const std::vector<int> &instances = {1},
             const std::vector<int> &dimensions = {5}) :
            RealSuite(problem_ids, instances, dimensions, "BBOB", 100, 100,
                      reinterpret_cast<Factory &>(problem::ProblemFactoryType<problem::BBOB>::instance()))
        {
        }
    };

    //! PBO suite
    struct PBO final : IntegerSuite<PBO>
    {
        /**
         * @brief Construct a new PBO object
         * 
         * @param problem_ids List of problem ids
         * @param instances List of problem instances (defaults to first instance)
         * @param dimensions List of problem dimensions  (defaults to 16D)
         */
        PBO(const std::vector<int> &problem_ids = {}, const std::vector<int> &instances = {1},
            const std::vector<int> &dimensions = {16}) :
            IntegerSuite(problem_ids, instances, dimensions, "PBO", 100, 20000,
                         reinterpret_cast<Factory &>(problem::ProblemFactoryType<problem::PBO>::instance()))
        {
        }
    };


    struct Submodular final : IntegerSuite<Submodular>
    {
        /**
         * @brief Construct a new Submodular object
         * 
         * @param problem_ids List of problem ids
         * @param instances List of problem instances (ignored)
         * @param dimensions List of problem dimensions (ignored)
         */
        Submodular(const std::vector<int> &problem_ids = {},
            [[maybe_unused]] const std::vector<int> &instances = {},
            [[maybe_unused]] const std::vector<int> &dimensions = {}) :
            IntegerSuite(problem_ids, {1}, {1}, "Submodular", 1, 2,
                         reinterpret_cast<Factory &>(problem::ProblemFactoryType<problem::submodular::GraphProblem>::instance()))
        {
        }

    };
} // namespace ioh::suite
