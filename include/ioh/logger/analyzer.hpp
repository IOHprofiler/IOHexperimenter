#pragma once

#include "ioh/logger/flatfile.hpp"


namespace ioh::logger
{
    namespace analyzer
    {
        namespace structures
        {
            template <typename T>
            struct Attribute : common::HasRepr
            {
                std::string name;
                T value;

                Attribute(const std::string &name, const T &value) : name(name), value(value) {}

                std::string repr() const override { return fmt::format("\"{}\": {}", name, value); }
            };

            template <>
            std::string Attribute<std::string>::repr() const
            {
                return fmt::format("\"{}\": \"{}\"", name, value);
            }

            using StringMap = std::map<std::string, std::string>;
            using dPtrMap = std::map<std::string, double *>;

            //! Struct containing metadata for a given experiment.
            struct Attributes
            {
                //! List of attributes constant per experiment
                StringMap experiment;

                //! List of attributes constant per run
                dPtrMap run;
            };

            struct BestPoint : common::HasRepr
            {
                size_t evals;
                problem::Solution<double> point;

                BestPoint(const size_t evals, const problem::Solution<double> &point) : evals(evals), point(point) {}

                std::string repr() const override
                {
                    return fmt::format("\"evals\": {}, \"y\": {}, \"x\": [{}]", evals, point.y,
                                       fmt::join(point.x, ", "));
                }
            };

            struct AlgorithmInfo : common::HasRepr
            {
                const std::string name;
                const std::string info;

                AlgorithmInfo(const std::string &name, const std::string &info) : name(name), info(info) {}

                std::string repr() const override
                {
                    return fmt::format("\"name\": \"{}\", \"info\": \"{}\"", name, info);
                }
            };

            struct RunInfo : common::HasRepr
            {
                const size_t instance;
                const size_t evals;
                const BestPoint best_point;
                const std::vector<Attribute<double>> attributes;

                RunInfo(const size_t instance, const size_t evals, const BestPoint &bp,
                        const std::vector<Attribute<double>> &ra = {}) :
                    instance(instance),
                    evals(evals), best_point(bp), attributes(ra)
                {
                }

                std::string repr() const override
                {
                    return fmt::format("\"instance\": {}, \"evals\": {}, \"best\": {{{}}}{}", instance, evals,
                                       best_point,
                                       (attributes.empty() ? "" : fmt::format(", {}", fmt::join(attributes, ", "))));
                }
            };

            struct DimensionInfo : common::HasRepr
            {
                const size_t dimension;
                const std::string data_file;
                std::vector<RunInfo> runs;

                DimensionInfo(const size_t dimension, const std::string &data_file,
                              const std::vector<RunInfo> runs = {}) :
                    dimension(dimension),
                    data_file(data_file), runs(runs)
                {
                }

                std::string repr() const override
                {
                    return fmt::format(
                        "\"dimension\": {},\n\t\t\"path\": \"{}\",\n\t\t\"runs\": [\n\t\t\t{{{}}}\n\t\t]", dimension,
                        data_file, fmt::join(runs, "},\n\t\t\t{"));
                }
            };

            struct ExperimentInfo : common::HasRepr
            {
                const std::string suite;
                const problem::MetaData problem;
                const AlgorithmInfo algorithm;
                const std::vector<Attribute<std::string>> attributes;
                const std::vector<std::string> run_attribute_names;
                const std::vector<std::string> extra_attribute_names;
                std::vector<DimensionInfo> dims;

                ExperimentInfo(const std::string &suite, const problem::MetaData &problem,
                               const AlgorithmInfo &algorithm,
                               const std::vector<Attribute<std::string>> &attributes = {},
                               const std::vector<std::string> &run_attribute_names = {},
                               const std::vector<std::string> &extra_attribute_names = {},
                               const std::vector<DimensionInfo> &dims = {}) :
                    suite(suite),
                    problem(problem), algorithm(algorithm), attributes(attributes),
                    run_attribute_names(run_attribute_names), extra_attribute_names(extra_attribute_names), dims(dims)
                {
                }

