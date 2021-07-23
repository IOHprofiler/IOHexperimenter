#pragma once

#include "ioh/logger/flatfile.hpp"


namespace ioh::logger
{
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
        /*   fs::path output_directory = fs::current_path(), const std::string &folder_name = "ioh_data",
             std::string algorithm_name = "algorithm_name", std::string algorithm_info = "algorithm_info",
             const common::OptimizationType optimization_type = common::OptimizationType::Minimization,             
             const bool store_positions = false, const bool t_always = false, const int t_on_interval = 0,
             const int t_per_time_range = 0, const bool t_on_improvement = true,
             const std::vector<int> &t_at_time_points = {0},
             const int trigger_at_time_points_exp_base = 10, const int trigger_at_range_exp_base = 10
         */

        static inline watch::Evaluations evaluations_{R"#("function evaluation")#"};
        static inline watch::CurrentY current_y_{R"#("current f(x)")#"};
        static inline watch::RawYBest y_best_{R"#("best-so-far f(x)")#"};
        static inline watch::TransformedY transformed_y_{R"#("current af(x)+b")#"};
        static inline watch::TransformedYBest transformed_y_best_{R"#("best af(x)+b")#"};
        static inline Properties default_properties_{evaluations_, current_y_, y_best_, transformed_y_,
                                                     transformed_y_best_};

        fs::path root_;
    public:
        Analyzer(const Triggers &triggers, const Properties &properties, const fs::path &root = fs::current_path()) :
            FlatFile(triggers, default_properties_, "IOH.dat", fs::current_path(), "\t", "", "None", "\n", true, false),
            root_(root)
        {
            for (const auto &p : properties)
                watch(p);
        }

        void attach_problem(const problem::MetaData &problem) override
        {
            // Determine file name
            const auto directory =
                root_ / common::string_format("data_f%d_%s", problem.problem_id, problem.name.c_str());
            const auto filename = common::string_format("IOHprofiler_f%d_DIM%d.dat", problem.problem_id,
                                                        problem.n_variables);

            open_stream(filename, directory);
            FlatFile::attach_problem(problem);

            std::cout << "attach an\n";
        }
    };
}
