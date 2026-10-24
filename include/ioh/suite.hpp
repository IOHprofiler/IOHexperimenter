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
         * @param factory the factory to take the problems from: this ProblemType's, or one of a derived type
         */
        template <typename FactoryType = Factory>
        Suite(const std::vector<int> &problem_ids, const std::vector<int> &instances,
              const std::vector<int> &dimensions, const std::string &name, const int max_instance = 1000,
              const int max_dimension = 1000, FactoryType &factory = Factory::instance()) :
            name_(name),
            problems_(), problem_ids_(problem_ids), instances_(instances), dimensions_(dimensions)
        {
            const auto available_ids = factory.ids();
            const int max_problem_id = *std::max_element(available_ids.begin(), available_ids.end());
            const int min_problem_id = *std::min_element(available_ids.begin(), available_ids.end());
            problem_ids_ = !problem_ids.empty() ? problem_ids : available_ids;
            
            for(size_t i = 0; i < problem_ids_.size(); i++)
                if (std::find(available_ids.begin(), available_ids.end(), problem_ids_[i]) == std::end(available_ids))
                    problem_ids_.erase(problem_ids_.begin() + i--);
            
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


    //! Typedef for Suite factory 
    template <class ProblemType>
    using SuiteRegistry = SuiteRegistryType<ProblemType>;


    /**
     * @brief Registers the suite class T, constructed from (problem ids, instances, dimensions), in the suite factory
     * of its problem type, under its static registry_id and registry_name. A taken id is replaced by the next free one;
     * a name that is already registered keeps its id and its entry is replaced, also a built-in suite's.
     */
    template <typename T>
    void register_suite()
    {
        using ProblemType = typename T::Problem::element_type;
        SuiteFactoryType<ProblemType>::instance().include(
            T::registry_name, T::registry_id,
            [](std::vector<int> &&problem_ids, std::vector<int> &&instances, std::vector<int> &&dimensions) {
                return std::make_shared<T>(problem_ids, instances, dimensions);
            });
    }

    //! Real suite
    struct Real final : Suite<problem::RealSingleObjective>
    {
        //! The id in the suite registry
        static constexpr int registry_id = 1;
        //! The name in the suite registry, and of the suite
        static constexpr const char *registry_name = "Real";

        /**
         * @brief Construct a new RealSingleObjective object
         * 
         * @param problem_ids List of problem ids
         * @param instances List of problem instances
         * @param dimensions List of problem dimensions
         */
        Real(const std::vector<int> &problem_ids, const std::vector<int> &instances,
             const std::vector<int> &dimensions) :
            Suite(problem_ids, instances, dimensions, registry_name)
        {
        }
    };  
    
    
    //! Integer suite
    struct Integer final : Suite<problem::IntegerSingleObjective>
    {
        //! The id in the suite registry
        static constexpr int registry_id = 1;
        //! The name in the suite registry, and of the suite
        static constexpr const char *registry_name = "Integer";

        /**
         * @brief Construct a new Integer object
         * 
         * @param problem_ids List of problem ids
         * @param instances List of problem instances
         * @param dimensions List of problem dimensions
         */
        Integer(const std::vector<int> &problem_ids, const std::vector<int> &instances,
                const std::vector<int> &dimensions) :
            Suite(problem_ids, instances, dimensions, registry_name)
        {
        }
    };

    //! BBOB suite
    struct BBOB final : Suite<problem::RealSingleObjective>
    {
        //! The id in the suite registry
        static constexpr int registry_id = 2;
        //! The name in the suite registry, and of the suite
        static constexpr const char *registry_name = "BBOB";

        /**
         * @brief Construct a new BBOB object
         * 
         * @param problem_ids List of problem ids
         * @param instances List of problem instances (defaults to first instance)
         * @param dimensions List of problem dimensions (defaults to 5D)
         */
        BBOB(const std::vector<int> &problem_ids = {}, const std::vector<int> &instances = {1},
             const std::vector<int> &dimensions = {5}) :
            Suite(problem_ids, instances, dimensions, registry_name, 100, 100,
                      problem::ProblemFactoryType<problem::BBOB>::instance())
        {
        }
    };

    struct SBOX final : Suite<problem::RealSingleObjective>
    {
        //! The id in the suite registry
        static constexpr int registry_id = 3;
        //! The name in the suite registry, and of the suite
        static constexpr const char *registry_name = "SBOX";

        /**
         * @brief Construct a new SBOX object
         * 
         * @param problem_ids List of problem ids
         * @param instances List of problem instances (defaults to first instance)
         * @param dimensions List of problem dimensions (defaults to 5D)
         */
        SBOX(const std::vector<int> &problem_ids = {}, const std::vector<int> &instances = {1},
             const std::vector<int> &dimensions = {5}) :
            Suite(problem_ids, instances, dimensions, registry_name, 100, 100,
                      problem::ProblemFactoryType<problem::SBOX>::instance())
        {
        }
    };

    struct RealStarDiscrepancy final : Suite<problem::RealSingleObjective>
    {
        //! The id in the suite registry
        static constexpr int registry_id = 4;
        //! The name in the suite registry, and of the suite
        static constexpr const char *registry_name = "RealStarDiscrepancy";

        /**
         * @brief Construct a new StarDiscrepancy object
         * 
         * @param problem_ids List of problem ids
         * @param instances List of problem instances (defaults to first instance)
         * @param dimensions List of problem dimensions (defaults to 5D)
         */
        RealStarDiscrepancy(const std::vector<int> &problem_ids = {}, const std::vector<int> &instances = {1},
             const std::vector<int> &dimensions = {5}) :
            Suite(problem_ids, instances, dimensions, registry_name, 10000, 10000,
                      problem::ProblemFactoryType<problem::star_discrepancy::real::StarDiscrepancy>::instance())
        {
        }
    };

    //! PBO suite
    struct PBO final : Suite<problem::IntegerSingleObjective>
    {
        //! The id in the suite registry
        static constexpr int registry_id = 2;
        //! The name in the suite registry, and of the suite
        static constexpr const char *registry_name = "PBO";

        /**
         * @brief Construct a new PBO object
         * 
         * @param problem_ids List of problem ids
         * @param instances List of problem instances (defaults to first instance)
         * @param dimensions List of problem dimensions  (defaults to 16D)
         */
        PBO(const std::vector<int> &problem_ids = {}, const std::vector<int> &instances = {1},
            const std::vector<int> &dimensions = {16}) :
            Suite(problem_ids, instances, dimensions, registry_name, 100, 20000,
                         problem::ProblemFactoryType<problem::PBO>::instance())
        {
        }
    };

    //! Submodular suite
    struct Submodular final : Suite<problem::IntegerSingleObjective>
    {
        //! The id in the suite registry
        static constexpr int registry_id = 3;
        //! The name in the suite registry, and of the suite
        static constexpr const char *registry_name = "Submodular";

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
            Suite(problem_ids, {1}, {1}, registry_name, 1, 2,
                         problem::ProblemFactoryType<problem::submodular::GraphProblem>::instance())
        {
        }
    };

    struct IntegerStarDiscrepancy final : Suite<problem::IntegerSingleObjective>
    {
        //! The id in the suite registry
        static constexpr int registry_id = 4;
        //! The name in the suite registry, and of the suite
        static constexpr const char *registry_name = "IntegerStarDiscrepancy";

        /**
         * @brief Construct a new StarDiscrepancy object
         * 
         * @param problem_ids List of problem ids
         * @param instances List of problem instances (defaults to first instance)
         * @param dimensions List of problem dimensions (defaults to 5D)
         */
        IntegerStarDiscrepancy(const std::vector<int> &problem_ids = {}, const std::vector<int> &instances = {1},
             const std::vector<int> &dimensions = {5}) :
            Suite(problem_ids, instances, dimensions, registry_name, 10000, 10000,
                      problem::ProblemFactoryType<problem::star_discrepancy::integer::StarDiscrepancy>::instance())
        {
        }
    };
} // namespace ioh::suite