                std::string repr() const override
                {
                    return fmt::format(
                        "{{\n\t\"suite\": \"{}\", \n\t\"function_id\": {}, \n\t\"function_name\": \"{}\", "
                        "\n\t\"maximization\": "
                        "{}, \n\t\"algorithm\": {{{}}},{}{}{}\n\t\"dimensions\": [\n\t\t{{{}}}\n\t]\n}}\n",
                        suite, problem.problem_id, problem.name,
                        (problem.optimization_type == common::OptimizationType::Maximization), algorithm,
                        !attributes.empty() ? fmt::format("\n\t{},", fmt::join(attributes, ",\n\t")) : "",
                        !run_attribute_names.empty()
                            ? fmt::format("\n\t\"run_attributes\": [\"{}\"],", fmt::join(run_attribute_names, "\", \""))
                            : "",
                        !extra_attribute_names.empty() ? fmt::format("\n\t\"extra_attributes\": [\"{}\"],",
                                                                     fmt::join(extra_attribute_names, "\", \""))
                                                       : "",
                        fmt::join(dims, "},\n\t\t{"));
                }
            };

        } // namespace structures

        namespace v1
        {
            /** A logger that stores information in a format supported by the IOHAnalyzer platform.
             * TODO: check if we log the last line
             * @code
                logger::Analyzer(
                    {trigger::always},
                    {watch::evaluations, watch::transformed_y},
                    "~/data",
                    "experiment_folder",
                    "MyOptimizer",
                    "Version 1.0"
                );
            * @endcode
            *
            * @ingroup Loggers
            */
            class Analyzer : public FlatFile
            {
            protected:
                const common::file::UniqueFolder path_;
                const structures::AlgorithmInfo algorithm_;
                std::ofstream info_stream_;
                structures::BestPoint best_point_;
                structures::Attributes attributes_;
                bool has_started_;
                logger::Info log_info_{};
                size_t evals_;

                //! Gets called when a new problem is attached
                virtual void handle_new_problem(const problem::MetaData &problem)
                {
                    if (info_stream_.is_open())
                        info_stream_.close();
                    info_stream_ = std::ofstream(
                        path_ / fmt::format("IOHprofiler_f{:d}_{}.info", problem.problem_id, problem.name));
                }

                //! Gets called after the last evaluation of a run
                virtual void handle_last_eval()
                {
                    info_stream_ << fmt::format(", {:d}:{:d}|{:g}", problem_->instance, best_point_.evals,
                                                best_point_.point.y);
                    for (auto &[key, value] : attributes_.run)
                        info_stream_ << fmt::format(";{:g}", *value);

                    if (log_info_.evaluations != 0)
                        FlatFile::call(log_info_);
                }

                //! Gets called when the current problem changes dimension
                virtual void handle_new_dimension(const problem::MetaData &problem, const std::string &dat_path)
                {
                    if (info_stream_.tellp() != 0)
                        info_stream_ << "\n";

                    info_stream_ << "suite = \"" << current_suite_ << "\", funcId = " << problem.problem_id
                                 << ", funcName = \"" << problem.name << "\", DIM = " << problem.n_variables
                                 << ", maximization = \""
                                 << (problem.optimization_type == common::OptimizationType::Maximization ? 'T' : 'F')
                                 << "\", algId = \"" << algorithm_.name << "\", algInfo = \"" << algorithm_.info << '"';

                    // extra attrs const per experiment
                    for (const auto &[fst, snd] : attributes_.experiment)
                        info_stream_ << ", " << fst << " = \"" << snd << '"';

                    // extra attrs const per run
                    if (!attributes_.run.empty())
                        info_stream_ << fmt::format(", dynamicAttribute = \"{}\"",
                                                    fmt::join(common::keys(attributes_.run), "|"));

                    info_stream_ << "\n%\n" << dat_path;
                }

