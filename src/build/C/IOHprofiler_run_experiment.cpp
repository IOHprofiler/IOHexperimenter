#include "../../Experiments/IOHprofiler_experimenter.hpp"

std::vector<int> Initialization(int dimension) {
  std::vector<int> x;
  x.reserve(dimension);
  for (int i = 0; i != dimension; ++i) {
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

void _run_experiment() {
  std::string configName = "./configuration.ini";
  IOHprofiler_experimenter<int> experimenter(configName,evolutionary_algorithm);
  experimenter._run();
}

int main(){
  _run_experiment();
  return 0;
}

