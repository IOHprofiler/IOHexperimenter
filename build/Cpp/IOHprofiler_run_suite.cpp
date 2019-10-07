#include "../../src/Suites/IOHprofiler_PBO_suite.hpp"
#include "../../src/Template/Loggers/IOHprofiler_csv_logger.h"

std::vector<int> Initialization(int dimension) {
  std::vector<int> x;
  x.reserve(dimension);
  for(int i = 0; i != dimension; ++i){
      x.push_back(rand()% 2);
  }
  return x;
};

int mutation(std::vector<int> &x, double mutation_rate) {
  int result = 0;
  int n = x.size();
  for(int i = 0; i != n; ++i) {
    if(rand() / double(RAND_MAX) < mutation_rate) {
      x[i] = (x[i] + 1) % 2;
      result = 1;
    }
  }
  return result;
}

/// This is an (1+1)_EA with static mutation rate = 1/n.
void evolutionary_algorithm(std::shared_ptr<IOHprofiler_problem<int>> problem, std::shared_ptr<IOHprofiler_csv_logger> logger) {
  /// Declaration for variables in the algorithm
  std::vector<int> x;
  std::vector<int> x_star;
  double y;
  double best_value;
  double mutation_rate = 1.0/problem->IOHprofiler_get_number_of_variables();

  x = Initialization(problem->IOHprofiler_get_number_of_variables());
  x_star = x;
  y = problem->evaluate(x);
  logger->write_line(problem->loggerInfo());
  best_value = y;

  int count= 0;
  while (count <= 100) {
    x = x_star;
    if (mutation(x,mutation_rate)) {
      y = problem->evaluate(x);
      logger->write_line(problem->loggerInfo());
    }
    if (y > best_value) {
      best_value = y;
      x_star = x;
    }
    count++;
  }
}

/// In this session multiple problems in PBO suite will be tested.
/// In the example, specfic problems are selected from PBO_suite.
/// User can also get a complete problem collection with default 
/// instances and dimension by the statement '  PBO_suite pbo;'
void _run_suite() {
  
  /// Allocate the suite to be tested
  std::vector<int> problem_id = {1,2};
  std::vector<int> instance_id ={1,2};
  std::vector<int> dimension = {100,200,300};
  PBO_suite pbo(problem_id,instance_id,dimension);
  pbo.loadProblem();
  /// If no logger is added, there will be not any output files, but users
  /// can still get fitness values.
  std::vector<int> time_points{1,2,5};
  std::shared_ptr<IOHprofiler_csv_logger> logger(new IOHprofiler_csv_logger("./","run_suite","EA","EA"));
  logger->set_complete_flag(true);
  logger->set_interval(2);
  logger->set_time_points(time_points,3);
  logger->activate_logger();
  logger->target_suite(pbo.IOHprofiler_suite_get_suite_name());
  
  std::shared_ptr<IOHprofiler_problem<int>> problem;

  /// Problems are tested one by one until 'get_next_problem' returns NULL.
  while ((problem = pbo.get_next_problem()) != nullptr) {
    logger->target_problem(problem->IOHprofiler_get_problem_id(), 
                          problem->IOHprofiler_get_number_of_variables(), 
                          problem->IOHprofiler_get_instance_id(),
                          problem->IOHprofiler_get_problem_name(),
                          problem->IOHprofiler_get_optimization_type());
    evolutionary_algorithm(problem,logger);
  }
}


int main(){
  _run_suite();
  return 0;
}