            private:
                void update_info_file(const problem::MetaData &problem, const std::string &dat_path)
                {
                    if (problem_ != nullptr && best_point_.evals != 0)
                        handle_last_eval();

                    if (problem_ == nullptr || problem_->problem_id != problem.problem_id)
                        handle_new_problem(problem);

                    if (problem_ == nullptr || problem.n_variables != problem_->n_variables ||
                        problem.problem_id != problem_->problem_id)
                        handle_new_dimension(problem, dat_path);

                    info_stream_.flush();
                }

                void guard_attributes(const std::function<void()> &f)
                {
                    if (!has_started_)
                        return f();
                    IOH_DBG(warning, "cannot change attributes after experiment has started")
                }

            public:
                /** Logger formatting data in a format supported by iohprofiler.
                 *
                 * @param triggers When to fire a log event.
                 * @param properties What to log.
                 * @param root Path in which to store the data.
                 * @param folder_name Name of folder in which to store data. Will be created as a subdirectory of
                 * `root`.
                 * @param algorithm_name The string separating fields.
                 * @param algorithm_info The string indicating a comment.
                 * @param store_positions Whether to store x positions in the logged data
                 * @param attributes See: analyzer::Attributes.
                 */
                Analyzer(const Triggers &triggers = {trigger::on_improvement}, const Properties &properties = {},
                         const fs::path &root = fs::current_path(), const std::string &folder_name = "ioh_data",
                         const std::string &algorithm_name = "algorithm_name",
                         const std::string &algorithm_info = "algorithm_info", const bool store_positions = false,
                         const structures::Attributes &attributes = {}) :
                    FlatFile(triggers, common::concatenate(default_properties_, properties), "", {}, " ", "", "None",
                             "\n", true, store_positions, {}),
                    path_(root, folder_name), algorithm_(algorithm_name, algorithm_info), best_point_{0, {{}}},
                    attributes_(attributes), has_started_(false)
                {
                }

                virtual ~Analyzer()
                {
                    IOH_DBG(debug, "close info file")
                    info_stream_.close();
                }

                //! Part of public interface. Updates info file.
                virtual void attach_problem(const problem::MetaData &problem) override
                {
                    const auto dat_directory = fmt::format("data_f{:d}_{}", problem.problem_id, problem.name);
                    const auto dat_filename =
                        fmt::format("IOHprofiler_f{:d}_DIM{:d}.dat", problem.problem_id, problem.n_variables);

                    update_info_file(problem, fmt::format("{}/{}", dat_directory, dat_filename));
                    open_stream(dat_filename, path_ / dat_directory);
                    FlatFile::attach_problem(problem);
                    best_point_ = {0, {std::vector<double>(problem.n_variables), problem.initial_objective_value}};
                    has_started_ = true;
                }

                //! Set log_info_ in order to check if the last line has been logged
                virtual void log(const logger::Info &log_info) override
                {
                    log_info_ = log_info;
                    FlatFile::log(log_info);
                }

                //! See: logger::FlatFile::call. Updates `best_point_`
                virtual void call(const Info &log_info) override
                {
                    evals_ = log_info.evaluations;
                    FlatFile::call(log_info);
                    if (compare_objectives(log_info.current.y, best_point_.point.y, problem_->optimization_type))
                        best_point_ = {log_info.evaluations, log_info.current.as_double()};
                    log_info_ = {};
                }

                //! Watcher::watch is protected, so it can only be called before track_problem is called for the first
                //! time.
                virtual void watch(logger::Property &property) override
                {
                    guard_attributes([&]() { FlatFile::watch(property); });
                }

                //! Adds an experiment attribute. Can only be called before track_problem is called for the first time.
                void add_experiment_attribute(const std::string &name, const std::string &value)
                {
                    guard_attributes([&]() { attributes_.experiment.insert_or_assign(name, value); });
                }

                //! Sets experiment attributes. Can only be called before track_problem is called for the first time.
                void set_experiment_attributes(const structures::StringMap &map)
                {
                    guard_attributes([&]() { attributes_.experiment = map; });
                }

                //! Adds a new run attribute. Can only be called before track_problem is called for the first time.
                void add_run_attribute(const std::string &name, double *value = nullptr)
                {
                    guard_attributes([&]() { attributes_.run.insert_or_assign(name, value); });
                }

