/// \file interfaceR.cpp
/// \brief Implementation of interface functions for R version.
///
/// \author Furong Ye
/// \date 2019-10-08

#include <Rcpp.h>


// Taken from http://tolstoy.newcastle.edu.au/R/e2/devel/06/11/1242.html
// Undefine the Realloc macro, which is defined by both R and by Windows stuff
#undef Realloc
// Also need to undefine the Free macro
#undef Free


// [[Rcpp::plugins(cpp11)]]


#include "Headers/IOHprofiler_all_suites.hpp"
#include "Headers/IOHprofiler_csv_logger.h"


using namespace Rcpp;

std::shared_ptr<IOHprofiler_suite<int> > currentIntSuite;
std::shared_ptr<IOHprofiler_problem<int> > currentIntProblem;
std::shared_ptr<IOHprofiler_suite<double> > currentDoubleSuite;
std::shared_ptr<IOHprofiler_problem<double> > currentDoubleProblem;
std::string currentSuiteName;
std::shared_ptr<IOHprofiler_csv_logger> currentLogger;
//std::vector<std::shared_ptr<double> > currentParameters;

//[[Rcpp::export]]
int cpp_init_suite(String suite_name, const std::vector<int> &problem_id, const std::vector<int> &instance_id, const std::vector<int> &dimension) {
  if (suite_name == "PBO") {
    currentSuiteName = suite_name;
    currentIntSuite = genericGenerator<IOHprofiler_suite<int> >::instance().create(suite_name); 
    currentIntSuite->IOHprofiler_set_suite_problem_id(problem_id);
    currentIntSuite->IOHprofiler_set_suite_instance_id(instance_id);
    currentIntSuite->IOHprofiler_set_suite_dimension(dimension); 
    currentIntSuite->loadProblem();
    return 0;
  } else if (suite_name == "BBOB") {
    currentSuiteName = suite_name;
    currentDoubleSuite = genericGenerator<IOHprofiler_suite<double> >::instance().create(suite_name);
    currentDoubleSuite->IOHprofiler_set_suite_problem_id(problem_id);
    currentDoubleSuite->IOHprofiler_set_suite_instance_id(instance_id);
    currentDoubleSuite->IOHprofiler_set_suite_dimension(dimension); 
    currentDoubleSuite->loadProblem();
    return 0;
  } else {
    Rcout << "This Suite does not exist.\n";
    return 1;
  }
}

//[[Rcpp::export]]
int cpp_init_logger(String output_directory, String result_folder, String algorithm_name, String algorithm_info,
                    bool dat, bool cdat, int tdat, int idat) {
  std::shared_ptr<IOHprofiler_csv_logger> logger(new IOHprofiler_csv_logger(output_directory, result_folder, algorithm_name, algorithm_info));
  if (logger == nullptr) {
    Rcout << "Creating logger fails.\n";
    return 1;
  }
  logger->set_complete_flag(cdat);
  logger->set_interval(idat);
  logger->set_time_points(std::vector<int> (tdat),0);
  logger->set_update_flag(dat);
  currentLogger = logger;
  currentLogger->activate_logger();
  return 0;
}

//[[Rcpp::export]]
IntegerVector cpp_get_problem_list() {
  if (currentSuiteName == "PBO" && currentIntSuite != nullptr) {
    return wrap(currentIntSuite->IOHprofiler_suite_get_problem_id());
  } else if (currentSuiteName == "BBOB" && currentDoubleSuite != nullptr) {
    return wrap(currentDoubleSuite->IOHprofiler_suite_get_problem_id());
  } else {
    Rcout << "No suite exists.\n";
    return R_NilValue;
  }
}

//[[Rcpp::export]]
IntegerVector cpp_get_dimension_list() {
  if (currentSuiteName == "PBO" && currentIntSuite != nullptr) {
    return wrap(currentIntSuite->IOHprofiler_suite_get_dimension());
  } else if (currentSuiteName == "BBOB" && currentDoubleSuite != nullptr) {
    return wrap(currentDoubleSuite->IOHprofiler_suite_get_dimension());
  } else {
    Rcout << "No suite exists.\n";
    return R_NilValue;
  }
}

