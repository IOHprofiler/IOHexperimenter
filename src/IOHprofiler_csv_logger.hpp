/// \file IOHprofiler_csv_logger.hpp
/// \brief Head file for class IOHprofiler_csv_logger.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _IOHPROFILER_CSV_LOGGER_H
#define _IOHPROFILER_CSV_LOGGER_H

#include "IOHprofiler_observer.hpp"
#include "common.h"

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

namespace fs = boost::filesystem;

/// \brief A class of logging csv files.
///
/// To activate logger functions as evaluating problems,  a 'logger' must be added to
/// IOHprofiler_problem by the statement 'problem.add_logger(logger)'.
class IOHprofiler_csv_logger : public IOHprofiler_observer {
public:
  IOHprofiler_csv_logger() {
    this->output_directory = "./";
    this->folder_name = "IOHprofiler_test";
    this->algorithm_name =  "algorithm";
    this->algorithm_info = "algorithm_info";
  };

  IOHprofiler_csv_logger(std::string directory, std::string folder_name,
                         std::string alg_name, std::string alg_info) {
    this->output_directory = directory;
    this->folder_name = folder_name;
    this->algorithm_name =  alg_name;
    this->algorithm_info = alg_info;
  }
  ~IOHprofiler_csv_logger() {
    this->clear_logger();
  };

  void activate_logger();
  void clear_logger();

  void target_problem(int problem_id, int dimension, int instance);
  void target_suite(std::string suite_name);

  void openInfo(int problem_id, int dimension);
  void write_info(int instance, double optimal, int evaluations);

  void write_line(size_t evaluations, double y, double best_so_far_y,
                  double transformed_y, double best_so_far_transformed_y,
                  std::vector<double> parameters);
  void write_line(size_t evaluations, double y, double best_so_far_y,
                  double transformed_y, double best_so_far_transformed_y);
  void update_logger_info(size_t optimal_evaluations, std::vector<double> found_optimal);

  

private:
  std::string folder_name;
  std::string output_directory;
  std::string algorithm_name;
  std::string algorithm_info;

  std::string suite_name = "No suite";

  // The information of logged problems.
  int dimension;
  int problem_id;
  int instance;
  std::vector<std::string> parameter_name;

  std::fstream cdat;
  std::fstream idat;
  std::fstream dat;
  std::fstream tdat;
  std::fstream infoFile;

  std::vector<double> found_optimal;
  size_t optimal_evaluations;

  int last_dimension = 0;
  int last_problem_id = -1;
  
  /// \fn std::string IOHprofiler_experiment_folder_name()
  /// \brief return an available name of folder to be created.
  std::string IOHprofiler_experiment_folder_name();
  int IOHprofiler_create_folder(const std::string path);

  /// \fn openIndex()
  /// \brief to create the folder of logging files.
  int openIndex();
};

#endif //_IOHPROFILER_CSV_LOGGER_H