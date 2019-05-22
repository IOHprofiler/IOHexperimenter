#include "../Problems/f_one_max.hpp"
#include "../IOHprofiler_csv_logger.hpp"
#include "../Suites/PBO_suite.hpp"
#include "../Experiments/IOHprofiler_experimenter.hpp"

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

// This is an (1+1)_EA with static mutation rate = 1/n.
void evolutionary_algorithm(std::shared_ptr<IOHprofiler_problem<int>> problem) {
  // Declaration for variables in the algorithm
  std::vector<int> x;
  std::vector<int> x_star;
  std::vector<double> y;
  double best_value;
  double mutation_rate = 1.0/problem->IOHprofiler_get_number_of_variables();

  x = Initialization(problem->IOHprofiler_get_number_of_variables());
  copyVector(x,x_star);
  y = problem->evaluate(x);
  best_value = y[0];

  while(!problem->IOHprofiler_hit_optimal()) {
    copyVector(x_star,x);
    if(mutation(x,mutation_rate)) {
      y = problem->evaluate(x);
    }
    if(y[0] > best_value) {
      best_value = y[0];
      copyVector(x,x_star);
    }
  }
}

// In this session, the algorithm will be tested on only one problem (OneMax).
// Therefore we declare a OneMax class, and get the fitness by the statement
// om.evaluate().
void _run_problem() {

  // Allocate the problem to be tested.
  OneMax om;
  int dimension = 1000;
  om.Initilize_problem(dimension);
  
  // If no logger is added, there will be not any output files, but users
  // can still get fitness values.
  IOHprofiler_csv_logger logger;
  std::vector<int> time_points{1,2,5};
  logger.init_logger("./","run_problem","EA","EA",true,true,2,time_points,3);
  om.addCSVLogger(logger);

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


// In this session multiple problems in PBO suite will be tested.
// In the example, specfic problems are selected from PBO_suite.
// User can also get a complete problem collection with default 
// instances and dimension by the statement '  PBO_suite pbo;'
void _run_suite() {
  
  // Allocate the suite to be tested
  std::vector<int> problem_id = {1,2};
  std::vector<int> instance_id ={1};
  std::vector<int> dimension = {100,200,300};
  PBO_suite pbo(problem_id,instance_id,dimension);

  // If no logger is added, there will be not any output files, but users
  // can still get fitness values.
  IOHprofiler_csv_logger logger1;
  std::vector<int> time_points{1,2,5};
  logger1.init_logger("./","run_suite","EA","EA",true,true,2,time_points,3);
  pbo.addCSVLogger(logger1);
  std::shared_ptr<IOHprofiler_problem<int>> problem;

  


  // Problems are tested one by one until 'get_next_problem' returns NULL.
  int index = 0;
  while(index < pbo.get_size()){
    problem = pbo.get_next_problem(index);
    evolutionary_algorithm(problem);
    index++;
  }
}

void _run_experiment() {
  std::string configName = "./configuration.ini";
  IOHprofiler_experimenter<int> experimenter(configName,evolutionary_algorithm);
  experimenter._run();
}

int main(){
  _run_problem();
  _run_suite();
  _run_experiment();
  return 0;
}