//[[Rcpp::export]]
IntegerVector cpp_get_instance_list() {
  if (currentSuiteName == "PBO" && currentIntSuite != nullptr) {
    return wrap(currentIntSuite->IOHprofiler_suite_get_instance_id());
  } else if (currentSuiteName == "BBOB" && currentDoubleSuite != nullptr) {
    return wrap(currentDoubleSuite->IOHprofiler_suite_get_instance_id());
  } else {
    Rcout << "No suite exists.\n";
    return R_NilValue;
  }
}

//[[Rcpp::export]]
List cpp_get_suite_info() {
  if (currentSuiteName == "PBO" && currentIntSuite != nullptr) {
    return List::create(_["problems"] = cpp_get_problem_list(),
                        _["dimensions"] = cpp_get_dimension_list(),
                        _["instances"] = cpp_get_instance_list());           
  } else if (currentSuiteName == "BBOB" && currentDoubleSuite != nullptr) {
    return List::create(_["problems"] = cpp_get_problem_list(),
                        _["dimensions"] = cpp_get_dimension_list(),
                        _["instances"] = cpp_get_instance_list());   
  } else {
    Rcout << "No suite exists.\n";
    return R_NilValue;
  }
}

//[[Rcpp::export]]
List cpp_get_problem_info() {
  if (currentSuiteName == "PBO" && currentIntProblem != nullptr) {
    return List::create(_["problem"] = currentIntProblem->IOHprofiler_get_problem_name(),
                        _["dimension"] = currentIntProblem->IOHprofiler_get_number_of_variables(),
                        _["instance"] = currentIntProblem->IOHprofiler_get_instance_id());           
  } else if (currentSuiteName == "BBOB" && currentDoubleProblem != nullptr) {
    return List::create(_["problem"] = currentDoubleProblem->IOHprofiler_get_problem_name(),
                        _["dimension"] = currentDoubleProblem->IOHprofiler_get_number_of_variables(),
                        _["instance"] = currentDoubleProblem->IOHprofiler_get_instance_id());  
  } else {
    Rcout << "No suite exists.\n";
    return R_NilValue;
  }
}

//[[Rcpp::export]]
List cpp_get_next_problem() {
  if (currentSuiteName == "PBO" && currentIntSuite != nullptr) {
    currentIntProblem = currentIntSuite->get_next_problem();
    if (currentIntProblem == nullptr) {
      Rcout << "No problem left.\n";
      return R_NilValue;
    }
    if (currentLogger != nullptr) {
      currentLogger->track_problem(currentIntProblem->IOHprofiler_get_problem_id(), 
                                    currentIntProblem->IOHprofiler_get_number_of_variables(), 
                                    currentIntProblem->IOHprofiler_get_instance_id(),
                                    currentIntProblem->IOHprofiler_get_problem_name(),
                                    currentIntProblem->IOHprofiler_get_optimization_type());
    }
    return cpp_get_problem_info();  
  } else if (currentSuiteName == "BBOB" && currentDoubleSuite != nullptr) {
    currentDoubleProblem = currentDoubleSuite->get_next_problem();
    if (currentDoubleProblem == nullptr) {
      Rcout << "No problem left.\n";
      return R_NilValue;
    }
    if (currentLogger != nullptr) {
      currentLogger->track_problem(currentDoubleProblem->IOHprofiler_get_problem_id(), 
                                    currentDoubleProblem->IOHprofiler_get_number_of_variables(), 
                                    currentDoubleProblem->IOHprofiler_get_instance_id(),
                                    currentDoubleProblem->IOHprofiler_get_problem_name(),
                                    currentDoubleProblem->IOHprofiler_get_optimization_type());
    } 
    return cpp_get_problem_info();  
  } else {
    Rcout << "No suite exists.\n";
    return R_NilValue;
  }
}

