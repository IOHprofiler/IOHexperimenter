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
    
    configSuite = genericGenerator<IOHprofiler_suite<int>>::instance().create(conf.get_suite_name());
    configSuite->IOHprofiler_set_suite_problem_id(conf.get_problem_id());
    configSuite->IOHprofiler_set_suite_instance_id(conf.get_instance_id());
    configSuite->IOHprofiler_set_suite_dimension(conf.get_dimension());
    configSuite->loadProblem();

    std::shared_ptr<IOHprofiler_csv_logger> logger(new IOHprofiler_csv_logger("./",conf.get_result_folder(),conf.get_algorithm_name(),conf.get_algorithm_info()));
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
    this->config_csv_logger->target_suite(this->configSuite->IOHprofiler_suite_get_suite_name());

    /// Problems are tested one by one until 'get_next_problem' returns NULL.
    while (current_problem = configSuite->get_next_problem()) {

      this->config_csv_logger->target_problem(current_problem->IOHprofiler_get_problem_id(), 
                                              current_problem->IOHprofiler_get_number_of_variables(), 
                                              current_problem->IOHprofiler_get_instance_id());

      algorithm(current_problem,this->config_csv_logger);
      int count = 1;
      while(independent_runs > count) {
        current_problem = configSuite->get_current_problem();
        this->config_csv_logger->target_problem(current_problem->IOHprofiler_get_problem_id(), 
                                              current_problem->IOHprofiler_get_number_of_variables(), 
                                              current_problem->IOHprofiler_get_instance_id());
        algorithm(current_problem,this->config_csv_logger);
        ++count;
      }
    }
  };

  void _set_independent_runs(int n) {
    this->independent_runs = n;
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