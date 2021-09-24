#pragma once

#include <functional>
#include <memory>

#include "ioh/common/timer.hpp"
#include "ioh/logger.hpp"
#include "ioh/suite.hpp"

namespace ioh
{
    /**
     * \brief A Class used for running experiments
     * \tparam ProblemType The type of the problems used in the experiments, needs to be a subclass of
     * \ref ioh::problem::Problem.
     */
    template <typename ProblemType>
    class Experimenter
    {
    public:
        /**
         * \brief A typedef for an optimizer; a function which takes a \ref ioh::problem::Problem
         */
        using Algorithm = std::function<void(std::shared_ptr<ProblemType>)>;

    private:
        /**
         * \brief The benchmark suite used in the Experiment 
         */
        std::shared_ptr<suite::Suite<ProblemType>> suite_;

        /**
         * \brief A pointer to the logger.
         */
        std::shared_ptr<Logger> logger_;

        /**
         * \brief A function pointer of type \ref algorithm_type
         */
        Algorithm algorithm_;

        /**
         * \brief The number of independent runs to be performed
         */
        int independent_runs_ = 1;

    public:
        Experimenter() = delete;
        Experimenter(const Experimenter &) = delete;
        Experimenter(const Experimenter &&) = delete;
        Experimenter &operator=(const Experimenter &) = delete;
        Experimenter &operator=(const Experimenter &&) = delete;
        ~Experimenter() = default;

        /**
         * \brief Constructs an Experimenter object from a Suite object and an Logger object
         * \param suite A suite object
         * \param logger A csv logger object
         * \param algorithm a function pointer of type \ref Algorithm
         * \param independent_runs the number of repetitions default = 1
         */
        Experimenter(std::shared_ptr<suite::Suite<ProblemType>> suite, std::shared_ptr<Logger> logger,
                     Algorithm algorithm = nullptr, const int independent_runs = 1) :
            suite_(std::move(suite)),
            logger_(std::move(logger)), algorithm_(algorithm), independent_runs_(independent_runs)
        {
        }

        /**
         * \brief Runs the experiment; Evaluates `algorithm_` on all problems X instances X dimensions,
         * for N = independent_runs_ repeated runs.
         */
        void run()
        {
            suite_ ->attach_logger(*logger_); 
            for (const auto &p : *suite_)
            {
                const auto p_timer = common::CpuTimer();
                for (auto count = 0; count < independent_runs_; ++count)
                {
                    algorithm_(p);
                    p->reset();
                }
            }
        }

        /**
         * \brief Set's the number of independent runs to be used in \ref run
         * \param n The number of runs
         */
        void independent_runs(const int n) { this->independent_runs_ = n; }

        /**
         * \brief Get's the number of independent runs to be used in \ref run
         */
        [[nodiscard]] int independent_runs() const { return this->independent_runs_; }

        /**
         * \brief Get method for `suite_`
         * \return Private `suite_`
         */
        [[nodiscard]] std::shared_ptr<suite::Suite<ProblemType>> suite() const { return suite_; }

        /**
         * \brief Get method for
         * \return Private 
         */
        [[nodiscard]] std::shared_ptr<Logger> logger() const { return logger_; }

        /**
         * \brief Prints a verbose representation of an Experimenter object to an output stream.
         * \param os The output stream
         * \param obj The Experimenter object
         * \return The output steam
         */
        friend std::ostream &operator<<(std::ostream &os, const Experimenter &obj)
        {
            return os << fmt::format("Experiment\nSuite: {}\nProblems: {}\nInstances: {}\n Dimensions: {}",
                                     obj.suite_->name, fmt::join(obj.suite_->problem_ids(), ", "),
                                     fmt::join(obj.suite_->instances(), ", "),
                                     fmt::join(obj.suite_->dimensions(), ", "));
        }
    };
} // namespace ioh
