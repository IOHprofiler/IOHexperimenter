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
  /* OMS: Why are these c'tors empty? Also, they could be merged using default values. */
  IOHprofiler_problem() {};
  IOHprofiler_problem(int instance_id, int dimension){};
  virtual ~IOHprofiler_problem(){};

  IOHprofiler_problem(const IOHprofiler_problem&) = delete;
  IOHprofiler_problem &operator=(const IOHprofiler_problem&) = delete;
  
  /// TODO
  /// \fn virtual std::vector<double> internal_evaluate_multi(std::vector<InputType> x)
  /// \brief A virtual internal evaluate function.
  ///
  /// The internal_evaluate function is to be used in evaluate function.
  /// This function must be decalred in derived function of new problems.
  virtual std::vector<double> internal_evaluate_multi(const std::vector<InputType> &x) { /* OMS: accessor - should become const */ 
    std::vector<double> result;
    printf("No multi evaluate function defined\n"); /* OMS: Usage of printf is inappropriate - please switch to cout (ostream) throughout the code*/
    return result;
  };

  /// \fn double internal_evaluate(std::vector<InputType> x)
  /// \brief A virtual internal evaluate function.
  ///
  /// The internal_evaluate function is to be used in evaluate function.
  /// This function must be decalred in derived function of new problems.
  virtual double internal_evaluate(const std::vector<InputType> &x) { /* OMS: accessor - should become const */
    double result = -DBL_MAX;
    printf("No evaluate function defined\n");
    return result;
  };

  virtual void prepare_problem() { /* OMS: why empty ? */

  }

  /// TODO multi-objectives optimization
  /// \fn std::vector<double> evevaluate_multialuate(std::vector<InputType> x)
  /// \brife A common function for evaluating fitness of problems.
  ///
  /// Raw evaluate process, tranformation operations, and logging process are excuted 
  /// in this function.
  /// \param x A InputType vector of variables.
  /// \return A double vector of objectives.
  std::vector<double> evaluate_multi(std::vector<InputType> x) { /* OMS: the argument should be passed by-ref (const) */
    ++this->evaluations;

    transformation.variables_transformation(x,this->problem_id,this->instance_id,this->problem_type);
    this->raw_objectives = internal_evaluate_multi(x);
    
    this->transformed_objectives = this->raw_objectives;
    transformation.objectives_transformation(x,this->transformed_objectives,this->problem_id,this->instance_id,this->problem_type);
    if (compareObjectives(this->transformed_objectives,this->best_so_far_transformed_objectives,this->maximization_minimization_flag)) {
      this->best_so_far_transformed_objectives = this->transformed_objectives;
      this->best_so_far_transformed_evaluations = this->evaluations;
      this->best_so_far_raw_objectives = this->raw_objectives;
      this->best_so_far_raw_evaluations = this->evaluations;
    
    }

    if (compareVector(this->transformed_objectives,this->optimal)) {
      this->optimalFound = true;
    }

    return this->transformed_objectives;
  };

  /// \fn double evaluate(std::vector<InputType> x)
  /// \brife A common function for evaluating fitness of problems.
  ///
  /// Raw evaluate process, tranformation operations, and logging process are excuted 
  /// in this function.
  /// \param x A InputType vector of variables.
  /// \return A double vector of objectives.
  double evaluate(std::vector<InputType> x) { /* OMS: the argument should be passed by-ref (const) */ 
    ++this->evaluations;

    transformation.variables_transformation(x,this->problem_id,this->instance_id,this->problem_type);
    this->raw_objectives[0] = internal_evaluate(x);
    /// todo. make it as vector assign.
    
    this->transformed_objectives[0] = this->raw_objectives[0];

    transformation.objectives_transformation(x,this->transformed_objectives,this->problem_id,this->instance_id,this->problem_type);
    if (compareObjectives(this->transformed_objectives,this->best_so_far_transformed_objectives,this->maximization_minimization_flag)) {
      this->best_so_far_transformed_objectives = this->transformed_objectives;
      this->best_so_far_transformed_evaluations = this->evaluations;
      this->best_so_far_raw_objectives = this->raw_objectives;
      this->best_so_far_raw_evaluations = this->evaluations;
      /// todo. add a function for this.
    }

    if (compareVector(this->transformed_objectives,this->optimal)) {
      this->optimalFound = true;
    }

    return this->transformed_objectives[0];
  };
  
  /// TODO multi-objectives optimization
  /// \fn void evaluate_multi(std::vector<InputType> x, std::vector<double> &y)
  /// \brife A common function for evaluating fitness of problems.
  ///
  /// Raw evaluate process, tranformation operations, and logging process are excuted 
  /// in this function.
  /// \param x A InputType vector of variables.
  /// \param y A double vector of objectives.
  void evaluate_multi(std::vector<InputType> x, std::vector<double> &y) { /* OMS: the first argument should be passed by-ref (const) */
    ++this->evaluations;

    transformation.variables_transformation(x,this->problem_id,this->instance_id,this->problem_type);
    y = internal_evaluate_multi(x);
    
    this->raw_objectives = y;
    if (compareObjectives(y,this->best_so_far_raw_objectives,this->maximization_minimization_flag)) {
      this->best_so_far_raw_objectives = y;
      this->best_so_far_raw_evaluations = this->evaluations;
    }
    
    transformation.objectives_transformation(x,y,this->problem_id,this->instance_id,this->problem_type);
    if (compareObjectives(y,this->best_so_far_transformed_objectives,this->maximization_minimization_flag)) {
      this->best_so_far_transformed_objectives = y;
      this->best_so_far_transformed_evaluations = this->evaluations;
    }
    
    this->transformed_objectives = y;
    if (compareVector(y,this->optimal)) {
      this->optimalFound = true;
    }
  };

  /// \fn void evaluate(std::vector<InputType> x, std::vector<double> &y)
  /// \brife A common function for evaluating fitness of problems.
  ///
  /// Raw evaluate process, tranformation operations, and logging process are excuted 
  /// in this function.
  /// \param x A InputType vector of variables.
  /// \param y A double vector of objectives.
  void evaluate(std::vector<InputType> x, double &y) {/* OMS: the first argument should be passed by-ref (const) */
    ++this->evaluations;

    transformation.variables_transformation(x,this->problem_id,this->instance_id,this->problem_type);
    y = internal_evaluate(x);
    
    this->raw_objectives = y;
    

    transformation.objectives_transformation(x,y,this->problem_id,this->instance_id,this->problem_type);
    if (y > this->best_so_far_transformed_objectives[0]) {
      this->best_so_far_transformed_objectives[0] = y;
      this->best_so_far_transformed_evaluations = this->evaluations;
      this->best_so_far_raw_objectives[0] = y;
      this->best_so_far_raw_evaluations = this->evaluations;
    }
    
    this->transformed_objectives = y;
    if (y == this->optimal[0]) {
      this->optimalFound = true;
    }
  };

  /// \fn void calc_optimal()
  ///
  /// A function to calculate optimal of the problem.
  /// It will be revoked after setting dimension (number_of_variables) or instance_id.
  void calc_optimal() {
    if (this->best_variables.size() == this->number_of_variables) {
      /// todo. Make Exception.
      /// Do not apply transformation on best_variables as calculating optimal
      if (this->number_of_objectives == 1) {
        this->optimal[0] = internal_evaluate(this->best_variables);
      } else {
        this->optimal = internal_evaluate_multi(this->best_variables);
      }
      transformation.objectives_transformation(this->best_variables,this->optimal,this->problem_id,this->instance_id,this->problem_type);
    }
    else {
      this->optimal.clear();
      for (int i = 0; i < this->number_of_objectives; ++i) {
        if (this->maximization_minimization_flag == 1) {
          this->optimal.push_back(DBL_MAX); 
        } else {
          this->optimal.push_back(-DBL_MAX);
        }
      }
    }
  };
