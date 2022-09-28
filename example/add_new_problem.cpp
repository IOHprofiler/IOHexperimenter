#include "ioh.hpp"

double new_problem(const std::vector<double> &x)
{
  double result = 0.0;
  for (auto i : x) {
    result += i;
  }
  return result;
}

std::vector<double> new_transform_variables_function(const std::vector<double> &x, int) {
  // The instance_id can be used as seed for possible randomizing process.
  auto tx = x;
  for (auto i : tx) {
    i -= 0.1;
  }
  return tx;
}

double new_transform_objective_functions(double y, int) {
  // The instance_id can be used as seed for possible randomizing process.
  return y + 100;
}

int main(){
  const std::vector<double> x {0.1,0.2,0.3,0.4,0.5};

  ioh::problem::wrap_function<double>(&new_problem,  // the new function
                                      "new_problem", // name of the new function
                                      ioh::common::OptimizationType::MIN, // optimization type
                                      0,  // lowerbound  
                                      1,  // upperbound
                                      &new_transform_variables_function, // the variable transformation method. Optional argument when transformation is applied.
                                      &new_transform_objective_functions // the objective transformation method. Optional argument when transformation is applied.
                                      );
  auto &factory = ioh::problem::ProblemRegistry<ioh::problem::Real>::instance();
  auto problem =  factory.create("new_problem",  // create by name
                                  1,             // instance id
                                  5              // number of search variables
                                  );
  std::cout << (*problem)(x) << std::endl;
  return 0;   
}      