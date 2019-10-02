#ifndef _IOHPROFILER_EXPERIMENTER_HPP
#define _IOHPROFILER_EXPERIMENTER_HPP

#include "../IOHprofiler_common.h"
#include "../../Suites/IOHprofiler_all_suites.hpp"
#include "../Loggers/IOHprofiler_csv_logger.h"
#include "IOHprofiler_configuration.hpp"


template <class InputType> class IOHprofiler_experimenter {
public:
  typedef void _algorithm(std::shared_ptr<IOHprofiler_problem<InputType>>, std::shared_ptr<IOHprofiler_csv_logger> logger);

  IOHprofiler_experimenter() {};
  IOHprofiler_experimenter(std::string configFileName, _algorithm *algorithm) {
      this->conf.readcfg(configFileName);
    
    configSuite = genericGenerator<IOHprofiler_suite<InputType>>::instance().create(conf.get_suite_name());
    if (configSuite == nullptr) {
      IOH_error("Creating suite fails, please check your configuration");
    }
    
    configSuite->IOHprofiler_set_suite_problem_id(conf.get_problem_id());
    configSuite->IOHprofiler_set_suite_instance_id(conf.get_instance_id());
    configSuite->IOHprofiler_set_suite_dimension(conf.get_dimension());
    configSuite->loadProblem();

    std::shared_ptr<IOHprofiler_csv_logger> logger(new IOHprofiler_csv_logger(conf.get_output_directory(),conf.get_result_folder(),conf.get_algorithm_name(),conf.get_algorithm_info()));
    if (logger == nullptr) {
      IOH_error("Creating logger fails, please check your configuration");
    }
    
    logger->set_complete_flag(conf.get_complete_triggers());
    logger->set_interval(conf.get_number_interval_triggers());
    logger->set_time_points(conf.get_base_evaluation_triggers(),conf.get_number_target_triggers());
    logger->set_update_flag(conf.get_update_triggers());
    
    config_csv_logger = logger;
    config_csv_logger->activate_logger();
    
    this->algorithm = algorithm;
  };

  IOHprofiler_experimenter(IOHprofiler_suite<InputType> suite, std::shared_ptr<IOHprofiler_csv_logger> csv_logger, _algorithm * algorithm) {
    configSuite = suite;
    config_csv_logger = csv_logger;
    this->algorithm = algorithm;
  };

  ~IOHprofiler_experimenter(){};

  void _run() {
    std::clock_t c_start_overall = std::clock();

    std::string info = "IOHprofiler_experiment\n";
    info += "Suite: " + this->configSuite->IOHprofiler_suite_get_suite_name() + "\n";
    info += "Problem: " + vectorToString(this->configSuite->IOHprofiler_suite_get_problem_id()) + "\n";
    info += "Instance: " + vectorToString(this->configSuite->IOHprofiler_suite_get_instance_id()) + "\n";
    info += "Dimension: " + vectorToString(this->configSuite->IOHprofiler_suite_get_dimension()) + "\n";
    print_info(info);

    this->config_csv_logger->target_suite(this->configSuite->IOHprofiler_suite_get_suite_name());

    /// Problems are tested one by one until 'get_next_problem' returns NULL.
    while ((current_problem = configSuite->get_next_problem()) != nullptr) {
      std::clock_t c_start = std::clock();
      info = "f";
      info += std::to_string(current_problem->IOHprofiler_get_problem_id());
      info += "_d" + std::to_string(current_problem->IOHprofiler_get_number_of_variables());
      info += "_i" + std::to_string(current_problem->IOHprofiler_get_instance_id());
      print_info(info);

      this->config_csv_logger->target_problem(current_problem->IOHprofiler_get_problem_id(), 
                                              current_problem->IOHprofiler_get_number_of_variables(), 
                                              current_problem->IOHprofiler_get_instance_id(),
                                              current_problem->IOHprofiler_get_problem_name(),
                                              current_problem->IOHprofiler_get_optimization_type());

      algorithm(current_problem,this->config_csv_logger);
      
      print_info(".");

      int count = 1;
      while(independent_runs > count) {
        current_problem = configSuite->get_current_problem();
        this->config_csv_logger->target_problem(current_problem->IOHprofiler_get_problem_id(), 
                                              current_problem->IOHprofiler_get_number_of_variables(), 
                                              current_problem->IOHprofiler_get_instance_id(),
                                              current_problem->IOHprofiler_get_problem_name(),
                                              current_problem->IOHprofiler_get_optimization_type());
        algorithm(current_problem,this->config_csv_logger);
        ++count;

        print_info(".");
      }

      std::clock_t c_end = std::clock();
      print_info("CPU Time" + std::to_string(1000.0 * (c_end-c_start) / CLOCKS_PER_SEC) + "ms\n");
    }

    std::clock_t c_end_overall = std::clock();
    print_info("Total CPU Time" + std::to_string(1000.0 * (c_end_overall-c_start_overall) / CLOCKS_PER_SEC) + "ms\n");
  };

  void _set_independent_runs(int n) {
    this->independent_runs = n;
  }

  void print_info(std::string info) {
    std::cout << info << std::flush;
  }
  
  std::string vectorToString(std::vector<int> v) {
    
    std::string s = "";
    if (v.size() == 0) {
      return s;
    }
    s = std::to_string(v[0]);
    for (int i = 1; i != v.size(); ++i) {
      s += " " + std::to_string(v[i]);
    }

    return s;
  }

private:
  IOHprofiler_configuration conf;
  std::shared_ptr<IOHprofiler_suite<InputType>> configSuite;
  std::shared_ptr<IOHprofiler_problem<InputType>> current_problem;
  std::shared_ptr<IOHprofiler_csv_logger> config_csv_logger;
  int independent_runs = 1;

  _algorithm *algorithm;
};

#endif
