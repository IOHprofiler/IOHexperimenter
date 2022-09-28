#pragma once

#include "ioh/common/file.hpp"
#include "ioh/logger/flatfile.hpp"

namespace ioh::logger
{
    /* Anything related to Analyzer logger */
    namespace analyzer
    {
        /* Structures related to Analyzer logger */
        namespace structures
        {
            //! Attribute
            template <typename T>
            struct Attribute : common::HasRepr
            {
                
                //! Name of the attribute
                std::string name;
                //! Value of the attribue
                T value;

                /**
                 * @brief Construct a new Attribute object
                 * 
                 * @param name name of the attribute
                 * @param value Value of the attribue
                 */
                Attribute(const std::string &name, const T &value) : name(name), value(value) {}

                std::string repr() const override { return fmt::format("\"{}\": {}", name, value); }
            };

            template <>
            inline std::string Attribute<std::string>::repr() const
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

            //! Current best point conatiner
            struct BestPoint : common::HasRepr
            {
                //! At what eval was the point recorded
                size_t evals;
                //! Value of the point
                problem::Solution<double> point;

                /**
                 * @brief Construct a new Best Point object
                 * 
                 * @param evals At what eval was the point recorded
                 * @param point Value of the point
                 */
                BestPoint(const size_t evals = 0, const problem::Solution<double> &point = {}) :
                    evals(evals), point(point)
                {
                }

                std::string repr() const override
                {
                    return fmt::format("\"evals\": {}, \"y\": {}, \"x\": [{}]", evals, point.y,
                                       fmt::join(point.x, ", "));
                }
            };

            //! Algorithm meta data
            struct AlgorithmInfo : common::HasRepr
            {
                
                //! Name of the algorithm
                const std::string name;

                //! Extra string of algoritm information
                const std::string info;

                /**
                 * @brief Construct a new Algorithm Info object
                 * 
                 * @param name Name of the algorithm
                 * @param info Extra string of algoritm information
                 */
                AlgorithmInfo(const std::string &name, const std::string &info) : name(name), info(info) {}

                std::string repr() const override
                {
                    return fmt::format("\"name\": \"{}\", \"info\": \"{}\"", name, info);
                }
            };

            //! Run information data
            struct RunInfo : common::HasRepr
            {
                //! Instance id
                const size_t instance;
                //! N evals
                const size_t evals;
                //! best point
                const BestPoint best_point;
                //! Attributes
                const std::vector<Attribute<double>> attributes;

                /**
                 * @brief Construct a new Run Info object
                 * 
                 * @param instance  Instance id
                 * @param evals N evals
                 * @param bp best point
                 * @param ra Attributes
                 */
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
            
            //! Scenario meta data
            struct ScenarioInfo : common::HasRepr
            {
                //! Dimension
                const size_t dimension;
                //! Data file
                const std::string data_file;
                //! Runs
                std::vector<RunInfo> runs;

                /**
                 * @brief Construct a new Scenario Info object
                 * 
                 * @param dimension Dimension
                 * @param data_file Data file
                 * @param runs Runs
                 */
                ScenarioInfo(const size_t dimension, const std::string &data_file,
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

            //! Experiment information
            struct ExperimentInfo : common::HasRepr
            {
                //! Suite name
                const std::string suite;
                //! problem 
                const problem::MetaData problem;
                //! Algoritm meta data
                const AlgorithmInfo algorithm;
                //! Attributes
                const std::vector<Attribute<std::string>> attributes;
                //! Run attributes
                const std::vector<std::string> run_attribute_names;
                //! attributes names
                const std::vector<std::string> attribute_names;
                //! Scenarios
                std::vector<ScenarioInfo> dims;

                /**
                 * @brief Construct a new Experiment Info object
                 * 
                 * @param suite suite name
                 * @param problem problem 
                 * @param algorithm Algoritm meta data
                 * @param attributes Attributes
                 * @param run_attribute_names Run attributes
                 * @param attribute_names attributes names
                 * @param dims Scenarios
                 */
                ExperimentInfo(const std::string &suite, const problem::MetaData &problem,
                               const AlgorithmInfo &algorithm,
                               const std::vector<Attribute<std::string>> &attributes = {},
                               const std::vector<std::string> &run_attribute_names = {},
                               const std::vector<std::string> &attribute_names = {},
                               const std::vector<ScenarioInfo> &dims = {}) :
                    suite(suite),
                    problem(problem), algorithm(algorithm), attributes(attributes),
                    run_attribute_names(run_attribute_names), attribute_names(attribute_names), dims(dims)
                {
                }

                std::string repr() const override
                {
                    return fmt::format(
                        "{{\n\t\"version\": \"{}\", \n\t\"suite\": \"{}\", \n\t\"function_id\": {}, \n\t\"function_name\": \"{}\", "
                        "\n\t\"maximization\": {}, "
                        "\n\t\"algorithm\": {{{}}},{}{}{}\n\t\"scenarios\": [\n\t\t{{{}}}\n\t]\n}}\n",
                        PROJECT_VER, suite, problem.problem_id, problem.name,
                        (problem.optimization_type == common::OptimizationType::MAX), algorithm,
                        !attributes.empty()
                            ? fmt::format("\n\t\"experiment_attributes\": [{{{}}}],", fmt::join(attributes, "}, {"))
                            : "",
                        !run_attribute_names.empty()
                            ? fmt::format("\n\t\"run_attributes\": [\"{}\"],", fmt::join(run_attribute_names, "\", \""))
                            : "",
                        !attribute_names.empty() ? fmt::format("\n\t\"attributes\": [\"{}\"],",
                                                                     fmt::join(attribute_names, "\", \""))
                                                       : "",
                        fmt::join(dims, "},\n\t\t{"));
                }
            };

        } // namespace structures

