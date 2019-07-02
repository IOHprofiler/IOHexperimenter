#include "../../Problems/f_one_max.hpp"

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
  std::shared_ptr<IOHprofiler_csv_logger> logger(new IOHprofiler_csv_logger("./","run_problem","EA","EA"));
  logger->set_complete_flag(true);
  logger->set_interval(0);
  logger->set_time_points(time_points,10);
  logger->activate_logger();
  om.addCSVLogger(std::move(logger));

  std::vector<int> x;
  std::vector<int> x_star;
  std::vector<double> y;
  double best_value;
  double mutation_rate = 1.0/dimension;

  x = Initialization(dimension);
  copyVector(x,x_star);
  y = om.evaluate(x);
  best_value = y[0];

  while(!om.IOHprofiler_hit_optimal()) {
    copyVector(x_star,x);
    if(mutation(x,mutation_rate)) {
      y = om.evaluate(x);
    }
    if(y[0] > best_value) {
      best_value = y[0];
      copyVector(x,x_star);
    }
  }
}

int main(){
  _run_problem();
  return 0;
}

