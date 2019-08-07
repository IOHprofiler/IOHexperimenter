/// \file IOHprofiler_problem.hpp
/// \brief Head file for class IOHprofiler_problem.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _IOHPROFILER_PROBLEM_HPP
#define _IOHPROFILER_PROBLEM_HPP

#include "IOHprofiler_common.h"
#include "IOHprofiler_transformation.hpp"

/// < transformation methods. 
static IOHprofiler_transformation transformation;

/// \brief A base class for defining problems.
///
/// Basic structure for IOHExperimentor, which is used for generating benchmark problems.
/// To define a new problem, values of 'number_of_variables', 'lowerbound', 'upperbound',
/// 'best_variables', 'optimal' must be given. An corresponding 'internal_evaluate' method
/// must be defined as well.
template <class InputType> class IOHprofiler_problem 
{
public:
  
  IOHprofiler_problem() {};
  IOHprofiler_problem(int instance_id, int dimension){};
  ~IOHprofiler_problem(){};

  IOHprofiler_problem(const IOHprofiler_problem&) = delete;
  IOHprofiler_problem &operator=(const IOHprofiler_problem&) = delete;
  
  /// \fn virtual std::vector<double> internal_evaluate(std::vector<InputType> x)
  /// \brief A virtual internal evaluate function.
  ///
  /// The internal_evaluate function is to be used in evaluate function.
  /// This function must be decalred in derived function of new problems.
  virtual std::vector<double> internal_evaluate(std::vector<InputType> x) {
    std::vector<double> result;
    printf("No evaluate function defined\n");
    return result;
  };

  virtual void update_evaluate_double_info() {
  };

  virtual void update_evaluate_int_info() {
  };

  /// \fn std::vector<double> evaluate(std::vector<InputType> x)
  /// \brife A common function for evaluating fitness of problems.
  ///
  /// Raw evaluate process, tranformation operations, and logging process are excuted 
  /// in this function.
  /// \param x A InputType vector of variables.
  /// \return A double vector of objectives.
  std::vector<double> evaluate(std::vector<InputType> x) {
    std::vector<double> y;  
    ++this->evaluations;

    variables_transformation(x);
    y = internal_evaluate(x);
    

    copyVector(y,this->raw_objectives);
    if (compareObjectives(y,this->best_so_far_raw_objectives)) {
      copyVector(y,this->best_so_far_raw_objectives);
      this->best_so_far_raw_evaluations = this->evaluations;
    }
    
    objectives_transformation(y);
    if (compareObjectives(y,this->best_so_far_transformed_objectives)) {
      copyVector(y,this->best_so_far_transformed_objectives);
      this->best_so_far_transformed_evaluations = this->evaluations;
    }

    if (compareVector(y,this->optimal)) {
      this->optimalFound = true;
    }

    copyVector(y, this->transformed_objectives);
    return y;
  };
  

  /// \fn void evaluate(std::vector<InputType> x, std::vector<double> &y)
  /// \brife A common function for evaluating fitness of problems.
  ///
  /// Raw evaluate process, tranformation operations, and logging process are excuted 
  /// in this function.
  /// \param x A InputType vector of variables.
  /// \param y A double vector of objectives.
  void evaluate(std::vector<InputType> x, std::vector<double> &y) {
    ++this->evaluations;

    variables_transformation(x);
    y = internal_evaluate(x);
    
    copyVector(y,this->raw_objectives);
    if (compareObjectives(y,this->best_so_far_raw_objectives)) {
      copyVector(y,this->best_so_far_raw_objectives);
      this->best_so_far_raw_evaluations = this->evaluations;
    }
    
    objectives_transformation(y);
    if (compareObjectives(y,this->best_so_far_transformed_objectives)) {
      copyVector(y,this->best_so_far_transformed_objectives);
      this->best_so_far_transformed_evaluations = this->evaluations;
    }
    
    copyVector(y, this->transformed_objectives);
    if (compareVector(y,this->optimal)) {
      this->optimalFound = true;
    }
  };

  /// \fn void calc_optimal()
  ///
  /// A function to calculate optimal of the problem.
  /// It will be revoked after setting dimension (number_of_variables) or instance_id.
  void calc_optimal() {
    if (this->best_variables.size() == this->number_of_variables) {
      /// Do not apply transformation on best_variables as calculating optimal
      this->optimal = internal_evaluate(this->best_variables);
      objectives_transformation(this->optimal);
    }
    else {
      this->optimal.clear();
      for (int i = 0; i < this->number_of_objectives; ++i) {
        this->optimal.push_back(DBL_MAX);
      }
    }
  };

  /// \fn void variables_transformation(std::vector<InputType> &x)
  /// \brief Transformation operations on variables.
  ///
  /// For instance_id in ]1,50], xor operation is applied.
  /// For instance_id in ]50,100], \sigma function is applied.
  void variables_transformation(std::vector<InputType> &x) { 
    if (instance_id > 1 && instance_id <= 50) { 
      transformation.transform_vars_xor(x,this->instance_id);
    } else if (instance_id > 50 && instance_id <= 100) {
      transformation.transform_vars_sigma(x,this->instance_id);
    }
  };

  /// \fn void objectives_transformation(std::vector<double> &y)
  /// \brief Transformation operations on objectives (a * f(x) + b).
  void objectives_transformation(std::vector<double> &y) {
    if (instance_id > 1) {
      transformation.transform_obj_scale(y,this->instance_id);
      transformation.transform_obj_shift(y,this->instance_id);
    }
  };

  /// \todo  To support constrained optimization.
  virtual std::vector<double> constraints() {
    std::vector<double> con;
    printf("No constraints function defined\n");
    return con;
  };
  virtual void constraints(std::vector<InputType> x, std::vector<double> c) {
    printf("No constraints function defined\n");
  };
  
  /// \fn void reset_problem()
  ///
  /// \brief Reset problem as the default condition before doing evaluating.
  void reset_problem() {
    this->evaluations = 0;
    this->best_so_far_raw_evaluations = 0;
    this->best_so_far_transformed_evaluations = 0;
    this->optimalFound = false;
    for (int i = 0; i !=  this->number_of_objectives; ++i) {
      this->best_so_far_raw_objectives[i] = DBL_MIN_EXP;
      this->best_so_far_transformed_objectives[i] = DBL_MIN_EXP;
    }
  };

  /// \fn std::vector<std::variant<int,double,std::string>> loggerInfo()
  ///
  /// Return a vector logger_info may be used by loggers.
  /// logger_info[0] evaluations
  /// logger_info[1] raw_objectives
  /// logger_info[2] best_so_far_raw_objectives
  /// logger_info[3] transformed_objective
  /// logger_info[4] best_so_far_transformed_objectives
  std::vector<double> loggerInfo() {
    std::vector<double> logger_info(5);
    logger_info[0] = (double)this->evaluations;
    logger_info[1] = this->raw_objectives[0];
    logger_info[2] = this->best_so_far_raw_objectives[0];
    logger_info[3] = this->transformed_objectives[0];
    logger_info[4] = this->best_so_far_transformed_objectives[0];
    return logger_info;
  }
  
  /// \fn IOHprofiler_hit_optimal()
  ///
  /// \brief Detect if the optimal have been found.
  bool IOHprofiler_hit_optimal() {
    return this->optimalFound;
  }

  int IOHprofiler_get_problem_id() {
    return this->problem_id;
  };

  void IOHprofiler_set_problem_id(int problem_id) {
    this->problem_id = problem_id;
  };
  
  int IOHprofiler_get_instance_id() {
    return this->instance_id;
  };
  
  /// \fn IOHprofiler_set_instance_id(int instance_id)
  ///
  /// To set instance_id of the problem. Since the optimal will be updated
  /// as instanced_id updated, calc_optimal() is revoked here.
  /// \para instance_id 
  void IOHprofiler_set_instance_id(int instance_id) {
    this->instance_id = instance_id;
    this->update_evaluate_double_info();
    this->update_evaluate_int_info();
    this->calc_optimal();
  };

  std::string IOHprofiler_get_problem_name() {
    return this->problem_name;
  };

  void IOHprofiler_set_problem_name(std::string problem_name) {
    this->problem_name = problem_name;
  };

  std::string IOHprofiler_get_problem_type() {
    return this->problem_type;
  };

  void IOHprofiler_set_problem_type(std::string problem_type) {
    this->problem_type = problem_type;
  };

  std::vector<InputType> IOHprofiler_get_lowerbound() {
    return this->lowerbound;
  };
  
  void IOHprofiler_set_lowerbound(int lowerbound) {
    std::vector<InputType>().swap(this->lowerbound);
    this->lowerbound.reserve(this->number_of_variables);
    for (int i = 0; i < this->number_of_variables; ++i) {
      this->lowerbound.push_back(lowerbound);
    }
  };

  void IOHprofiler_set_lowerbound(std::vector<InputType> lowerbound) {
    copyVector(lowerbound,this->lowerbound);
  };

  std::vector<InputType> IOHprofiler_get_upperbound() {
    return this->upperbound;
  };

  void IOHprofiler_set_upperbound(int upperbound) {
    std::vector<InputType>().swap(this->upperbound);
    this->upperbound.reserve(this->number_of_variables);
    for (int i = 0; i < this->number_of_variables; ++i) {
      this->upperbound.push_back(upperbound);
    }
  };

  void IOHprofiler_set_upperbound(std::vector<InputType> upperbound) {
    copyVector(upperbound,this->upperbound);
  };

  std::vector<int> IOHprofiler_get_evaluate_int_info() {
    return this->evaluate_int_info;
  };

  void IOHprofiler_set_evaluate_int_info(std::vector<int> evaluate_int_info) {
    copyVector(evaluate_int_info, this->evaluate_int_info);
  };

  std::vector<double> IOHprofiler_get_evaluate_double_info() {
    return this->evaluate_double_info;
  };

  void IOHprofiler_set_evaluate_double_info(std::vector<double> evaluate_double_info) {
    copyVector(evaluate_double_info, this->evaluate_double_info);
  };
 
  int IOHprofiler_get_number_of_variables() {
    return this->number_of_variables;
  };

  /// \fn IOHprofiler_set_number_of_variables(int number_of_variables)
  /// 
  /// To set number_of_variables of the problem. When the number_of_variables is updated,
  /// best_variables, lowerbound, upperbound, and optimal need to be updated as well.
  ///
  /// \para number_of_variables
  void IOHprofiler_set_number_of_variables(int number_of_variables) {
    this->number_of_variables = number_of_variables;
    if (this->best_variables.size() != 0) {
      this->IOHprofiler_set_best_variables(this->best_variables[0]);
    }
    if (this->lowerbound.size() != 0) {
      this->IOHprofiler_set_lowerbound(this->lowerbound[0]);
    }
    if (this->upperbound.size() != 0) {
      this->IOHprofiler_set_upperbound(this->upperbound[0]);
    }
    this->update_evaluate_double_info();
    this->update_evaluate_int_info();
    this->calc_optimal();
  };

  /// \fn IOHprofiler_set_number_of_variables(int number_of_variables)
  /// 
  /// To set number_of_variables of the problem. When the number_of_variables is updated,
  /// best_variables, lowerbound, upperbound, and optimal need to be updated as well. In case 
  /// the best value for each bit is not staic, another input 'best_variables' is supplied.
  ///
  /// \para number_of_variables, best_variables
  void IOHprofiler_set_number_of_variables(int number_of_variables, std::vector<InputType> best_variables) {
    this->number_of_variables = number_of_variables;
    copyVector(best_variables,this->best_variables);
    if (this->lowerbound.size() != 0) {
      this->IOHprofiler_set_lowerbound(this->lowerbound[0]);
    }
    if (this->upperbound.size() != 0) {
      this->IOHprofiler_set_upperbound(this->upperbound[0]);
    }
    this->update_evaluate_double_info();
    this->update_evaluate_int_info();
    this->calc_optimal();
  };

  int IOHprofiler_get_number_of_objectives() {
    return this->number_of_objectives;
  };

  void IOHprofiler_set_number_of_objectives(int number_of_objectives) {
    this->number_of_objectives = number_of_objectives;
    this->raw_objectives.reserve(this->number_of_objectives);
    this->transformed_objectives.reserve(this->number_of_objectives);
    this->best_so_far_raw_objectives.reserve(this->number_of_objectives);
    this->best_so_far_transformed_objectives.reserve(this->number_of_objectives);
    for (int i = 0; i !=  this->number_of_objectives; ++i) {
      this->best_so_far_raw_objectives.push_back(DBL_MIN_EXP);
      this->best_so_far_transformed_objectives.push_back(DBL_MIN_EXP);
    }
  };

  std::vector<double> IOHprofiler_get_raw_objectives() {
    return this->raw_objectives;
  };

  std::vector<double> IOHprofiler_get_transformed_objectives() {
    return this->transformed_objectives;
  };

  int IOHprofiler_get_transformed_number_of_variables() {
    return this->transformed_number_of_variables;
  };

  std::vector<InputType> IOHprofiler_get_transformed_variables() {
    return this->transformed_variables;
  };

  std::vector<InputType> IOHprofiler_get_best_variables() {
    return this->best_variables;
  };

  void IOHprofiler_set_best_variables(int best_variables) {
    this->best_variables.clear();
    for (int i = 0; i < this->number_of_variables; ++i) {
      this->best_variables.push_back(best_variables);
    }
  };

  void IOHprofiler_set_best_variables(std::vector<InputType> best_variables) {
    copyVector(best_variables,this->best_variables);
  };

  std::vector<double> IOHprofiler_get_optimal() {
    return this->optimal;
  };

  void IOHprofiler_set_optimal(double optimal) {
    std::vector<double>().swap(this->optimal);
    this->optimal.reserve(this->number_of_objectives);
    for (int i = 0; i < this->number_of_objectives; ++i) {
      this->optimal.push_back(optimal);
    }
  };

  void IOHprofiler_set_optimal(std::vector<double> optimal) {
    copyVector(optimal,this->optimal);
  };

  void IOHprofiler_evaluate_optimal(std::vector<InputType> best_variables) {
    this->optimal = this->evaluate(best_variables);
  };

  void IOHprofiler_evaluate_optimal() {
    this->optimal = this->evaluate(this->best_variables);
  };

  int IOHprofiler_get_evaluations() {
    return this->evaluations;
  };

  std::vector<double> IOHprofiler_get_best_so_far_raw_objectives() {
    return this->best_so_far_raw_objectives;
  };
  int IOHprofiler_get_best_so_far_raw_evaluations() {
    return this-> best_so_far_raw_evaluations;
  };
  std::vector<double> IOHprofiler_get_best_so_far_transformed_objectives(){
    return this->best_so_far_transformed_objectives;
  };
  int IOHprofiler_get_best_so_far_transformed_evaluations() {
    return this->best_so_far_transformed_evaluations;
  };

private:
  
  int problem_id = DEFAULT_PROBLEM_ID; /// < problem id, assigned as being added into a suite.
  int instance_id = DEFAULT_INSTANCE; /// < evaluate function is validated with instance and dimension. set default to avoid invalid class.
  
  std::string problem_name;
  std::string problem_type;   /// todo. eet it as enum.
  
  std::vector<InputType> lowerbound;
  std::vector<InputType> upperbound;
  
  std::vector<int> evaluate_int_info; /// < common used info for evaluating variables, integer type.
  std::vector<double> evaluate_double_info; /// < common used info for evaluating variables, double type.

  std::size_t number_of_variables = DEFAULT_DIMENSION; /// < evaluate function is validated with instance and dimension. set default to avoid invalid class.
  std::size_t number_of_objectives;

  std::vector<InputType> best_variables;
  std::vector<InputType> best_transformed_variables;
  std::vector<double> optimal;
  bool optimalFound = false;

  std::vector<double> raw_objectives; /// < to record objectives before transformation.
  std::vector<double> transformed_objectives; /// < to record objectives after transformation.
  int transformed_number_of_variables; /// < intermediate variables in evaluate. 
  std::vector<InputType> transformed_variables; /// < intermediate variables in evaluate.

  /// todo. constrainted optimization.
  std::size_t number_of_constraints;

  size_t evaluations = 0; /// < to record optimization process.
  std::vector<double> best_so_far_raw_objectives; /// < to record optimization process.
  int best_so_far_raw_evaluations = 0; /// < to record optimization process.
  std::vector<double> best_so_far_transformed_objectives; /// < to record optimization process.
  int best_so_far_transformed_evaluations; /// < to record optimization process.
};

#endif //_IOHPROFILER_PROBLEM_HPP