                //! Sets run attributes. Can only be called before track_problem is called for the first time.
                void set_run_attributes(const structures::dPtrMap &map)
                {
                    guard_attributes([&]() { attributes_.run = map; });
                }

                //! Sets the value for a single run attribute.
                void set_run_attribute(const std::string &name, double *value)
                {
                    assert(attributes_.run.find(name) != attributes_.run.end());
                    attributes_.run.at(name) = value;
                }

                //! Accessor for output directory
                virtual fs::path output_directory() const { return path_.path(); }

            private:
                static inline watch::Evaluations evaluations_{R"#("function evaluation")#"};
                static inline watch::CurrentY current_y_{R"#("current f(x)")#"};
                static inline watch::RawYBest y_best_{R"#("best-so-far f(x)")#"};
                static inline watch::TransformedY transformed_y_{R"#("current af(x)+b")#"};
                static inline watch::TransformedYBest transformed_y_best_{R"#("best af(x)+b")#"};
                static inline Properties default_properties_{evaluations_, current_y_, y_best_, transformed_y_,
                                                             transformed_y_best_};
            };
        } // namespace v1

        namespace v2
        {
            /**
             * @brief New logger method: using csv and json files.
             * json file structure:
             *  ~ 1 file per problem:
             *  ~ every dimension is a struct with fields:
             *      {suite, fid, fname, dim, maximization, algid, alginfo, csv file name, *experiment attr., list of run
             * attributes names, list of runs} ~ every run is struct with: {instance, neval, best point {e, y, x}, *run
             * attributes} csv file: {runid, "function evaluation", "current f(x)" "best-so-far f(x)" "current af(x)+b"
             * "best af(x)+b"}
             */
            class Analyzer : public analyzer::v1::Analyzer
            {
                std::map<std::string, structures::ExperimentInfo> experiments_;
                std::string current_filename_;

                std::vector<std::string> extra_attribute_names() const
                {
                    std::vector<std::string> names;
                    for (size_t i = 5; i < properties_vector_.size(); i++)
                        names.push_back(properties_vector_.at(i).get().name());
                    return names;
                }

                //! Gets called when a new problem is attached
                virtual void handle_new_problem(const problem::MetaData &problem)
                {
                    using str = std::string;
                    using sAttr = structures::Attribute<str>;
                    current_filename_ = fmt::format("IOHprofiler_f{:d}_{}.json", problem.problem_id, problem.name);

                    if (experiments_.find(current_filename_) == experiments_.end())
                        experiments_.insert({current_filename_,
                                             {current_suite_, problem, algorithm_,
                                              common::as_vector<str, str, sAttr>(attributes_.experiment),
                                              common::keys(attributes_.run), extra_attribute_names()}});
                }

                //! Gets called after the last evaluation of a run
                virtual void handle_new_dimension(const problem::MetaData &problem, const std::string &dat_path)
                {
                    experiments_.at(current_filename_)
                        .dims.emplace_back(static_cast<size_t>(problem.n_variables), dat_path);
                }

                //! Gets called when the current problem changes dimension
                virtual void handle_last_eval()
                {
                    experiments_.at(current_filename_)
                        .dims.back()
                        .runs.emplace_back(
                            static_cast<size_t>(problem_->instance), evals_, best_point_,
                            common::as_vector<std::string, double, structures::Attribute<double>>(attributes_.run));
                    if (log_info_.evaluations != 0)
                        FlatFile::call(log_info_);

                    fmt::print("{}", experiments_.at(current_filename_));
                }

                //! Writes all data to the info file
                void write()
                {
                    for (const auto &[filename, exp] : experiments_)
                    {
                        info_stream_ = std::ofstream(path_ / filename);
                        info_stream_ << fmt::format("{}", exp);
                        info_stream_.close();
                    }
                }

            public:
                using logger::analyzer::v1::Analyzer::Analyzer;

                virtual ~Analyzer() { write(); }
            };
        } // namespace v2
    } // namespace analyzer

    using Analyzer = analyzer::v1::Analyzer;
} // namespace ioh::logger
