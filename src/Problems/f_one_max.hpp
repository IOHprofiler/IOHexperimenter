#ifndef _F_ONE_MAX_H
#define _F_ONE_MAX_H

#include "../IOHprofiler_problem.hpp"

class OneMax : public IOHprofiler_problem<int> {
public:
  OneMax() {
    problem_id = 1;
    instance_id = 1;
    problem_name = "OneMax";
    problem_type = "pseudo_Boolean_problem";

    number_of_objectives = 1;
  }
  //~OneMax();
  
  OneMax(int instance_id, int dimension) {
    this->problem_id = 1;
    this->instance_id = instance_id;
    this->problem_name = "OneMax";
    this->problem_type = "pseudo_Boolean_problem";
    number_of_objectives = 1;

    Initilize_problem(dimension);
  }

  void Initilize_problem(int  dimension) {
    number_of_variables = dimension;
    lowerbound.reserve(dimension);
    upperbound.reserve(dimension);
    best_variables.reserve(dimension);
    optimal.reserve(number_of_objectives);

    for(int i = 0; i != dimension; ++i) {
      lowerbound[i] = 0;
      upperbound[i] = 1;
      best_variables[i] = 1;
    }
    optimal.push_back((double)dimension);
  };

  void internal_evaluate(std::vector<int> x,std::vector<double> &y) {
    y.clear();
    y.reserve(number_of_objectives);
    int n = x.size();
    int result = 0;
    for(int i = 0; i != n; ++i) {
      result += x[i];
    }
    y.push_back(result);
  };
};

#endif