//[[Rcpp::export]]
List cpp_reset_problem() {
  if (currentSuiteName == "PBO" && currentIntSuite != nullptr) {
    currentIntProblem = currentIntSuite->get_current_problem();
    if (currentLogger != nullptr && currentIntProblem != nullptr) {
      currentLogger->track_problem(currentIntProblem->IOHprofiler_get_problem_id(), 
                                    currentIntProblem->IOHprofiler_get_number_of_variables(), 
                                    currentIntProblem->IOHprofiler_get_instance_id(),
                                    currentIntProblem->IOHprofiler_get_problem_name(),
                                    currentIntProblem->IOHprofiler_get_optimization_type());
    }
    return cpp_get_problem_info();
  } else if (currentSuiteName == "BBOB" && currentDoubleSuite != nullptr) {
    currentDoubleProblem = currentDoubleSuite->get_current_problem();
    if (currentLogger != nullptr && currentDoubleProblem != nullptr) {
      currentLogger->track_problem(currentDoubleProblem->IOHprofiler_get_problem_id(), 
                                    currentDoubleProblem->IOHprofiler_get_number_of_variables(), 
                                    currentDoubleProblem->IOHprofiler_get_instance_id(),
                                    currentDoubleProblem->IOHprofiler_get_problem_name(),
                                    currentDoubleProblem->IOHprofiler_get_optimization_type());
    }
    return cpp_get_problem_info();    
  } else {
    Rcout << "No suite exists.\n";
    return R_NilValue;
  }
}

//[[Rcpp::export]]
double cpp_int_evaluate(const std::vector<int> &x) {
  if (currentIntProblem == nullptr) {
    Rcout << "Error! no function selected.\n";
    return -DBL_MAX;
  }

  if (x.size() != currentIntProblem->IOHprofiler_get_number_of_variables()) {
    Rcout << "Error! The length of input vector is incorrect.\n";
    return -DBL_MAX;
  }
  return currentIntProblem->evaluate(x);
}

//[[Rcpp::export]]
double cpp_double_evaluate(const std::vector<double> & x) {
  if (currentDoubleProblem == nullptr) {
    Rcout << "Error! no function selected.\n";
    return -DBL_MAX;
  }


  if (x.size() != currentDoubleProblem->IOHprofiler_get_number_of_variables()) {
    Rcout << "Error! The length of input vector is incorrect. It should be lenght: " << 
      currentDoubleProblem->IOHprofiler_get_number_of_variables() << "\n";
    return -DBL_MAX;
  }
  return currentDoubleProblem->evaluate(x);
}

//[[Rcpp::export]]
NumericVector cpp_loggerCOCOInfo() {
  if (currentSuiteName == "PBO" && currentIntSuite != nullptr) {
    if (currentIntProblem == nullptr) {
      Rcout << "Error: No problem exist!\n";
      return R_NilValue;
    } else {
      return wrap(currentIntProblem->loggerCOCOInfo());
    }
  } else if (currentSuiteName == "BBOB" && currentDoubleSuite != nullptr) {
    if (currentDoubleProblem == nullptr) {
      Rcout << "Error: No problem exist!\n";
      return R_NilValue;
    } else {
      return wrap(currentDoubleProblem->loggerCOCOInfo());
    }
  } else {
    Rcout << "Error: No problem exist!\n";
    return R_NilValue;
  }
}

//[[Rcpp::export]]
NumericVector cpp_loggerInfo() {
  if (currentSuiteName == "PBO" && currentIntSuite != nullptr) {
    if (currentIntProblem == nullptr) {
      Rcout << "Error: No problem exist!\n";
      return R_NilValue;
    } else {
      return wrap(currentIntProblem->loggerInfo());
    }
  } else if (currentSuiteName == "BBOB" && currentDoubleSuite != nullptr) {
    if (currentDoubleProblem == nullptr) {
      Rcout << "Error: No problem exist!\n";
      return R_NilValue;
    } else {
      return wrap(currentDoubleProblem->loggerInfo());
    }
  } else {
    Rcout << "Error: No problem exist!\n";
    return R_NilValue;
  }
}

//[[Rcpp::export]]
int cpp_write_line(const std::vector<double> &line_info) {
  if (currentLogger == nullptr) {
    Rcout << "Error! No logger exists.\n";
    return 1;
  } else {
    currentLogger->write_line(line_info);
    return 0;
  }
}