        /* Version 1 */
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
                //! output path
                const common::file::UniqueFolder path_;
                
                //! Algorithm meta data
                const structures::AlgorithmInfo algorithm_;
                
                //! info file stream
                std::ofstream info_stream_;

                //! Best point
                structures::BestPoint best_point_;

                //! Run/Experiment attribues
                structures::Attributes attributes_;

                //! Has started logging?
                bool has_started_;

                //! Current log info
                logger::Info log_info_{};

                //! Evals
                size_t evals_ = 0;

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
                    if (best_point_.evals != 0)
                    {
                        info_stream_ << fmt::format(", {:d}:{:d}|{:g}", problem_.value().instance, best_point_.evals,
                                                    best_point_.point.y);
                        for (auto &p : attributes_.run)
                            info_stream_ << fmt::format(";{:g}", *p.second);

                        if (log_info_.evaluations != 0)
                            FlatFile::call(log_info_);
                    }
                }

                //! Gets called when the current problem changes dimension
                virtual void handle_new_dimension(const problem::MetaData &problem, const std::string &dat_path)
                {
                    if (info_stream_.tellp() != 0)
                        info_stream_ << "\n";

                    info_stream_ << "suite = \"" << current_suite_ << "\", funcId = " << problem.problem_id
                                 << ", funcName = \"" << problem.name << "\", DIM = " << problem.n_variables
                                 << ", maximization = \""
                                 << (problem.optimization_type == common::OptimizationType::MAX ? 'T' : 'F')
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
                    if (problem_.has_value())
                        handle_last_eval();

                    if (!problem_.has_value() || problem_.value().problem_id != problem.problem_id)
                        handle_new_problem(problem);

                    if (!problem_.has_value() || problem.n_variables != problem_.value().n_variables ||
                        problem.problem_id != problem_.value().problem_id)
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
                 * @param additional_properties What to log.
                 * @param root Path in which to store the data.
                 * @param folder_name Name of folder in which to store data. Will be created as a subdirectory of
                 * `root`.
                 * @param algorithm_name The string separating fields.
                 * @param algorithm_info The string indicating a comment.
                 * @param store_positions Whether to store x positions in the logged data
                 * @param use_old_data_format Wheter to use the old data format
                 * @param attributes See: analyzer::Attributes.
                 */
                Analyzer(const Triggers &triggers = {trigger::on_improvement},
                         const Properties &additional_properties = {},
                         const fs::path &root = fs::current_path(),
                         const std::string &folder_name = "ioh_data",
                         const std::string &algorithm_name = "algorithm_name",
                         const std::string &algorithm_info = "algorithm_info",
                         const bool store_positions = false,
                         const bool use_old_data_format = true,
                         const structures::Attributes &attributes = {}
                ) :
                    FlatFile(triggers,
                             common::concatenate(use_old_data_format ? default_properties_old_ : default_properties_,
                                                 additional_properties),
                             "", {},
                             " ", "",
                             "None", "\n", true, store_positions, {}),
                    path_(root, folder_name), algorithm_(algorithm_name, algorithm_info), best_point_{},
                    attributes_(attributes), has_started_(false)
                {
                }

                //! close data file
                virtual void close() override {
                    if (info_stream_.is_open()){
                        handle_last_eval();
                        IOH_DBG(debug, "close info file")
                        info_stream_.close();
                    }
                    if (!has_started_)
                        fs::remove(output_directory());
                    FlatFile::close();
                }

                virtual ~Analyzer()
                {
                    close();
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
                    best_point_ = {0, {std::vector<double>(problem.n_variables), problem.optimization_type.initial_value()}};
                    has_started_ = true;
                }

                //! Set log_info_ in order to check if the last line has been logged
                virtual void log(const logger::Info &log_info) override
                {
                    IOH_DBG(debug, "Analyzer log");
                    log_info_ = log_info;
                    evals_ = log_info.evaluations;
                    FlatFile::log(log_info);
                }

                //! See: logger::FlatFile::call. Updates `best_point_`
                virtual void call(const Info &log_info) override
                {
                    IOH_DBG(debug, "Analyzer called");
                    
                    FlatFile::call(log_info);
                    if (log_info.has_improved)
                        best_point_ = {log_info.evaluations, {log_info.x, log_info.raw_y}};
                    log_info_ = {};
                }

                #ifndef NDEBUG
                void reset() override
                {
                    IOH_DBG(debug, "Analyzer reset");
                    FlatFile::reset();
                }
                #endif

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
                virtual fs::path output_directory() const override { return path_.path(); }

            private:
                static inline watch::Evaluations evaluations_{R"#("function evaluation")#"};
                static inline watch::RawY current_y_{R"#("current f(x)")#"};
                static inline watch::RawYBest y_best_{R"#("best-so-far f(x)")#"};
                static inline watch::TransformedY transformed_y_{R"#("current af(x)+b")#"};
                static inline watch::TransformedYBest transformed_y_best_{R"#("best af(x)+b")#"};
                static inline Properties default_properties_old_{evaluations_, current_y_, y_best_, transformed_y_,
                                                             transformed_y_best_};


                //! The only properties used by the analyzer (actually only raw_y is used)
                static inline Properties default_properties_{watch::evaluations, watch::raw_y};
            };
        } // namespace v1
        
