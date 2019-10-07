#include "../../src/Problems/f_one_max.hpp"
#include "../../src/Template/Loggers/IOHprofiler_csv_logger.h"

std::vector<int> Initialization(int dimension) {
  std::vector<int> x;
  x.reserve(dimension);
  for (int i = 0; i != dimension; ++i){
      x.push_back(rand()% 2);
  }
  return x;
};

int mutation(std::vector<int> &x, double mutation_rate) {
  int result = 0;
  int n = x.size();
  for (int i = 0; i != n; ++i) {
    if (rand() / double(RAND_MAX) < mutation_rate) {
      x[i] = (x[i] + 1) % 2;
      result = 1;
    }
  }
  return result;
}

/// In this session, the algorithm will be tested on only one problem (OneMax).
/// Therefore we declare a OneMax class, and get the fitness by the statement
/// om.evaluate().
void _run_problem() {

  /// Allocate the problem to be tested.
  OneMax om;
  int dimension = 1000;
  om.Initilize_problem(dimension);
  
  /// If no logger is added, there will be not any output files, but users
  /// can still get fitness values.
  std::vector<int> time_points{1,2,5};
  IOHprofiler_csv_logger logger("./","run_problem","EA","EA");
  logger.set_complete_flag(true);
  logger.set_interval(0);
  logger.set_time_points(time_points,10);
  logger.activate_logger();
  logger.target_problem(om.IOHprofiler_get_problem_id(), 
                      om.IOHprofiler_get_number_of_variables(), 
                      om.IOHprofiler_get_instance_id(),
                      om.IOHprofiler_get_problem_name(),
                      om.IOHprofiler_get_optimization_type());

  std::vector<int> x;
  std::vector<int> x_star;
  double y;
  double best_value;
  double mutation_rate = 1.0/dimension;

  x = Initialization(dimension);
  x_star = x;
  y = om.evaluate(x);
  logger.write_line(om.loggerInfo());
  best_value = y;

  while(!om.IOHprofiler_hit_optimal()) {
    x = x_star;
    if(mutation(x,mutation_rate)) {
      y = om.evaluate(x);
      logger.write_line(om.loggerInfo());
    }
    if(y > best_value) {
      best_value = y;
      x_star = x;
    }
  }
}

int main(){
  _run_problem();
  return 0;
}

