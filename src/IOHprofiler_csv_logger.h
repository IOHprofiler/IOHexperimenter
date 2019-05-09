#ifndef _IOHPROFILER_CSV_LOGGER_H
#define _IOHPROFILER_CSV_LOGGER_H

#include "IOHprofiler_observer.hpp"
#include "common.h"

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

namespace fs = boost::filesystem;

// Implementation for logging process of problem evaluations.
// A folder for storing data with given name 'path' will be created
// when the class is generated. To activate logger functions as 
// evaluating problems, a 'logger' must be added to a
// IOHprofiler_problem with format 'problem.add_logger(logger)'.
//
// Hint: Only use an unique logger for each experiments for an algorithm.
// But a logger is avaiable for test of an algorithm on multiple prorblems.

class IOHprofiler_csv_logger : public IOHprofiler_observer {
public:

  // Once a logger is created, the working directory is established, and data files
  // to be created is fixed (by setting paramters of IOHprofiler_observer class).
  // Experiments on different problems can sure the same logger, but the problem info
  // needs to be set by function "IOHprofiler_logger_target_problem()" for experiment 
  // on each function. So does parameters infomation.
  IOHprofiler_csv_logger() {}
  IOHprofiler_csv_logger(std::string directory, std::string folder_name,
                         std::string alg_name, std::string alg_info, 
                         bool complete_triggers, bool update_triggers, int number_interval_triggers,
                         std::vector<int> time_points_trigger, int number_evaluation_triggers) {
    this->output_directory = directory;
    this->folder_name = folder_name;
    this->algorithm_name =  alg_name;
    this->algorithm_info = alg_info;

    set_complete_flag(complete_triggers);
    set_update_flag(update_triggers);
    set_interval(number_interval_triggers);
    set_time_points(time_points_trigger,number_evaluation_triggers);
    openIndex();
  }

  void init_logger(std::string directory, std::string folder_name,
                     std::string alg_name, std::string alg_info, 
                     bool complete_triggers, bool update_triggers, int number_interval_triggers,
                     std::vector<int> time_points_trigger, int number_evaluation_triggers
                    );
  //~IOHprofiler_logger();

  void write_line(size_t evaluations, double y, double best_so_far_y,
                           double transformed_y, double best_so_far_transformed_y,
                           std::vector<double> parameters);
  void write_line(size_t evaluations, double y, double best_so_far_y,
                           double transformed_y, double best_so_far_transformed_y);

  void target_problem(int problem_id, int dimension, int instance);
  void target_suite(std::string suite_name);

  size_t evaluations;
  std::string folder_name;
  std::string output_directory;
  std::string suite_name = "";

  // The information of logged problems.
  int dimension;
  int problem_id;
  int instance;
  std::string algorithm_name;
  std::string algorithm_info;

  std::vector<std::string> parameter_name;

  //Variables for logging files
  std::ofstream cdat;
  std::ofstream idat;
  std::ofstream dat;
  std::ofstream infoFile;

  // For openInfo function, to check if write headline.
  int last_dimension = 0;
  int last_problem_id;

  IOHprofiler_csv_logger& operator = (IOHprofiler_csv_logger& logger) {
    this->evaluations = logger.evaluations;
    this->folder_name = logger.folder_name;
    this->output_directory = logger.output_directory;
    this->suite_name = logger.suite_name;

    this->dimension = logger.dimension;
    this->problem_id = logger.problem_id;
    this->instance = logger.instance;
    this->algorithm_info = logger.algorithm_info;
    this->algorithm_name = logger.algorithm_name;
    copyVector(logger.parameter_name,this->parameter_name);

    this->observer_interval = logger.observer_interval;
    this->observer_complete_flag = logger.observer_complete_flag;
    this->observer_update_flag = logger.observer_update_flag;
    this->current_best_fitness = logger.current_best_fitness;
    copyVector(logger.observer_time_points,this->observer_time_points);
    this->observer_number_of_evaluations = logger.observer_number_of_evaluations;
    this->evaluations_value1 = logger.evaluations_value1;
    this->evaluations_value2 = logger.evaluations_value2;
    this->time_points_expi = logger.time_points_expi;
    this->time_points_index = logger.time_points_index;
    this->evaluations_expi = logger.evaluations_expi;


    this->last_dimension = logger.last_dimension;
    this->last_problem_id = logger.last_dimension;
  };
  void write_info(int instance, double optimal, int evaluations);
  void openInfo(int problem_id, int dimension);
  void clear_logger();

private:
  
  int IOHprofiler_create_folder(const std::string path);

  // Creating the folder for logging files of the corresponding problem.
  int openIndex();

  
  // Returns a name that is allowed.
  std::string IOHprofiler_experiment_folder_name();
};

#endif