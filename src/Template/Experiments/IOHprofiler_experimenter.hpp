#ifndef _IOHPROFILER_EXPERIMENTER_HPP
#define _IOHPROFILER_EXPERIMENTER_HPP

#include "../IOHprofiler_common.h"
#include "../../Suites/IOHprofiler_all_suites.hpp"
#include "../Loggers/IOHprofiler_csv_logger.h"
#include "IOHprofiler_configuration.hpp"

#include <mutex>
#include <chrono>
#include "../../ctpl_stl.h"


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

    std::shared_ptr<IOHprofiler_csv_logger> logger(new IOHprofiler_csv_logger(conf.get_output_directory(),conf.get_result_folder(),conf.get_algorithm_name(),conf.get_algorithm_info()));
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
    auto start_overall = std::chrono::high_resolution_clock::now();

    std::string info = "IOHprofiler_experiment\n";
    info += "Suite: " + this->configSuite->IOHprofiler_suite_get_suite_name() + "\n";
    info += "Problem: " + vectorToString(this->configSuite->IOHprofiler_suite_get_problem_id()) + "\n";
    info += "Instance: " + vectorToString(this->configSuite->IOHprofiler_suite_get_instance_id()) + "\n";
    info += "Dimension: " + vectorToString(this->configSuite->IOHprofiler_suite_get_dimension());
    print_info(info);

    size_t num_problems = configSuite->size();
    size_t num_done = 0;
    std::mutex num_done_mutex;

    ctpl::thread_pool pool(conf.get_number_threads());
    for (size_t problem_nr = 0; problem_nr < num_problems; ++problem_nr) {
      pool.push([this, problem_nr, num_problems, &num_done, &num_done_mutex](int _thread_id) {
        std::shared_ptr<IOHprofiler_csv_logger> local_logger(new IOHprofiler_csv_logger(*config_csv_logger));
        local_logger->target_suite(this->configSuite->IOHprofiler_suite_get_suite_name());

        std::shared_ptr<IOHprofiler_problem<InputType>> current_problem = (*configSuite)[problem_nr];
        std::string id = "f";
        id += std::to_string(current_problem->IOHprofiler_get_problem_id());
        id += "_d" + std::to_string(current_problem->IOHprofiler_get_number_of_variables());
        id += "_i" + std::to_string(current_problem->IOHprofiler_get_instance_id());


        auto start = std::chrono::high_resolution_clock::now();
        print_info("\nStarted " + id + "  ");

        current_problem->reset_problem();
        local_logger->target_problem(current_problem->IOHprofiler_get_problem_id(), 
                                     current_problem->IOHprofiler_get_number_of_variables(), 
                                     current_problem->IOHprofiler_get_instance_id(),
                                     current_problem->IOHprofiler_get_problem_name());
        algorithm(current_problem,local_logger);
        
        int count = 1;
        while(independent_runs > count) {
          current_problem->reset_problem();
          local_logger->target_problem(current_problem->IOHprofiler_get_problem_id(), 
                                       current_problem->IOHprofiler_get_number_of_variables(), 
                                       current_problem->IOHprofiler_get_instance_id(),
                                       current_problem->IOHprofiler_get_problem_name());
          algorithm(current_problem,local_logger);
          print_info(".");
          ++count;
        }

        {
          std::lock_guard<std::mutex> guard(num_done_mutex);
          num_done += 1;

          auto end = std::chrono::high_resolution_clock::now();
          print_info("\nFinished " + id + " Time " + std::to_string(std::chrono::duration<double>(end-start).count()) + "s [" + std::to_string(num_done) + "/" + std::to_string(num_problems) + "]  ");
        }
      });
    }
    pool.stop(true);

    auto end_overall = std::chrono::high_resolution_clock::now();
    print_info("\nTotal Time " + std::to_string(std::chrono::duration<double>(end_overall-start_overall).count()) + "s\n");
  };

  void _set_independent_runs(int n) {
    this->independent_runs = n;
  }

  void print_info(std::string info) {
    std::lock_guard<std::mutex> guard(output_mutex);
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
  std::mutex output_mutex;


  _algorithm *algorithm;
};

#endif