//[[Rcpp::export]]
int cpp_do_log(const std::vector<double> &line_info) {
  if (currentLogger == nullptr) {
    Rcout << "Error! No logger exists.\n";
    return 1;
  } else {
    currentLogger->do_log(line_info);
    return 0;
  }
}

// Since we suppose that users will only get problems from a suite, only target_suite function is provided here.
//[[Rcpp::export]]
int cpp_logger_target_suite() {
  if (currentLogger == nullptr) {
    Rcout << "Error! No logger exists.\n";
    return 1;
  } else if ((currentSuiteName == "PBO" && currentIntSuite != nullptr) || 
    (currentSuiteName == "BBOB" && currentDoubleSuite != nullptr))  {
    currentLogger->track_suite(currentSuiteName);
    return 0;
  } else {
    Rcout << "Error! No suite exists.\n";
    return 1;
  }
}


//[[Rcpp::export]]
int cpp_set_parameters_name(const std::vector<std::string> &parameters_name) {
  if (currentLogger == nullptr) {
    Rcout << "Error! No logger exists.\n";
    return 1;
  }
  currentLogger->set_parameters_name(parameters_name);
  return 0;
}

//[[Rcpp::export]]
int cpp_set_parameters(const std::vector<std::string> & parameters_name,const std::vector<double> &parameters) {
  if (currentLogger == nullptr) {
    Rcout << "Error! No logger exists.\n";
    return 1;
  }
  currentLogger->set_parameters_name(parameters_name,parameters);
  return 0;
}

//[[Rcpp::export]]
int cpp_add_double_attribute(std::string name, double value) {
  if (currentLogger == nullptr) {
    Rcout << "Error! No logger exists.\n";
    return 1;
  }
  currentLogger->add_attribute(name,value);
  return 0;
}

//[[Rcpp::export]]
int cpp_add_int_attribute(std::string name, int value) {
  if (currentLogger == nullptr) {
    Rcout << "Error! No logger exists.\n";
    return 1;
  }
  currentLogger->add_attribute(name,value);
  return 0;
}

//[[Rcpp::export]]
int cpp_add_string_attribute1(std::string name, std::string value) {
  if (currentLogger == nullptr) {
    Rcout << "Error! No logger exists.\n";
    return 1;
  }
  currentLogger->add_attribute(name,value);
  return 0;
}

//[[Rcpp::export]] 
int cpp_delete_attribute(std::string name) {
  if (currentLogger == nullptr) {
    Rcout << "Error! No logger exists.\n";
    return 1;
  }
  currentLogger->delete_attribute(name);
  return 0;
}

//[[Rcpp::export]]
bool cpp_is_target_hit() {
  if (currentSuiteName == "PBO" && currentIntSuite != nullptr) {
    if (currentIntProblem == nullptr) {
      Rcout << "Error: No problem exist!\n";
      return false;
    } else {
      return currentIntProblem->IOHprofiler_hit_optimal();
    }
  } else if (currentSuiteName == "BBOB" && currentDoubleSuite != nullptr) {
    if (currentDoubleProblem == nullptr) {
      Rcout << "Error: No problem exist!\n";
      return false;
    } else {
      return currentDoubleProblem->IOHprofiler_hit_optimal();
    }
  } else {
    Rcout << "Error: No problem exist!\n";
    return false;
  }
}

//[[Rcpp::export]]
int cpp_get_evaluations() {
  if (currentSuiteName == "PBO" && currentIntSuite != nullptr) {
    if (currentIntProblem == nullptr) {
      Rcout << "Error: No problem exist!\n";
      return -1;
    } else {
      return currentIntProblem->IOHprofiler_get_evaluations();
    }
  } else if (currentSuiteName == "BBOB" && currentDoubleSuite != nullptr) {
    if (currentDoubleProblem == nullptr) {
      Rcout << "Error: No problem exist!\n";
      return -1;
    } else {
      return currentDoubleProblem->IOHprofiler_get_evaluations();
    }
  } else {
    Rcout << "Error: No problem exist!\n";
    return -1;
  }
}

