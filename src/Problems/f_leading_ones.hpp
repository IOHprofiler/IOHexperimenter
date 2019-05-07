#ifndef _F_LEADING_ONES_H
#define _F_LEADING_ONES_H


#include "../IOHprofiler_problem.hpp"

class LeadingOnes : public IOHprofiler_problem<int> {
public:
  LeadingOnes() {
    problem_id = 1;
    instance_id = 1;
    problem_name = "Leading_Ones";
    problem_type = "pseudo_Boolean_problem";

    number_of_objectives = 1;
  }
  //~LeadingOnes();
  LeadingOnes(int instance_id, int dimension) {
    this->problem_id = 1;
    this->instance_id = instance_id;
    this->problem_name = "LeadingOnes";
    this->problem_type = "pseudo_Boolean_problem";
    number_of_objectives = 1;

    Initilize_problem(dimension);
  }

  LeadingOnes( const LeadingOnes &lo) {
    this->problem_id = lo.problem_id;
    this->instance_id = lo.instance_id;
    this->problem_name = lo.problem_name;
    this->problem_type = lo.problem_type;
    this->number_of_variables = lo.number_of_variables;
    this->number_of_objectives = lo.number_of_objectives;

    this->evaluations = 0;
    copyVector(lo.lowerbound,this->lowerbound);
    copyVector(lo.upperbound,this->upperbound);
    copyVector(lo.best_variables,this->best_variables);
    copyVector(lo.optimal,this->optimal);  
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