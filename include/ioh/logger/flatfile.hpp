#pragma once

#include "ioh/common/format.hpp"
#include "ioh/problem/single.hpp"

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
    template <typename R>
    class FlatFile : public Watcher<R>
    {
    protected:
        //! Seperator
        const std::string sep_;
        
        //! Comment character
        const std::string com_;
        
        //! EOL character
        const std::string eol_;
        
        //! NAN string
        const std::string nan_;
        
        //! Common header
        const std::string common_header_;

        //! Repeat header for every run?
        const bool repeat_header_;

        //! Store x positions?
        const bool store_positions_;

        //! Requires header?
        bool requires_header_;

        //! Log meta data?
        const bool log_meta_data_;
        
        //! Output directory
        fs::path output_directory_;

        //! Filename
        std::string filename_;
    
        //! Output stream
        std::ofstream out_;

       
        
        //! Current run
        size_t current_run_;

        //! Current meta data
        std::string current_meta_data_;

        //! Open a file
        void open_stream(const std::string &filename, const fs::path &output_directory)
        {
            if (filename != filename_)
            {
                filename_ = filename;
                out_.close();
            }
            if (output_directory_ != output_directory)
            {
                output_directory_ = output_directory;
                if (not exists(output_directory_))
                {
                    IOH_DBG(debug, "some directories do not exist in " << output_directory_ << ", try to create them")
                    create_directories(output_directory_);
                }
                out_.close();
            }
            if (!out_.is_open())
            {
                IOH_DBG(debug, "will output data in " << output_directory_ / filename_)
                out_ = std::ofstream(output_directory_ / filename_, std::ofstream::out | std::ofstream::app);
                requires_header_ = true;
            }
        }

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
         * @param store_positions Whether to store x positions in the logged data
         * @param common_header_titles Seven strings to print in the header for the common problem meta data (property
         * names are automatically printed after).
         */
        FlatFile(std::vector<std::reference_wrapper<Trigger<R>>> triggers,
                 std::vector<std::reference_wrapper<Property<R>>> properties, const std::string &filename = "IOH.dat",
                 const fs::path &output_directory = fs::current_path(), const std::string &separator = "\t",
                 const std::string &comment = "# ", const std::string &no_value = "None",
                 const std::string &end_of_line = "\n", const bool repeat_header = false,
                 const bool store_positions = false,
                 const std::vector<std::string> &common_header_titles = {"suite_name", "problem_name", "problem_id",
                                                                         "problem_instance", "optimization_type",
                                                                         "dimension", "run"}) :
            Watcher<R>(triggers, properties),
            sep_(separator), com_(comment), eol_(end_of_line), nan_(no_value),
            common_header_(format("{}", fmt::join(common_header_titles.begin(), common_header_titles.end(), sep_)) +
                           (common_header_titles.empty() ? "" : sep_)),
            repeat_header_(repeat_header), store_positions_(store_positions), requires_header_(true),
            log_meta_data_(!common_header_titles.empty()), output_directory_(output_directory), filename_(filename),
            current_run_(0), current_meta_data_{}
        {
            assert(common_header_titles.empty() || common_header_titles.size() == 7);
        }

      

        void attach_problem(const problem::MetaData &problem) override
        {
            // If this is a new problem.
            if (!this->problem_.has_value() or this->problem_.value() != problem)
            {
                IOH_DBG(xdebug, "reset run counter")
                current_run_ = 0; // Then reset the run counter.
            }
            else
            {
                IOH_DBG(xdebug, "increment run counter")
                current_run_++; // Then it's a new run.
            }

            Logger<R>::attach_problem(problem);
            open_stream(filename_, output_directory_);

            requires_header_ = requires_header_ or repeat_header_;
            cache_meta_data();
        }

        void call(const Info<R> &log_info) override
        {
            IOH_DBG(debug, "FlatFile called");
            if (requires_header_)
            {
                IOH_DBG(xdebug, "print header")
                
                out_ << com_ + common_header_ + format("{}", fmt::join(this->properties_vector_, sep_));
                if constexpr (std::is_same_v<R, problem::MultiObjective>){
                    for (size_t i = 0; i < log_info.y.size(); i++)
                        out_ << sep_ << "y" << i;
                }
                if (store_positions_)
                    for (size_t i = 0; i < log_info.x.size(); i++)
                        out_ << sep_ << "x" << i;
                out_ << eol_;
                requires_header_ = false;
            }

            IOH_DBG(xdebug, "print problem meta data")
            out_ << current_meta_data_;
            
            IOH_DBG(xdebug, "print watched properties")
            
            for (auto p = this->properties_vector_.begin(); p != this->properties_vector_.end();){
                out_ << p->get().call_to_string(log_info, nan_) << (++p != this->properties_vector_.end() ? sep_ : "");
            }
            if constexpr (std::is_same_v<R, problem::MultiObjective>){
                out_ << sep_ << format("{:f}", fmt::join(log_info.y, this->sep_));
            }
            if (store_positions_)
                out_ << sep_ << format("{:f}", fmt::join(log_info.x, this->sep_));

            out_ << eol_;
            out_.flush();
        }

        //! Accessor for output directory
        virtual fs::path output_directory() const { return this->output_directory_; }

        //! Accessor for filename
        std::string filename() const { return this->filename_; }

        //! close data file
        virtual void close() override {
            if (out_.is_open()){
                IOH_DBG(debug, "close data file")
                out_.close();
            }
        }
        
        virtual ~FlatFile()
        {
            close();
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
                ss /* no sep */ << this->suite_;
                ss << sep_ << this->problem_.value().name;
                ss << sep_ << this->problem_.value().problem_id;
                ss << sep_ << this->problem_.value().instance;
                ss << sep_ << (this->problem_.value().optimization_type == common::OptimizationType::MIN ? "min" : "max");
                ss << sep_ << this->problem_.value().n_variables;
                ss << sep_ << current_run_;
                current_meta_data_ = ss.str();
            }
        }
    };
} // namespace ioh::logger