        /* Version 1 */
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
            class Analyzer : public v1::Analyzer
            {
                std::map<std::string, structures::ExperimentInfo> experiments_;
                std::string current_filename_;

                std::vector<std::string> attribute_names() const
                {
                    std::vector<std::string> names;
                    for (size_t i = 0; i < properties_vector_.size(); i++)
                        names.push_back(properties_vector_.at(i).get().name());
                    return names;
                }

                //! Gets called when a new problem is attached
                virtual void handle_new_problem(const problem::MetaData &problem) override
                {
                    using str = std::string;
                    using sAttr = structures::Attribute<str>;
                    current_filename_ = fmt::format("IOHprofiler_f{:d}_{}.json", problem.problem_id, problem.name);

                    if (experiments_.find(current_filename_) == experiments_.end())
                        experiments_.insert({current_filename_,
                                             {current_suite_, problem, algorithm_,
                                              common::as_vector<str, str, sAttr>(attributes_.experiment),
                                              common::keys(attributes_.run), attribute_names()}});
                }

                //! Gets called after the last evaluation of a run
                virtual void handle_new_dimension(const problem::MetaData &problem, const std::string &dat_path) override
                {
                    experiments_.at(current_filename_)
                        .dims.emplace_back(static_cast<size_t>(problem.n_variables), dat_path);
                }

                //! Gets called when the current problem changes dimension
                virtual void handle_last_eval() override
                {
                    if (best_point_.evals != 0)
                    {
                        experiments_.at(current_filename_)
                            .dims.back()
                            .runs.emplace_back(
                                static_cast<size_t>(problem_.value().instance), evals_, best_point_,
                                common::as_vector<std::string, double, structures::Attribute<double>>(attributes_.run));
                        if (log_info_.evaluations != 0)
                            FlatFile::call(log_info_);
                    }
                }

            protected:
                //! Writes all data to the info file
                void close() override
                {
                    handle_last_eval(); // TODO: check if this doesn't cause duplicate writes
                    for (const auto &[filename, exp] : experiments_)
                    {
                        info_stream_ = std::ofstream(path_ / filename);
                        info_stream_ << fmt::format("{}", exp);
                        info_stream_.close();
                    }
                    if (!has_started_)
                        fs::remove(output_directory());
                    FlatFile::close();                    
                }

            public:
                /** Logger formatting data in a format supported by iohprofiler.
                 *
                 * @param triggers When to fire a log event.
                 * @param additional_properties What to log.
                 * @param root Path in which to store the data.
                 * @param folder_name Name of folder in which to store data. Will be created as a subdirectory of
                 * `root`.
                 * @param algorithm_name The string separating fields.
                 * @param algorithm_info The string indicating a comment.
                 * @param store_positions Whether to store x positions in the logged data
                 * @param use_old_data_format Wheter to use the old data format
                 * @param attributes See: analyzer::Attributes.
                 */
                Analyzer(const Triggers &triggers = {trigger::on_improvement},
                         const Properties &additional_properties = {}, 
                         const fs::path &root = fs::current_path(),
                         const std::string &folder_name = "ioh_data",
                         const std::string &algorithm_name = "algorithm_name",
                         const std::string &algorithm_info = "algorithm_info", const bool store_positions = false,
                         const structures::Attributes &attributes = {}) :
                    v1::Analyzer(triggers, additional_properties, root, folder_name, algorithm_name,
                                           algorithm_info, 
                        store_positions, false, attributes)
                {
                }

                virtual ~Analyzer() { close(); }
            };
        } // namespace v2
    } // namespace analyzer


    //! Default version of the logger
    using Analyzer = analyzer::v2::Analyzer;
} // namespace ioh::logger