//[[Rcpp::export]]
double cpp_get_optimal() {
  if (currentSuiteName == "PBO" && currentIntSuite != nullptr) {
    if (currentIntProblem == nullptr) {
      Rcout << "Error: No problem exist!\n";
      return -1;
    } else {
      return currentIntProblem->IOHprofiler_get_optimal()[0];
    }
  } else if (currentSuiteName == "BBOB" && currentDoubleSuite != nullptr) {
    if (currentDoubleProblem == nullptr) {
      Rcout << "Error: No problem exist!\n";
      return -1;
    } else {
      return currentDoubleProblem->IOHprofiler_get_optimal()[0];
    }
  } else {
    Rcout << "Error: No problem exist!\n";
    return -1;
  }
}

//[[Rcpp::export]]
NumericVector cpp_get_double_upper_bounds() {
  if (currentSuiteName == "BBOB" && currentDoubleSuite != nullptr) {
    if (currentDoubleProblem == nullptr) {
      Rcout << "Error: No problem exist!\n";
      return -1;
    } else {
      return wrap(currentDoubleProblem->IOHprofiler_get_upperbound());
    }
  } else {
    Rcout << "Error: No problem exist!\n";
    return R_NilValue;
  }
} 

//[[Rcpp::export]]
NumericVector cpp_get_double_lower_bounds() {
  if (currentSuiteName == "BBOB" && currentDoubleSuite != nullptr) {
    if (currentDoubleProblem == nullptr) {
      Rcout << "Error: No problem exist!\n";
      return -1;
    } else {
      return wrap(currentDoubleProblem->IOHprofiler_get_lowerbound());
    }
  } else {
    Rcout << "Error: No problem exist!\n";
    return R_NilValue;
  }
} 

//[[Rcpp::export]]
IntegerVector cpp_get_int_upper_bounds() {
  if (currentSuiteName == "PBO" && currentIntSuite != nullptr) {
    if (currentIntProblem == nullptr) {
      Rcout << "Error: No problem exist!\n";
      return -1;
    } else {
      return wrap(currentIntProblem->IOHprofiler_get_upperbound());
    }
  } else {
    Rcout << "Error: No problem exist!\n";
    return R_NilValue;
  }
} 

//[[Rcpp::export]]
IntegerVector cpp_get_int_lower_bounds() {
  if (currentSuiteName == "PBO" && currentIntSuite != nullptr) {
    if (currentIntProblem == nullptr) {
      Rcout << "Error: No problem exist!\n";
      return -1;
    } else {
      return wrap(currentIntProblem->IOHprofiler_get_lowerbound());
    }
  } else {
    Rcout << "Error: No problem exist!\n";
    return R_NilValue;
  }
} 

/// Return 1 if it is maximization.
//[[Rcpp::export]]
int cpp_get_optimization_type() {
  if (currentSuiteName == "PBO" && currentIntSuite != nullptr) {
    if (currentIntProblem == nullptr) {
      Rcout << "Error: No problem exist!\n";
      return -1;
    } else {
      return currentIntProblem->IOHprofiler_get_optimization_type();
    }
  } else if (currentSuiteName == "BBOB" && currentDoubleSuite != nullptr) {
    if (currentDoubleProblem == nullptr) {
      Rcout << "Error: No problem exist!\n";
      return -1;
    } else {
      return currentDoubleProblem->IOHprofiler_get_optimization_type();
    }
  } else {
    Rcout << "Error: No problem exist!\n";
    return -1;
  }
} 

void cpp_clear_int_problem() {
  currentIntProblem = nullptr;
}

void cpp_clear_double_problem() {
  currentDoubleProblem = nullptr;
}

//[[Rcpp::export]]
void cpp_clear_problem() {
  cpp_clear_int_problem();
  cpp_clear_double_problem();
}


void cpp_clear_int_suite() {
  currentIntSuite = nullptr;
  currentIntProblem = nullptr;
}

void cpp_clear_double_suite() {
  currentDoubleSuite = nullptr;
  currentDoubleProblem = nullptr;
}

//[[Rcpp::export]]
void cpp_clear_suite() {
  cpp_clear_int_suite();
  cpp_clear_double_suite();
}

//[[Rcpp::export]]
void cpp_clear_logger() {
  if (currentLogger != nullptr){
    currentLogger->clear_logger();
  }
  currentLogger = nullptr;
}
