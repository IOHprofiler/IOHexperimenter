/// \file IOHprofiler_csv_logger.hpp
/// \brief Hpp file for class IOHprofiler_csv_logger.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _IOHPROFILER_CSV_LOGGER_HPP
#define _IOHPROFILER_CSV_LOGGER_HPP

#include "IOHprofiler_observer.hpp"
#include "IOHprofiler_problem.hpp"

/// \brief A class of logging csv files.
///
/// To activate logger functions as evaluating problems,  a 'logger' must be added to
/// IOHprofiler_problem by the statement 'problem.add_logger(logger)'.
class IOHprofiler_csv_logger : public IOHprofiler_observer {
public:

  IOHprofiler_csv_logger();

  IOHprofiler_csv_logger(std::string directory, std::string folder_name,
                         std::string alg_name, std::string alg_info);
  
  ~IOHprofiler_csv_logger();

  bool folder_exist(std::string folder_name);

  void activate_logger();
  
  void clear_logger();
  
  void set_attribute(const std::vector<std::shared_ptr<double> > &attributes);
  
  void set_attribute(const std::vector<std::shared_ptr<double> > &attributes, const std::vector<std::string> &attributes_name);

  void add_attribute(std::string, int);

  void add_attribute(std::string, double);

  void add_attribute(std::string, float);

  void add_attribute(std::string, std::string);

  void delete_attribute(std::string);

  void track_problem(const int problem_id, const int dimension, const int instance, const std::string problem_name, const IOH_optimization_type maximization_minimization_flag);
  
  void track_problem(const IOHprofiler_problem<int> & problem);
  
  void track_problem(const IOHprofiler_problem<double> & problem);

  void track_suite(std::string suite_name);

  void openInfo(int problem_id, int dimension, std::string problem_name);
  
  void write_info(int instance, double best_y, double best_transformed_y, int evaluations, 
                  double last_y, double last_transformed_y, int last_evaluations);

  void write_line(const size_t evaluations, const double y, const double best_so_far_y,
                 const double transformed_y, const double best_so_far_transformed_y);
  void do_log(const std::vector<double> &log_info);
  void write_line(const std::vector<double> &log_info);
  
  void update_logger_info(size_t optimal_evaluations, double y, double transformed_y);
  
  void set_parameters(const std::vector<std::shared_ptr<double> > &parameters);
  
  void set_parameters(const std::vector<std::shared_ptr<double> > &parameters, const std::vector<std::string> &parameters_name);

private:

  // The information for directory.
  std::string folder_name;
  std::string output_directory;

  // The information for logging.
  std::string algorithm_name;
  std::string algorithm_info;
  IOH_optimization_type maximization_minimization_flag = IOH_optimization_type::Maximization;
  std::map<std::string, std::string> attr_per_exp_name_value;
  std::map<std::string, std::shared_ptr<double> >  attr_per_run_name_value;

  std::string suite_name = "No suite";

  int dimension;
  int problem_id;
  int instance;
  std::string problem_name;
  //std::string problem_type;

  std::vector<double> best_y;
  std::vector<double> best_transformed_y;
  size_t optimal_evaluations;
  std::vector<double> last_y;
  std::vector<double> last_transformed_y;
  size_t last_evaluations;

  //std::vector<std::shared_ptr<double> > logging_parameters; /// < parameters to be logged as logging evaluation information.
  //std::vector<std::string> logging_parameters_name; /// < name of parameters to be logged as logging evaluation information.
  std::map<std::string, std::shared_ptr<double> > logging_parameters;

  // fstream
  std::fstream cdat;
  std::fstream idat;
  std::fstream dat;
  std::fstream tdat;
  std::fstream infoFile;

  int last_dimension = 0;
  int last_problem_id = -1;
  
  /// \fn std::string IOHprofiler_experiment_folder_name()
  /// \brief return an available name of folder to be created.
  std::string IOHprofiler_experiment_folder_name();
  
  int IOHprofiler_create_folder(const std::string path);

  bool header_flag; /// < parameters to track if the header line is logged.
  
  void write_header();

  /// \fn openIndex()
  /// \brief to create the folder of logging files.
  int openIndex();

  // std::shared_ptr<IOHprofiler_problem<int> >  tracked_problem_int;
  // std::shared_ptr<IOHprofiler_problem<double> > tracked_problem_double;
};

#endif //_IOHPROFILER_CSV_LOGGER_HPP
