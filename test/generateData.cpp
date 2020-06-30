#include <IOHprofiler_BBOB_suite.hpp>
#include <IOHprofiler_PBO_suite.hpp>
#include <IOHprofiler_csv_logger.h>
#include <iomanip>

std::string suite_name;
int dimension;

template <class InputType> void Initialization(int dimension, std::vector<InputType> &x) {
  x.clear();
  for(int i = 0; i != dimension; ++i){
    if(suite_name == "pbo") {
      x.push_back(rand() % 2);
    }
    else if (suite_name == "bbob") {
      x.push_back(rand() / double(RAND_MAX) * 10.0 - 5.0);
    }
    else {
      std::cerr << "unexist suite" << std::endl;
    }
  }
};

void outputvector(const std::vector<int> &x) {
  for (int i = 0; i != x.size(); ++i) {
    std::cout << x[i];
  }
}

void outputvector(const std::vector<double> &x) {
  for (int i = 0; i != x.size() - 1; ++i) {
    std::cout << x[i] << ',';
  }
  std::cout << x[x.size() - 1];
}

template <class InputType> void randomTest(std::shared_ptr<IOHprofiler_problem<InputType> > problem) {
  std::vector<InputType> x;

  double y;
  std::cout << std::setprecision(10);
  int n = problem->IOHprofiler_get_number_of_variables();
  x = std::vector<InputType> (n,0);
  y = problem->evaluate(x);
  std::cout << problem->IOHprofiler_get_problem_id() << ' ' << problem->IOHprofiler_get_instance_id() << ' ';
  outputvector(x);
  std::cout << ' ' << y << std::endl;

  Initialization(n,x);
  y = problem->evaluate(x);
  std::cout << problem->IOHprofiler_get_problem_id() << ' ' << problem->IOHprofiler_get_instance_id() << ' ';
  outputvector(x);
  std::cout << ' ' << y << std::endl;

  Initialization(n,x);
  y = problem->evaluate(x);
  std::cout << problem->IOHprofiler_get_problem_id() << ' ' << problem->IOHprofiler_get_instance_id() << ' ';
  outputvector(x);
  std::cout << ' ' << y << std::endl;
  
  Initialization(n,x);
  y = problem->evaluate(x);
  std::cout << problem->IOHprofiler_get_problem_id() << ' ' << problem->IOHprofiler_get_instance_id() << ' ';
  outputvector(x);
  std::cout << ' ' << y << std::endl;
  
  Initialization(n,x);
  y = problem->evaluate(x);
  std::cout << problem->IOHprofiler_get_problem_id() << ' ' << problem->IOHprofiler_get_instance_id() << ' ';
  outputvector(x);
  std::cout << ' ' << y << std::endl;
  
  Initialization(n,x);
  y = problem->evaluate(x);
  std::cout << problem->IOHprofiler_get_problem_id() << ' ' << problem->IOHprofiler_get_instance_id() << ' ';
  outputvector(x);
  std::cout << ' ' << y << std::endl;
}

void _run_suite() {

  std::vector<int> instance_id = {1,2,3,4,5};
  std::vector<int> dimensions;
  dimensions.push_back(dimension);
  /// Allocate the suite to be tested
  if (suite_name == "pbo") {
    PBO_suite suite;
    suite.IOHprofiler_set_suite_instance_id(instance_id);
    suite.IOHprofiler_set_suite_dimension(dimensions);
    std::shared_ptr<IOHprofiler_problem<int> > problem;

    /// Problems arr tested one by one until 'get_next_problem' returns NULL.
    while ((problem = suite.get_next_problem()) != nullptr) {
      randomTest(problem);
    }
  }
  else if (suite_name == "bbob"){
    BBOB_suite suite;
    suite.IOHprofiler_set_suite_instance_id(instance_id);
    suite.IOHprofiler_set_suite_dimension(dimensions);
    
    std::shared_ptr<IOHprofiler_problem<double> > problem;

    /// Problems arr tested one by one until 'get_next_problem' returns NULL.
    while ((problem = suite.get_next_problem()) != nullptr) {
      randomTest(problem);
    }
  } else {
    std::cerr << "unexist suite" << std::endl;
  }
  
}


int main(int argc, char** argv){
  if(argc < 3) {
    std::cerr << 'incorrect input' << std::endl;
  }
  suite_name = argv[1];
  dimension = std::stoi(argv[2]);

  _run_suite();
  return 0;
}

