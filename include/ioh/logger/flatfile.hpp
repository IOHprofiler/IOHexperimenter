#pragma once

#include <fmt/format.h>
#include <fmt/compile.h>

#include "ioh/problem/problem.hpp"

namespace ioh::logger
{
    /** A logger that stores some information in a single, tabular-like, file.
     * 
     * Each line displays the problem metadata and the watched properties.
     * This format displays a lot of redundant information, but is very easy to parse.
     * 
     * @code
        logger::FlatFile(
            {trigger::always},
            {watch::evaluations, watch::transformed_y},
            "my_experiment.dat",
            "./today/"
        );
     * @endcode
     *
     * @ingroup Loggers
     */
    class FlatFile : public Watcher
    {
    protected:
        const std::string sep_;
        const std::string com_;
        const std::string eol_;
        const std::string nan_;
        const std::array<const std::string, 7> heads_;
        const bool repeat_header_;
        bool has_header_;
        const bool log_meta_data_;

        fs::path exp_dir_;
        std::string filename_;

        std::ofstream out_;

        std::string current_suite_;
        size_t current_run_;
        std::string current_meta_data_;

    public:
        /** The logger should at least track one logger::Property, or else it makes no sense to use it.
         *
         * @param triggers When to fire a log event.
         * @param properties What to log.
         * @param filename File name in which to write the logged properties.
         * @param output_directory Directory in which to put the data file.
         * @param separator The string separating fields.
         * @param comment The string indicating a comment.
         * @param no_value The string indicating that a watched property does not exists in this context.
         * @param end_of_line The string to use when all fields have been written.
         * @param repeat_header If true, the commented header is printed for each new run.
         * @param log_meta_data If true, the values for common_header_titles are logged with every row.
         * @param common_header_titles Seven strings to print in the header for the common problem meta data (property names are automatically printed after).
         */
        FlatFile(std::vector<std::reference_wrapper<Trigger>> triggers,
                 std::vector<std::reference_wrapper<Property>> properties,
                 const std::string &filename = "IOH.dat", const fs::path &output_directory = fs::current_path(),
                 const std::string &separator = "\t", const std::string &comment = "# ",
                 const std::string &no_value = "None", const std::string &end_of_line = "\n", 
                 const bool repeat_header = false, const bool log_meta_data = true, 
                 const std::array<const std::string, 7> &common_header_titles = {"suite_name", "problem_name",
                     "problem_id", "problem_instance",
                     "optimization_type", "dimension",
                     "run"}                 
            ) :
            Watcher(triggers, properties)
            , sep_(separator)
            , com_(comment)
            , eol_(end_of_line)
            , nan_(no_value)
            , heads_(common_header_titles)
            , repeat_header_(repeat_header)
            , has_header_(false)
            , log_meta_data_(log_meta_data)
            , exp_dir_(output_directory)
            , filename_(filename)
            , current_suite_("unknown_suite")
            , current_run_(0)
            , current_meta_data_{}
        {
        }

        void open_stream(const std::string &filename, const fs::path &output_directory)
        {
            if (filename != filename_)
            {
                filename_ = filename;
                out_.close();
            }
            if (exp_dir_ != output_directory)
            {
                exp_dir_ = output_directory;
                if (not exists(exp_dir_))
                {
                    IOH_DBG(debug, "some directories do not exist in " << exp_dir_ << ", try to create them")
                    create_directories(exp_dir_);
                }
                out_.close();
            }
            if (!out_.is_open())
            {
                IOH_DBG(debug, "will output data in " << exp_dir_ / filename_)
                out_ = std::ofstream(exp_dir_ / filename_);    
            }              
        }

        void attach_suite(const std::string &suite_name) override
        {
            current_suite_ = suite_name;
        }

        void attach_problem(const problem::MetaData &problem) override
        {
            // If this is a new problem.
            if (problem_ == nullptr or *problem_ != problem)
            {
                IOH_DBG(xdebug, "reset run counter")
                current_run_ = 0; // Then reset the run counter.
            }
            else
            {
                IOH_DBG(xdebug, "increment run counter")
                current_run_++; // Then it's a new run.
            }

            Logger::attach_problem(problem); // update _problem
            open_stream(filename_, exp_dir_);

            if (repeat_header_ or not has_header_)
            {
                IOH_DBG(xdebug, "print header")
                out_ << com_;
                if (log_meta_data_)
                    for (const auto& head : heads_)
                       out_ << sep_ << head;
                for (const auto &p_ref : properties_vector_)
                    out_ << sep_ << p_ref.get().name();
                out_ << eol_;
                out_.flush();
                has_header_ = true;
            }
            cache_meta_data();
        }

        void call(const Info &log_info) override
        {
            // Common static values
            IOH_DBG(xdebug, "print problem meta data")
            out_ << current_meta_data_;

            IOH_DBG(xdebug, "print watched properties")
            for (const auto &p_ref : properties_vector_)
            {
                auto property = p_ref.get()(log_info);
                if (property)
                    out_ << sep_ << fmt::format(FMT_COMPILE("{:f}"), property.value());
                else
                    out_ << sep_ << nan_;
            }
            out_ << eol_;
            out_.flush();
        }

        virtual ~FlatFile()
        {
            IOH_DBG(debug, "close data file")
            out_.close();
        }

    private:
        /**
         * \brief Caches the meta data logged at every row. If log_meta_data is false,
         * it will store an empty string.
         */
        void cache_meta_data()
        {
            current_meta_data_.clear();
            if (log_meta_data_)
            {
                std::stringstream ss;
                ss /* no sep */ << current_suite_;
                ss << sep_ << problem_->name;
                ss << sep_ << problem_->problem_id;
                ss << sep_ << problem_->instance;
                ss << sep_ << (problem_->optimization_type == common::OptimizationType::Minimization ? "min" : "max");
                ss << sep_ << problem_->n_variables;
                ss << sep_ << current_run_;
                current_meta_data_ = ss.str();
            }
        }
    };
}
