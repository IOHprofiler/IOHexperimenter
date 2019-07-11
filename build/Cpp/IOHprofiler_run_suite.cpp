#include "../../src/Suites/IOHprofiler_PBO_suite.hpp"

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
void evolutionary_algorithm(std::shared_ptr<IOHprofiler_problem<int>> problem) {
  /// Declaration for variables in the algorithm
  std::vector<int> x;
  std::vector<int> x_star;
  std::vector<double> y;
  double best_value;
  double mutation_rate = 1.0/problem->IOHprofiler_get_number_of_variables();

  x = Initialization(problem->IOHprofiler_get_number_of_variables());
  copyVector(x,x_star);
  y = problem->evaluate(x);
  best_value = y[0];

  int count= 0;
  while (count <= 100) {
    copyVector(x_star,x);
    if (mutation(x,mutation_rate)) {
      y = problem->evaluate(x);
    }
    if (y[0] > best_value) {
      best_value = y[0];
      copyVector(x,x_star);
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

  /// If no logger is added, there will be not any output files, but users
  /// can still get fitness values.
  std::vector<int> time_points{1,2,5};
  std::shared_ptr<IOHprofiler_csv_logger> logger1(new IOHprofiler_csv_logger("./","run_suite","EA","EA"));
  logger1->set_complete_flag(true);
  logger1->set_interval(2);
  logger1->set_time_points(time_points,3);
  logger1->activate_logger();
  pbo.addCSVLogger(logger1);
  std::shared_ptr<IOHprofiler_problem<int>> problem;

  /// Problems are tested one by one until 'get_next_problem' returns NULL.
  while (problem = pbo.get_next_problem()) {
    evolutionary_algorithm(problem);
  }
}


int main(){
  _run_suite();
  return 0;
}

