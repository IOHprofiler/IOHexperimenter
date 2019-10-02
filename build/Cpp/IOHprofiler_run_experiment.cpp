#include "../../src/Template/Experiments/IOHprofiler_experimenter.hpp"

IOHprofiler_random random_generator(1);
static int budget_scale = 100;

std::vector<int> Initialization(int dimension) {
  std::vector<int> x;
  x.reserve(dimension);
  for (int i = 0; i != dimension; ++i) {
      x.push_back((int)(random_generator.IOHprofiler_uniform_rand() * 2));
  }
  return x;
};

int mutation(std::vector<int> &x, double mutation_rate) {
  int result = 0;
  int n = x.size();
  for(int i = 0; i != n; ++i) {
    if(random_generator.IOHprofiler_uniform_rand() < mutation_rate) {
      x[i] = (x[i] + 1) % 2;
      result = 1;
    }
  }
  return result;
}


/// This is an (1+1)_EA with static mutation rate = 1/n.
/// An example for discrete optimization problems, such as PBO suite.
void evolutionary_algorithm(std::shared_ptr<IOHprofiler_problem<int> > problem, std::shared_ptr<IOHprofiler_csv_logger> logger) {
  /// Declaration for variables in the algorithm
  std::vector<int> x;
  std::vector<int> x_star;
  double y;
  double best_value;
  double * mutation_rate = new double(1);
  *mutation_rate = 1.0/problem->IOHprofiler_get_number_of_variables();
  int budget = budget_scale * problem->IOHprofiler_get_number_of_variables() * problem->IOHprofiler_get_number_of_variables();

  std::vector<std::shared_ptr<double>> parameters;
  parameters.push_back(std::shared_ptr<double>(mutation_rate));
  std::vector<std::string> parameters_name;
  parameters_name.push_back("mutation_rate");
  logger->set_parameters(parameters,parameters_name);

  x = Initialization(problem->IOHprofiler_get_number_of_variables());
  x_star = x;
  y = problem->evaluate(x);
  logger->write_line(problem->loggerInfo());
  best_value = y;

  int count = 0;
  while (count <= budget && !problem->IOHprofiler_hit_optimal()) {
    x = x_star;
    if (mutation(x,*mutation_rate)) {
      y = problem->evaluate(x);
      logger->write_line(problem->loggerInfo());
    }
    if (y >= best_value) {
      best_value = y;
      x_star = x;
    }
    count++;
  }
}

/// This is an (1+1)_EA with static mutation rate = 1/n.
/// An example for continuous optimization problems, such as BBOB suite.
void random_search(std::shared_ptr<IOHprofiler_problem<double> > problem, std::shared_ptr<IOHprofiler_csv_logger> logger) {
  /// Declaration for variables in the algorithm
  std::vector<double> x(problem->IOHprofiler_get_number_of_variables());
  double y;

  int count = 0;
  while (count <= 500) {
    for (int i = 0; i != problem->IOHprofiler_get_number_of_variables(); ++i) {
      x[i] = random_generator.IOHprofiler_uniform_rand() * 10 - 5;
    }
  
    y = problem->evaluate(x);
    logger->write_line(problem->loggerCOCOInfo());
    count++;
  }
}

void _run_experiment() {
  std::string configName = "./configuration.ini";
  /// An example for PBO suite.
  IOHprofiler_experimenter<int> experimenter(configName,evolutionary_algorithm);

  /// An exmaple for BBOB suite.
  /// IOHprofiler_experimenter<double> experimenter(configName, random_search);
  experimenter._set_independent_runs(10);
  experimenter._run();
}

int main(){
  _run_experiment();
  return 0;
}

