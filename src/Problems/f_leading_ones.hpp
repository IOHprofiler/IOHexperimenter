#ifndef _F_LEADING_ONES_H
#define _F_LEADING_ONES_H


#include "../IOHprofiler_problem.hpp"

class LeadingOnes : public IOHprofiler_problem<int> {
public:
  LeadingOnes() {
    problem_id = 2;
    instance_id = 1;
    problem_name = "Leading_Ones";
    problem_type = "pseudo_Boolean_problem";

    number_of_objectives = 1;
  }
  //~LeadingOnes();
  LeadingOnes(int instance_id, int dimension) {
    this->problem_id = 2;
    this->instance_id = instance_id;
    this->problem_name = "LeadingOnes";
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
      if(x[i] == 1)
        result = i + 1;
      else
        break;
    }
    y.push_back(result);
  };
};

#endif