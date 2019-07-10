#ifndef _IOHPROFILER_EXPERIMENTER_HPP
#define _IOHPROFILER_EXPERIMENTER_HPP

#include "../IOHprofiler_common.h"
#include "../../Suites/IOHprofiler_all_suites.hpp"
#include "IOHprofiler_configuration.hpp"


template <class InputType> class IOHprofiler_experimenter {
public:
  typedef void _algorithm(std::shared_ptr<IOHprofiler_problem<InputType>>);

  IOHprofiler_experimenter() {};
  IOHprofiler_experimenter(std::string configFileName, _algorithm *algorithm) {
    this->conf.readcfg(configFileName);
    
    configSuite = genericGenerator<IOHprofiler_suite<int>>::instance().create(conf.get_suite_name());
    configSuite->IOHprofiler_set_suite_problem_id(conf.get_problem_id());
    configSuite->IOHprofiler_set_suite_instance_id(conf.get_instance_id());
    configSuite->IOHprofiler_set_suite_dimension(conf.get_dimension());
    
    std::shared_ptr<IOHprofiler_csv_logger> logger(new IOHprofiler_csv_logger("./",conf.get_result_folder(),conf.get_algorithm_name(),conf.get_algorithm_info()));
    logger->set_complete_flag(conf.get_complete_triggers());
    logger->set_interval(conf.get_number_interval_triggers());
    logger->set_time_points(conf.get_base_evaluation_triggers(),conf.get_number_target_triggers());
    logger->set_update_flag(conf.get_update_triggers());
    
    config_csv_logger = logger;
    config_csv_logger->activate_logger();
    configSuite->addCSVLogger(config_csv_logger);
    
    this->algorithm = algorithm;
  };


  IOHprofiler_experimenter(IOHprofiler_suite<InputType> suite, std::shared_ptr<IOHprofiler_csv_logger> csv_logger, _algorithm * algorithm) {
    configSuite = suite;
    config_csv_logger = csv_logger;
    configSuite->addCSVLogger(config_csv_logger);
    this->algorithm = algorithm;
  };

  ~IOHprofiler_experimenter(){};

  void _run() {
    int index = 0;
    /// Problems are tested one by one until 'get_next_problem' returns NULL.
    while (current_problem = configSuite->get_next_problem()) {
      algorithm(current_problem);
      index++;
    }
  };


private:
  IOHprofiler_configuration conf;
  std::shared_ptr<IOHprofiler_suite<InputType>> configSuite;
  std::shared_ptr<IOHprofiler_problem<InputType>> current_problem;
  std::shared_ptr<IOHprofiler_csv_logger> config_csv_logger;
  
  _algorithm *algorithm;
};

#endif