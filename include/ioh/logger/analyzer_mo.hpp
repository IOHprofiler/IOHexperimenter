#pragma once

#include "ioh/logger/analyzer.hpp"
#include "ioh/logger/properties_mo.hpp"

namespace ioh::logger
{
    /* Anything related to Analyzer logger */
    namespace analyzer
    {
        /* Structures related to Analyzer logger */
        namespace structures
        {
            //! Current pareto front points conatiner
            struct ParetoFrontPoint : common::HasRepr
            {
                //! At what eval was the pareto front is valid.
                size_t evals;
                //! Value of the point
                std::vector<problem::MultiObjectiveSolution<double>> points;

                /**
                 * @brief Construct a new Best Point object
                 *
                 * @param evals At what eval was the point recorded
                 * @param point Value of the point
                 */
                ParetoFrontPoint(const size_t evals = 0,
                                 const std::vector<problem::MultiObjectiveSolution<double>> &points = {}) :
                    evals(evals),
                    points(points)
                {
                }

                std::string repr() const override
                {
                    std::vector<std::string> x_string(points.size());
                    std::vector<std::string> y_string(points.size());
                    for (auto i = 0; i != points.size(); ++i)
                    {
                        x_string[i] = fmt::format("[{}]", fmt::join(points[i].x, ","));
                        y_string[i] = fmt::format("[{}]", fmt::join(points[i].y, ","));
                    }
                    return fmt::format("\"evals\": {}, \"y\": [{}], \"x\": [{}]", evals, fmt::join(y_string, ","),
                                       fmt::join(x_string, ","));
                }
            };

            //! Run information data
            //  A version for the multiobjective optimization.
            struct RunInfo_MO : common::HasRepr
            {
                //! Instance id
                const size_t instance;
                //! N evals
                const size_t evals;
                //! best point
                const ParetoFrontPoint best_points;
                //! Attributes
                const std::vector<Attribute<double> > attributes;

                /**
                 * @brief Construct a new Run Info object
                 *
                 * @param instance  Instance id
                 * @param evals N evals
                 * @param bp best point
                 * @param ra Attributes
                 */
                RunInfo_MO(const size_t instance, const size_t evals, const ParetoFrontPoint &bp,
                           const std::vector<Attribute<double>> &ra = {}) :
                    instance(instance),
                    evals(evals), best_points(bp), attributes(ra)
                {
                }

                std::string repr() const override
                {
                    return fmt::format("\"instance\": {}, \"evals\": {}, \"best\": {{{}}}{}", instance, evals,
                                       best_points,
                                       (attributes.empty() ? "" : fmt::format(", {}", fmt::join(attributes, ", "))));
                }
            };

            //! Scenario meta data
            struct ScenarioInfo_MO : common::HasRepr
            {
                //! Dimension
                const size_t dimension;
                //! Data file
                const std::string data_file;
                //! Runs
                std::vector<RunInfo_MO> runs;

                /**
                 * @brief Construct a new Scenario Info object
                 * 
                 * @param dimension Dimension
                 * @param data_file Data file
                 * @param runs Runs
                 */
                ScenarioInfo_MO(const size_t dimension, const std::string &data_file,
                             const std::vector<RunInfo_MO> runs = {}) :
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
            struct ExperimentInfo_MO : common::HasRepr
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
                std::vector<ScenarioInfo_MO> dims;

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
                ExperimentInfo_MO(const std::string &suite, const problem::MetaData &problem,
                               const AlgorithmInfo &algorithm,
                               const std::vector<Attribute<std::string>> &attributes = {},
                               const std::vector<std::string> &run_attribute_names = {},
                               const std::vector<std::string> &attribute_names = {},
                               const std::vector<ScenarioInfo_MO> &dims = {}) :
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


        namespace mo
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
                // structures::BestPoint best_point_;

                //! Run/Experiment attribues
                structures::Attributes attributes_;

                //! Has started logging?
                bool has_started_;

                //! Current log info
                logger::Info log_info_{};

                //! Evals
                size_t evals_ = 0;

                structures::ParetoFrontPoint best_points_;
                std::map<std::string, structures::ExperimentInfo_MO> experiments_;
                std::string current_filename_;

                std::vector<std::string> attribute_names() const
                {
                    std::vector<std::string> names;
                    for (size_t i = 0; i < properties_vector_.size(); i++)
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
                                              common::keys(attributes_.run), attribute_names()}});

                }

                //! Gets called after the last evaluation of a run
                virtual void handle_last_eval()
                {
                    if (best_points_.evals != 0)
                    {
                        experiments_.at(current_filename_)
                            .dims.back()
                            .runs.emplace_back(
                                static_cast<size_t>(problem_.value().instance), evals_, best_points_,
                                common::as_vector<std::string, double, structures::Attribute<double>>(attributes_.run));
                        if (log_info_.evaluations != 0)
                            FlatFile::call(log_info_);
                    }
                }

