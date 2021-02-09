#pragma once

#include <ostream>
#include <utility>

#include "configuration.hpp"
#include "ioh/common.hpp"
#include "ioh/logger.hpp"
#include "ioh/suite.hpp"

namespace ioh {
    namespace experiment {
        /**
         * \brief A Class used for running experiments
         * \tparam ProblemType The type of the problems used in the experiments, needs to be a subclass of
         * \ref `ioh::problem::base`.
         */
        template <typename ProblemType>
        class Experimenter {
        public:
            /**
             * \brief A typedef for an optimizer; a function which takes a \ref ioh::problem::base and ioh::logger::csv
             * as arguments.
             */
            typedef void algorithm_type(std::shared_ptr<ProblemType>,
                                        std::shared_ptr<logger::Csv<ProblemType>
                                        >);

            /**
             * \brief A shorthand for accessing the class factory (\ref ioh::common::Factory) for suites.
             */
            typedef common::Factory<suite::base<ProblemType>,
                                    std::vector<int>, std::vector<int>,
                                    std::vector<int>>
            suite_factory;


            Experimenter() = delete;
            Experimenter(const Experimenter &) = delete;
            Experimenter(const Experimenter &&) = delete;
            Experimenter &operator=(const Experimenter &) = delete;
            Experimenter &operator=(const Experimenter &&) = delete;
            ~Experimenter() = default;


            /**
             * \brief Constructs an Experimenter object, from a config file
             * \param config_file_name the path to the configuration file
             * \param algorithm a function pointer of type \ref algorithm_type
             */
            Experimenter(fs::path &config_file_name, algorithm_type *algorithm)
                : conf_(config_file_name), algorithm_(algorithm) {
                auto suite_name = conf_.get_suite_name();
                auto problems = conf_.get_problem_id();
                auto instances = conf_.get_instance_id();
                auto dimensions = conf_.get_dimension();

                suite_ = suite_factory::get().create(suite_name, problems, instances, dimensions);

                if (suite_ == nullptr)
                    common::log::error("Creating suite fails, please check your Configuration");

                csv_logger_ = std::make_shared<logger::Csv<ProblemType>>(conf_);

                if (csv_logger_ == nullptr)
                    common::log::error("Creating logger fails, please check your Configuration");
            }

            /**
             * \brief Constructs an Experimenter object from a Suite object and an Logger object
             * \param suite A suite object
             * \param csv_logger A csv logger object
             * \param algorithm a function pointer of type \ref algorithm_type
             * \param independent_runs the number of repetitions default = 1
             */
            Experimenter(std::shared_ptr<suite::base<ProblemType>> suite,
                         std::shared_ptr<logger::Csv<ProblemType>> csv_logger,
                         algorithm_type *algorithm,
                         const int independent_runs = 1
                )
                : suite_(std::move(suite)),
                  csv_logger_(std::move(csv_logger)),
                  algorithm_(algorithm),
                  independent_runs_(independent_runs) {
            }

            /**
             * \brief Runs the experiment; Evaluates \ref algorithm_ on all problems X instances X dimensions,
             * for N = independent_runs_ repeated runs.
             */
            void run() {
                const auto t_timer = common::CpuTimer("Total ");
                std::cout << *this << std::endl;
                this->csv_logger_->track_suite(suite_->get_suite_name());

                while ((current_problem_ = suite_->get_next_problem()) !=
                       nullptr) {
                    const auto p_timer = common::CpuTimer();
                    std::cout << *current_problem_;
                    for (auto count = 0; independent_runs_ > count; ++count) {
                        csv_logger_->track_problem(*current_problem_);
                        algorithm_(current_problem_, csv_logger_);
                        std::cout << "." << std::flush;
                        current_problem_ = suite_->get_current_problem();
                        // This resets problem
                    }
                }
            }

            /**
             * \brief Set's the number of independent runs to be used in \ref run
             * \param n The number of runs
             */
            void set_independent_runs(const int n) {
                this->independent_runs_ = n;
            }

            /**
             * \brief Get's the number of independent runs to be used in \ref run
             */
            [[nodiscard]] int get_independent_runs() const {
                return this->independent_runs_;
            }

            /**
             * \brief Get method for \ref suite_
             * \return Private \ref suite_
             */
            [[nodiscard]] std::shared_ptr<suite::base<ProblemType>> get_suite() const {
                return suite_;
            }

            /**
             * \brief Get method for \ref current_problem_
             * \return Private \ref current_problem_
             */
            [[nodiscard]] std::shared_ptr<ProblemType> get_current_problem() const {
                return current_problem_;
            }

            /**
             * \brief Get method for \ref csv_logger_
             * \return Private \ref csv_logger_
             */
            [[nodiscard]] std::shared_ptr<logger::Csv<ProblemType>> get_logger() const {
                return csv_logger_;
            }

            /**
             * \brief Prints a verbose representation of an Experimenter object to an output stream.
             * \param os The output stream
             * \param obj The Experimenter object
             * \return The output steam
             */
            friend std::ostream &operator<<(std::ostream &os, const Experimenter &obj) {
                return os << "Experiment"
                       << "\nSuite: " << obj.suite_->get_suite_name()
                       << "\nProblem: " << common::vector_to_string(obj.suite_->get_problem_id())
                       << "\nInstance: " << common::vector_to_string(obj.suite_->get_instance_id())
                       << "\nDimension: " << common::vector_to_string(obj.suite_->get_dimension());
            }

        private:
            /**
             * \brief A configuration object. Only used when Experimenter is instantiated from a config file.
             */
            Configuration conf_;

            /**
             * \brief The benchmark suite used in the Experiment
             */
            std::shared_ptr<suite::base<ProblemType>> suite_;

            /**
             * \brief A pointer to the currently evaluated problem
             */
            std::shared_ptr<ProblemType> current_problem_;

            /**
             * \brief A pointer to the logger. 
             */
            std::shared_ptr<logger::Csv<ProblemType>> csv_logger_;

            /**
             * \brief A function pointer of type \ref algorithm_type
             */
            algorithm_type *algorithm_;

            /**
             * \brief The number of independent runs to be performed
             */
            int independent_runs_ = 1;
        };
    }
}
