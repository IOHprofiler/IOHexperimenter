#pragma once

#include "ioh/logger/flatfile.hpp"


namespace ioh::logger
{

    namespace analyzer
    {
        using StringMap = std::map<std::string, std::string>;
        using dPtrMap = std::map<std::string, double*>;

        struct Attributes
        {
            /**
             * \brief List of attributes constant per experiment
             */
            StringMap experiment;

            /**
             * \brief List of parameters constant per run
             */
            dPtrMap run;
        };
    } // namespace analyzer


    /** A logger that stores information in a format supported by the IOHAnalyzer platform.
     *
     * @code
        logger::Analyzer(
            {trigger::always},
            {watch::evaluations, watch::transformed_y},
            "my_experiment.dat",
            "./today/"
        );
     * @endcode
     *
     * @ingroup Loggers
     */
    class Analyzer : public FlatFile
    {
        static inline watch::Evaluations evaluations_{R"#("function evaluation")#"};
        static inline watch::CurrentY current_y_{R"#("current f(x)")#"};
        static inline watch::RawYBest y_best_{R"#("best-so-far f(x)")#"};
        static inline watch::TransformedY transformed_y_{R"#("current af(x)+b")#"};
        static inline watch::TransformedYBest transformed_y_best_{R"#("best af(x)+b")#"};
        static inline Properties default_properties_{evaluations_, current_y_, y_best_, transformed_y_,
                                                     transformed_y_best_};

        const common::file::UniqueFolder path_;
        const std::string algorithm_name_;
        const std::string algorithm_info_;
        std::ofstream info_stream_;
        struct
        {
            double y;
            size_t evaluations;
        } best_point_{};

        analyzer::Attributes attributes_;
        bool has_started_;

    protected:
        void update_info_file(const problem::MetaData &problem, const std::string &dat_path)
        {
            const auto info_filename = fmt::format("IOHprofiler_f{:d}_{}.info", problem.problem_id, problem.name);

            if (problem_ != nullptr && best_point_.evaluations != 0)
            {
                info_stream_ << fmt::format(", {:d}:{:d}|{:g}", problem_->instance, best_point_.evaluations,
                                                      best_point_.y);
                for (auto &[key, value] : attributes_.run)
                    info_stream_ << fmt::format(";{:g}", *value);
            }

            if (problem_ == nullptr || problem_->problem_id != problem.problem_id)
            {
                if (info_stream_.is_open())
                    info_stream_.close();
                info_stream_ = std::ofstream(path_ / info_filename);
            }

            if (problem_ == nullptr || problem.n_variables != problem_->n_variables ||
                problem.problem_id != problem_->problem_id)
            {
                if (info_stream_.tellp() != 0)
                    info_stream_ << "\n";

                info_stream_ << "suite = \"" << current_suite_ << "\", funcId = " << problem.problem_id
                             << ", funcName = \"" << problem.name << "\", DIM = " << problem.n_variables
                             << ", maximization = "
                             << (problem.optimization_type == common::OptimizationType::Maximization ? 'T' : 'F')
                             << ", algId = \"" << algorithm_name_ << "\", algInfo = \"" << algorithm_info_ << '"';

                // extra attrs const per experiment
                for (const auto &[fst, snd] : attributes_.experiment)
                    info_stream_ << ", " << fst << " = \"" << snd << '"';

                // extra attrs const per run
                if (!attributes_.run.empty())
                {
                    info_stream_ << ", dynamicAttribute = \"";
                    for (auto i = attributes_.run.begin(); i != attributes_.run.end();)
                        info_stream_ << i->first << (++i != attributes_.run.end() ? "|" : "");
                    info_stream_ << "\"";
                }
                info_stream_ << "\n%\n" << dat_path;
            }
            info_stream_.flush();
        }

        void guard_attributes(const std::function<void()> &f)
        {
            if (has_started_)
            {
                IOH_DBG(warning, "cannot change attributes after experiment has started")
                return;
            }
            f();
        }

    public:
        Analyzer(const Triggers &triggers, const Properties &properties, const fs::path &root = fs::current_path(),
                 const std::string &folder_name = "ioh_data", const std::string &algorithm_name = "algorithm_name",
                 const std::string &algorithm_info = "algorithm_info", const bool store_positions = false,
                 const analyzer::Attributes &attributes = {}) :
            FlatFile(triggers, common::concatenate(default_properties_, properties), "", {}, "\t", "", "None", "\n",
                     true, store_positions, {}),
            path_(root, folder_name), algorithm_name_(algorithm_name), algorithm_info_(algorithm_info),
            attributes_(attributes), has_started_(false)
        {
        }

        virtual ~Analyzer()
        {
            IOH_DBG(debug, "close info file")
            info_stream_.close();
        }

        void attach_problem(const problem::MetaData &problem) override
        {
            const auto dat_directory = fmt::format("data_f{:d}_{}", problem.problem_id, problem.name);
            const auto dat_filename =
                fmt::format("IOHprofiler_f{:d}_DIM{:d}.dat", problem.problem_id, problem.n_variables);

            update_info_file(problem, fmt::format("{}/{}", dat_directory, dat_filename));
            open_stream(dat_filename, path_ / dat_directory);
            FlatFile::attach_problem(problem);
            best_point_ = {problem.initial_objective_value, 0};
            has_started_ = true;
        }

        void call(const Info &log_info) override
        {
            FlatFile::call(log_info);
            if (compare_objectives(log_info.current.y, best_point_.y, problem_->optimization_type))
                best_point_ = {log_info.current.y, log_info.evaluations};
        }

        void watch(logger::Property &property) override
        {
            guard_attributes([&]() { FlatFile::watch(property); });
        }

        void add_experiment_attribute(const std::string &name, const std::string &value)
        {
            guard_attributes([&]() { attributes_.experiment.insert_or_assign(name, value); });
        }

        void set_experiment_attributes(const analyzer::StringMap &map)
        {
            guard_attributes([&]() { attributes_.experiment = map; });
        }

        void add_run_attribute(const std::string &name, double* value = nullptr)
        {
            guard_attributes([&]() { attributes_.run.insert_or_assign(name, value); });
        }

        void set_run_attributes(const analyzer::dPtrMap &map)
        {
            guard_attributes([&]() { attributes_.run = map; });
        }

        void set_run_attribute(const std::string &name, double* value)
        {
            assert(attributes_.run.find(name) != attributes_.run.end());
            attributes_.run.at(name) = value;
        }
    };
} // namespace ioh::logger