                //! Gets called when the current problem changes dimension
                virtual void handle_new_dimension(const problem::MetaData &problem, const std::string &dat_path)
                {
                    experiments_.at(current_filename_)
                        .dims.emplace_back(static_cast<size_t>(problem.n_variables), dat_path);
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
                             common::concatenate(default_properties_,
                                                 additional_properties),
                                                 default_properties_mo_,
                             "", {},
                             " ", "",
                             "None", "\n", true, store_positions, {}),
                    path_(root, folder_name), algorithm_(algorithm_name, algorithm_info), best_points_{},
                    attributes_(attributes), has_started_(false)
                {
                }

                //! close data file
                virtual void close() override {
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
                    best_points_ = {0, std::vector<problem::MultiObjectiveSolution<double> >(1,
                                                                                            ioh::problem::MultiObjectiveSolution<double>(std::vector<double>(problem.n_variables), 
                                                                                                                                        std::vector<double>(problem.n_objectives,problem.optimization_type.initial_value())))};
                    has_started_ = true;
                }

                //! Set log_info_ in order to check if the last line has been logged
                virtual void log(const logger::MultiObjectiveInfo &log_info) override
                {
                    IOH_DBG(debug, "Analyzer log");
                    log_info_ = log_info;
                    evals_ = log_info.evaluations;
                    FlatFile::log(log_info);
                }

                //! See: logger::FlatFile::call. Updates `best_point_`
                virtual void call(const logger::MultiObjectiveInfo &log_info) override
                {
                    IOH_DBG(debug, "Analyzer called");

                    FlatFile::call(log_info);
                    if (log_info.has_improved)
                        best_points_ = {log_info.evaluations, log_info.pareto_front};
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
                // static inline watch::Evaluations evaluations_{R"#("function evaluation")#"};
                // static inline watch::RawY current_y_{R"#("current f(x)")#"};
                // static inline watch::RawYBest y_best_{R"#("best-so-far f(x)")#"};
                // static inline watch::TransformedY transformed_y_{R"#("current af(x)+b")#"};
                // static inline watch::TransformedYBest transformed_y_best_{R"#("best af(x)+b")#"};
                // static inline Properties default_properties_old_{evaluations_, current_y_, y_best_, transformed_y_,
                //                                              transformed_y_best_};


                // //! The only properties used by the analyzer (actually only raw_y is used)
                // static inline Properties default_properties_{watch::evaluations, watch::raw_y};
                static inline logger::Properties default_properties_{ioh::watch::evaluations};
                static inline ioh::logger::mo::Properties default_properties_mo_{ioh::logger::mo::watch::current_y_mo};
            };

            /**
            //  * @brief Multiobjective logger method: using csv and json files.
            //  * json file structure:
            //  *  ~ 1 file per problem:
            //  *  ~ every dimension is a struct with fields:
            //  *      {suite, fid, fname, dim, maximization, algid, alginfo, csv file name, *experiment attr., list of run
            //  * attributes names, list of runs} ~ every run is struct with: {instance, neval, best point {e, y, x}, *run
            //  * attributes} csv file: {runid, "function evaluation", "obj1" "obj2" "obj3" ....}
            //  */
            // class Analyzer : public v1::Analyzer
            // {


            //     virtual void call(const Info &log_info) override
            //     {
                    
            //     }

            //     //! Set log_info_ in order to check if the last line has been logged
            //     virtual void log(const logger::Info &log_info) override
            //     {
            
            //     }

            //     //! Gets called when a new problem is attached
            //     virtual void handle_new_problem(const problem::MetaData &problem) override
            //     {
                    
            //     }

            //     //! Gets called after the last evaluation of a run
            //     virtual void handle_new_dimension(const problem::MetaData &problem,
            //                                       const std::string &dat_path) override
            //     {
                    
            //     }

            //     //! Gets called when the current problem changes dimension
            //     virtual void handle_last_eval() override
            //     {
                    
            //     }

            // protected:
            //     //! Writes all data to the info file
            //     void close() override
            //     {
                    
            //     }

            // public:
            //     /** Logger formatting data in a format supported by iohprofiler.
            //      *
            //      * @param triggers When to fire a log event.
            //      * @param additional_properties What to log.
            //      * @param root Path in which to store the data.
            //      * @param folder_name Name of folder in which to store data. Will be created as a subdirectory of
            //      * `root`.
            //      * @param algorithm_name The string separating fields.
            //      * @param algorithm_info The string indicating a comment.
            //      * @param store_positions Whether to store x positions in the logged data
            //      * @param use_old_data_format Wheter to use the old data format
            //      * @param attributes See: analyzer::Attributes.
            //      */
            //     Analyzer(const Triggers &triggers = {trigger::on_improvement},
            //              const logger::Properties &additional_properties = {},
            //              const fs::path &root = fs::current_path(), const std::string &folder_name = "ioh_data",
            //              const std::string &algorithm_name = "algorithm_name",
            //              const std::string &algorithm_info = "algorithm_info", const bool store_positions = false,
            //              const structures::Attributes &attributes = {}) :
            //         v1::Analyzer(triggers, additional_properties, root, folder_name, algorithm_name, algorithm_info,
            //                      store_positions, false, attributes)
            //     {
            //     }

            //     virtual ~Analyzer() { close(); }

            // private:
                
            // };
        } // namespace mo

    } // namespace analyzer
    using Analyzer_mo = analyzer::mo::Analyzer;
} // namespace ioh::logger