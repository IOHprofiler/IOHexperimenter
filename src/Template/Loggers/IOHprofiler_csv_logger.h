/// \file IOHprofiler_csv_logger.h
/// \brief Header file for class IOHprofiler_csv_logger.
///
/// \author Furong Ye
#ifndef _IOHPROFILER_CSV_LOGGER_H
#define _IOHPROFILER_CSV_LOGGER_H

#include "IOHprofiler_observer.h"
#include "IOHprofiler_problem.h"

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
  
  void add_dynamic_attribute(const std::vector<std::shared_ptr<double> > &attributes);
  
  void add_dynamic_attribute(const std::vector<std::shared_ptr<double> > &attributes, const std::vector<std::string> &attributes_name);

  void add_attribute(std::string, int);

  void add_attribute(std::string, double);

  void add_attribute(std::string, float);

  void add_attribute(std::string, std::string);

  void delete_attribute(std::string);

  /// \fn track_problem(int problem_id, int dimension, int instance)
  ///
  /// This function is to be invoked by IOHprofiler_problem class.
  /// To update info of current working problem, and to write headline in corresponding files.

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

  // Only for python wrapper.
  void set_parameters_name(const std::vector<std::string > &parameters_name);
  void set_parameters_name(const std::vector<std::string > &parameters_name, const std::vector<double> & initial_parameters);
  void set_parameters(const std::vector<std::string> &parameters_name, const std::vector<double> &parameters);

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

  // Only for python wrapper.
  std::map<std::string, double > wrapper_logging_parameters;

  // fstream
  std::fstream cdat;
  std::fstream idat;
  std::fstream dat;
  std::fstream tdat;
  std::fstream infoFile;

  std::string cdat_buffer;
  std::string idat_buffer;
  std::string dat_buffer;
  std::string tdat_buffer;
  std::string info_buffer;

  int last_dimension = 0;
  int last_problem_id = -1;
  
  /// \fn std::string IOHprofiler_experiment_folder_name()
  /// \brief return an available name of folder to be created.
  ///
  /// To create a name of the folder logging files.
  /// If there exists a files or a folder with the same name, the expected 
  /// directory will be renamed by adding a suffix.
  /// For example,
  ///     If a folder or file 'test' has already been in currect path, the 
  ///     expected directory will be renamed as 'test-1', 'test-2', ... 
  ///     until there is no such a folder or file. 
  std::string IOHprofiler_experiment_folder_name();
  
  int IOHprofiler_create_folder(const std::string path);

  bool header_flag; /// < parameters to track if the header line is logged.
  
  /// \fn write_header()
  ///
  /// This function is to be invoked before recoring evaluations (if evaluations == 0).
  void write_header();

  void write_stream(const std::string buffer_string, std::fstream & dat_stream);

  void write_in_buffer(const std::string add_string, std::string & buffer_string, std::fstream & dat_stream);

  /// \fn openIndex()
  /// \brief to create the folder of logging files.
  int openIndex();

  // std::shared_ptr<IOHprofiler_problem<int> >  tracked_problem_int;
  // std::shared_ptr<IOHprofiler_problem<double> > tracked_problem_double;
};

#endif //_IOHPROFILER_CSV_LOGGER_H
