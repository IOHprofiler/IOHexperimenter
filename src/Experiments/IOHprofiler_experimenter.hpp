#ifndef _IOHPROFILER_EXPERIMENTER_HPP
#define _IOHPROFILER_EXPERIMENTER_HPP

#include "../common.h"
#include "../Suites/PBO_suite.hpp"
#include "../IOHprofiler_suite.hpp"
#include "../IOHprofiler_csv_logger.hpp"
#include "IOHprofiler_configuration.hpp"
#include <functional>


template <class InputType> class IOHprofiler_experimenter {
public:
  typedef void _algorithm(std::shared_ptr<IOHprofiler_problem<InputType>>);

  IOHprofiler_experimenter(){};
  IOHprofiler_experimenter(std::string configFileName, _algorithm *algorithm) {
    this->conf.readcfg(configFileName);
    if(conf.get_suite_name() == "PBO")
    configSuite = std::shared_ptr<IOHprofiler_suite<InputType>>(new PBO_suite(conf.get_problem_id(),conf.get_instance_id(),conf.get_dimension()));
    config_csv_logger.init_logger("./",conf.get_result_folder(),conf.get_algorithm_name(),conf.get_algorithm_info(),
                conf.get_complete_triggers(),conf.get_update_triggers(),conf.get_number_interval_triggers(),
                conf.get_base_evaluation_triggers(),conf.get_number_target_triggers());
    configSuite->addCSVLogger(config_csv_logger);
    this->algorithm = algorithm;
  };


  IOHprofiler_experimenter(IOHprofiler_suite<InputType> suite, IOHprofiler_csv_logger csv_logger, _algorithm * algorithm) {
    configSuite = suite;
    config_csv_logger = csv_logger;
    configSuite->addCSVLogger(config_csv_logger);
    this->algorithm = algorithm;
  };

  ~IOHprofiler_experimenter(){};

  void _run() {
    int index = 0;
    // Problems are tested one by one until 'get_next_problem' returns NULL.
    while(index != configSuite->get_size()){
      current_problem = configSuite->get_next_problem(index);
      algorithm(current_problem);
      index++;
    }
  };


private:
  IOHprofiler_configuration conf;
  std::shared_ptr<IOHprofiler_suite<InputType>> configSuite;
  std::shared_ptr<IOHprofiler_problem<InputType>> current_problem;


  IOHprofiler_csv_logger config_csv_logger;

  
  _algorithm *algorithm;
  //std::function<void(IOHprofiler_problem<InputType>)> _algorithm;
    
};

#endif