/* OMS: the following function overloading is confusing - consider renaming one of them */
  /// \todo  To support constrained optimization.
  virtual std::vector<double> constraints() { /* OMS: accessor ==> const */
    std::vector<double> con;
    printf("No constraints function defined\n");
    return con;
  };
  virtual void constraints(std::vector<InputType> x, std::vector<double> c) { /* OMS: arguments should be passed by-ref (const) */
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
      if (this->maximization_minimization_flag == 1) {
        this->best_so_far_raw_objectives[i] = -DBL_MAX;
        this->best_so_far_transformed_objectives[i] = -DBL_MAX;
      } else {
        this->best_so_far_raw_objectives[i] = DBL_MAX;
        this->best_so_far_transformed_objectives[i] = DBL_MAX;
      }
    }
    this->prepare_problem();
  };

  /// \fn std::vector<std::variant<int,double,std::string>> loggerInfo()
  ///
  /// Return a vector logger_info may be used by loggers.
  /// logger_info[0] evaluations
  /// logger_info[1] precision
  /// logger_info[2] best_so_far_precision
  /// logger_info[3] transformed_objective
  /// logger_info[4] best_so_far_transformed_objectives
  std::vector<double> loggerCOCOInfo() { /* OMS: accessor ==> const */
    std::vector<double> logger_info(5);
    logger_info[0] = (double)this->evaluations;
    logger_info[1] = this->transformed_objectives[0] - this->optimal[0];
    logger_info[2] = this->best_so_far_transformed_objectives[0] - this->optimal[0];
    logger_info[3] = this->transformed_objectives[0];
    logger_info[4] = this->best_so_far_transformed_objectives[0];

    return logger_info;
  }
  
  /// \fn std::vector<std::variant<int,double,std::string>> loggerInfo()
  ///
  /// Return a vector logger_info may be used by loggers.
  /// logger_info[0] evaluations
  /// logger_info[1] raw_objectives
  /// logger_info[2] best_so_far_raw_objectives
  /// logger_info[3] transformed_objective
  /// logger_info[4] best_so_far_transformed_objectives
  std::vector<double> loggerInfo() { /* OMS: accessor ==> const */
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
  bool IOHprofiler_hit_optimal() { /* OMS: accessor ==> const */
    return this->optimalFound;
  }

  int IOHprofiler_get_problem_id() { /* OMS: accessor ==> const */
    return this->problem_id;
  };

  void IOHprofiler_set_problem_id(int problem_id) {
    this->problem_id = problem_id;
  };
  
  int IOHprofiler_get_instance_id() { /* OMS: accessor ==> const */
    return this->instance_id;
  };
  
  /// \fn IOHprofiler_set_instance_id(int instance_id)
  ///
  /// To set instance_id of the problem. Since the optimal will be updated
  /// as instanced_id updated, calc_optimal() is revoked here.
  /// \para instance_id 
  void IOHprofiler_set_instance_id(int instance_id) {
    this->instance_id = instance_id;
    this->prepare_problem();
    this->calc_optimal();
  };

  std::string IOHprofiler_get_problem_name() { /* OMS: accessor ==> const */
    return this->problem_name;
  };

  void IOHprofiler_set_problem_name(std::string problem_name) {
    this->problem_name = problem_name;
  };

  std::string IOHprofiler_get_problem_type() { /* OMS: accessor ==> const */
    return this->problem_type;
  };

  void IOHprofiler_set_problem_type(std::string problem_type) {
    this->problem_type = problem_type;
  };

  std::vector<InputType> IOHprofiler_get_lowerbound() { /* OMS: accessor ==> const */
    return this->lowerbound;
  };
  
  void IOHprofiler_set_lowerbound(int lowerbound) {
    std::vector<InputType>().swap(this->lowerbound);
    this->lowerbound.reserve(this->number_of_variables);
    for (int i = 0; i < this->number_of_variables; ++i) {
      this->lowerbound.push_back(lowerbound);
    }
  };

  void IOHprofiler_set_lowerbound(const std::vector<InputType> &lowerbound) {
    this->lowerbound = lowerbound;
  };

  std::vector<InputType> IOHprofiler_get_upperbound() { /* OMS: accessor ==> const */
    return this->upperbound;
  };

  void IOHprofiler_set_upperbound(int upperbound) {
    std::vector<InputType>().swap(this->upperbound);
    this->upperbound.reserve(this->number_of_variables);
    for (int i = 0; i < this->number_of_variables; ++i) {
      this->upperbound.push_back(upperbound);
    }
  };

  void IOHprofiler_set_upperbound(const std::vector<InputType> &upperbound) {
    this->upperbound = upperbound;
  };

  std::vector<int> IOHprofiler_get_evaluate_int_info() { /* OMS: accessor ==> const */
    return this->evaluate_int_info;
  };

  void IOHprofiler_set_evaluate_int_info(const std::vector<int> &evaluate_int_info) {
    this->evaluate_int_info = evaluate_int_info;
  };

  //std::vector<double> IOHprofiler_get_evaluate_double_info() {
  //  return this->evaluate_double_info;
  //};

  //void IOHprofiler_set_evaluate_double_info(const std::vector<double> &evaluate_double_info) {
  //  this->evaluate_double_info = evaluate_double_info;
  //};
 
  int IOHprofiler_get_number_of_variables() { /* OMS: accessor ==> const */
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
    this->prepare_problem();
    this->calc_optimal();
  };

  /// \fn IOHprofiler_set_number_of_variables(int number_of_variables)
  /// 
  /// To set number_of_variables of the problem. When the number_of_variables is updated,
  /// best_variables, lowerbound, upperbound, and optimal need to be updated as well. In case 
  /// the best value for each bit is not staic, another input 'best_variables' is supplied.
  ///
  /// \para number_of_variables, best_variables
  void IOHprofiler_set_number_of_variables(int number_of_variables, const std::vector<InputType> &best_variables) {
    this->number_of_variables = number_of_variables;
    this->best_variables = best_variables;
    if (this->lowerbound.size() != 0) {
      this->IOHprofiler_set_lowerbound(this->lowerbound[0]);
    }
    if (this->upperbound.size() != 0) {
      this->IOHprofiler_set_upperbound(this->upperbound[0]);
    }
    this->prepare_problem();
    this->calc_optimal();
  };

  int IOHprofiler_get_number_of_objectives() { /* OMS: accessor ==> const */
    return this->number_of_objectives;
  };

  void IOHprofiler_set_number_of_objectives(int number_of_objectives) {
    this->number_of_objectives = number_of_objectives;
    this->raw_objectives = std::vector<double>(this->number_of_objectives);
    this->transformed_objectives = std::vector<double>(this->number_of_objectives);
    this->best_so_far_raw_objectives = std::vector<double>(this->number_of_objectives,-DBL_MAX);
    this->best_so_far_transformed_objectives = std::vector<double>(this->number_of_objectives,-DBL_MAX);

    this->optimal = std::vector<double>(this->number_of_objectives);
  };

  std::vector<double> IOHprofiler_get_raw_objectives() { /* OMS: accessor ==> const */
    return this->raw_objectives;
  };

  std::vector<double> IOHprofiler_get_transformed_objectives() { /* OMS: accessor ==> const */
    return this->transformed_objectives;
  };

  int IOHprofiler_get_transformed_number_of_variables() { /* OMS: accessor ==> const */
    return this->transformed_number_of_variables;
  };

  std::vector<InputType> IOHprofiler_get_transformed_variables() { /* OMS: accessor ==> const */
    return this->transformed_variables;
  };

  std::vector<InputType> IOHprofiler_get_best_variables() { /* OMS: accessor ==> const */
    return this->best_variables;
  };

  void IOHprofiler_set_best_variables(InputType best_variables) { /* OMS: argument should be passed by-ref (const) */
    this->best_variables.clear();
    for (int i = 0; i < this->number_of_variables; ++i) {
      this->best_variables.push_back(best_variables);
    }
  };

  void IOHprofiler_set_best_variables(std::vector<InputType> best_variables) { /* OMS: argument should be passed by-ref (const) */
    this->best_variables = best_variables;
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
    this->optimal = optimal;
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

  int IOHprofiler_get_optimization_type() {
    return this->maximization_minimization_flag;
  }

  void IOHprofiler_set_as_maximization() {
    this->maximization_minimization_flag = 1;
  }

  void IOHprofiler_set_as_minimization() {
    this->maximization_minimization_flag = 0;
  }

private:
  /* OMS: All the following assignments are misplaced - should appear in the c'tors */
  int problem_id = DEFAULT_PROBLEM_ID; /// < problem id, assigned as being added into a suite.
  int instance_id = DEFAULT_INSTANCE; /// < evaluate function is validated with instance and dimension. set default to avoid invalid class.
  
  std::string problem_name;
  std::string problem_type;   /// todo. make it as enum.

  int maximization_minimization_flag = 1; /// < set as maximization if flag = 1, otherwise minimization.
  
  std::vector<InputType> lowerbound;
  std::vector<InputType> upperbound;
  
  ///std::vector<int> evaluate_int_info; /// < common used info for evaluating variables, integer type.
  ///std::vector<double> evaluate_double_info; /// < common used info for evaluating variables, double type.

  std::size_t number_of_variables = DEFAULT_DIMENSION; /// < evaluate function is validated with instance and dimension. set default to avoid invalid class.
  std::size_t number_of_objectives;

  std::vector<InputType> best_variables; /// todo. comments, rename?
  std::vector<InputType> best_transformed_variables;
  std::vector<double> optimal; /// todo. How to evluate distance to optima. In global optima case, which optimum to be recorded.
  bool optimalFound = false;

  std::vector<double> raw_objectives; /// < to record objectives before transformation.
  std::vector<double> transformed_objectives; /// < to record objectives after transformation.
  int transformed_number_of_variables; /// < intermediate variables in evaluate.
  /// todo.  check.
  std::vector<InputType> transformed_variables; /// < intermediate variables in evaluate.

  /// todo. constrainted optimization.
  std::size_t number_of_constraints;

  std::size_t evaluations = 0; /// < to record optimization process. 
  /// todo. rename number_of_evaluations.
  std::vector<double> best_so_far_raw_objectives; /// < to record optimization process.
  int best_so_far_raw_evaluations = 0; /// < to record optimization process.
  std::vector<double> best_so_far_transformed_objectives; /// < to record optimization process.
  int best_so_far_transformed_evaluations; /// < to record optimization process.
};

#endif //_IOHPROFILER_PROBLEM_HPP
