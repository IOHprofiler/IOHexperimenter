/// \file IOHprofiler_problem.h
/// \brief Header file for the template class IOHprofiler_problem.
///
/// A detailed file description.
///
/// \author Furong Ye
#ifndef _IOHPROFILER_PROBLEM_H
#define _IOHPROFILER_PROBLEM_H

#include "IOHprofiler_common.h"
#include "IOHprofiler_transformation.h"

/// < transformation methods. 
static IOHprofiler_transformation transformation;

/// \brief A base class for defining problems.
///
/// Basic structure for IOHExperimentor, which is used for generating benchmark problems.
/// To define a new problem, the 'internal_evaluate' method must be defined. The problem
/// sets as maximization by default. If the 'best_variables' are given, the optimal of
/// the problem will be calculated with the 'best_variables'; or you can set the optimal
/// by defining the 'customized_optimal' function; otherwise, the optimal is set as 
/// min()(max()) for maximization(minimization). If additional calculation is needed by
/// 'internal_evaluate', you can configure it in 'prepare_problem()'.
template <class InputType> class IOHprofiler_problem 
{
public:
  IOHprofiler_problem(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) : 
    problem_id(DEFAULT_PROBLEM_ID), 
    instance_id(instance_id), 
    maximization_minimization_flag(IOH_optimization_type::Maximization),
    number_of_variables(DEFAULT_DIMENSION), 
    number_of_objectives(1),
    lowerbound(std::vector<InputType> (number_of_variables) ), 
    upperbound(std::vector<InputType> (number_of_variables) ),
    optimal(std::vector<double>(number_of_objectives) ),
    optimalFound(false),
    evaluations(0),
    best_so_far_raw_objectives(std::vector<double>(number_of_objectives) ),
    best_so_far_raw_evaluations(0),
    best_so_far_transformed_objectives(std::vector<double>(number_of_objectives) ) {}
  
  virtual ~IOHprofiler_problem() {}

  IOHprofiler_problem(const IOHprofiler_problem&) = delete;
  IOHprofiler_problem &operator=(const IOHprofiler_problem&) = delete;
  
  /// \todo to support multi-objective optimization
  /// \fn virtual std::vector<double> internal_evaluate_multi(std::vector<InputType> x)
  /// \brief A virtual internal evaluate function.
  ///
  /// The internal_evaluate function is to be used in evaluate function.
  /// This function must be decalred in derived function of new problems.
  // virtual std::vector<double> internal_evaluate_multi (const std::vector<InputType> &x) {
  //   std::vector<double> result;
  //   std::cout << "No multi evaluate function defined" << std::endl;
  //   return result;
  // };

  /// \fn double internal_evaluate(std::vector<InputType> x)
  /// \brief A virtual internal evaluate function.
  ///
  /// The internal_evaluate function is to be used in evaluate function.
  /// This function must be decalred in derived function of new problems.
  virtual double internal_evaluate(const std::vector<InputType> &x) {
    double result = std::numeric_limits<double>::lowest();
    IOH_warning("No evaluate function defined");
    return result;
  }

  virtual void prepare_problem() {
  }

  /// \todo to support multi-objective optimization
  /// \fn std::vector<double> evevaluate_multialuate(std::vector<InputType> x)
  /// \brife A common function for evaluating fitness of problems.
  ///
  /// Raw evaluate process, tranformation operations, and logging process are excuted 
  /// in this function.
  /// \param x A InputType vector of variables.
  /// \return A double vector of objectives.
  // std::vector<double> evaluate_multi(std::vector<InputType> x) {
  //   ++this->evaluations;

  //   transformation.variables_transformation(x,this->problem_id,this->instance_id,this->problem_type);
  //   this->raw_objectives = internal_evaluate_multi(x);
    
  //   this->transformed_objectives = this->raw_objectives;
  //   transformation.objectives_transformation(x,this->transformed_objectives,this->problem_id,this->instance_id,this->problem_type);
  //   if (compareObjectives(this->transformed_objectives,this->best_so_far_transformed_objectives,this->maximization_minimization_flag)) {
  //     this->best_so_far_transformed_objectives = this->transformed_objectives;
  //     this->best_so_far_transformed_evaluations = this->evaluations;
  //     this->best_so_far_raw_objectives = this->raw_objectives;
  //     this->best_so_far_raw_evaluations = this->evaluations;
    
  //   }

  //   if (compareVector(this->transformed_objectives,this->optimal)) {
  //     this->optimalFound = true;
  //   }

  //   return this->transformed_objectives;
  // }

  /// \fn double evaluate(std::vector<InputType> x)
  /// \brife A common function for evaluating fitness of problems.
  ///
  /// Raw evaluate process, tranformation operations, and logging process are excuted 
  /// in this function.
  /// \param x A InputType vector of variables.
  /// \return A double vector of objectives.
  double evaluate(std::vector<InputType> x);

  /// \fn virtual void customized_optimal()
  ///
  /// A virtual function to customize optimal of the problem.
  virtual void customize_optimal(){

  }

  /// \fn void calc_optimal()
  ///
  /// A function to calculate optimal of the problem.
  /// It will be revoked after setting dimension (number_of_variables) or instance_id.
  void calc_optimal();

  /// \todo  To support constrained optimization.
  // virtual std::vector<double> constraints() {
  //   std::vector<double> con;
  //   printf("No constraints function defined\n");
  //   return con;
  // };
  
  /// \fn void reset_problem()
  ///
  /// \brief Reset problem as the default condition before doing evaluating.
  void reset_problem();

  /// \fn std::vector<std::variant<int,double,std::string>> loggerInfo()
  ///
  /// Return a vector logger_info may be used by loggers.
  /// logger_info[0] evaluations
  /// logger_info[1] precision
  /// logger_info[2] best_so_far_precision
  /// logger_info[3] transformed_objective
  /// logger_info[4] best_so_far_transformed_objectives
  std::vector<double> loggerCOCOInfo() const;
  
  /// \fn std::vector<std::variant<int,double,std::string>> loggerInfo()
  ///
  /// Return a vector logger_info may be used by loggers.
  /// logger_info[0] evaluations
  /// logger_info[1] raw_objectives
  /// logger_info[2] best_so_far_raw_objectives
  /// logger_info[3] transformed_objective
  /// logger_info[4] best_so_far_transformed_objectives
  std::vector<double> loggerInfo() const;
  
  /// \fn IOHprofiler_hit_optimal()
  ///
  /// \brief Detect if the optimal have been found.
  bool IOHprofiler_hit_optimal() const;

  int IOHprofiler_get_problem_id() const;

  void IOHprofiler_set_problem_id(int problem_id);
  
  int IOHprofiler_get_instance_id() const;
  
  /// \fn IOHprofiler_set_instance_id(int instance_id)
  ///
  /// To set instance_id of the problem. Since the optimal will be updated
  /// as instanced_id updated, calc_optimal() is revoked here.
  /// \param instance_id 
  void IOHprofiler_set_instance_id(int instance_id);

  std::string IOHprofiler_get_problem_name() const;

  void IOHprofiler_set_problem_name(std::string problem_name);

  std::string IOHprofiler_get_problem_type() const;

  void IOHprofiler_set_problem_type(std::string problem_type);

  std::vector<InputType> IOHprofiler_get_lowerbound() const;
  
  void IOHprofiler_set_lowerbound(int lowerbound);

  void IOHprofiler_set_lowerbound(const std::vector<InputType> &lowerbound);

  std::vector<InputType> IOHprofiler_get_upperbound() const;

  void IOHprofiler_set_upperbound(int upperbound);

  void IOHprofiler_set_upperbound(const std::vector<InputType> &upperbound);
 
  int IOHprofiler_get_number_of_variables() const;

  /// \fn IOHprofiler_set_number_of_variables(int number_of_variables)
  /// 
  /// To set number_of_variables of the problem. When the number_of_variables is updated,
  /// best_variables, lowerbound, upperbound, and optimal need to be updated as well.
  ///
  /// \param number_of_variables
  void IOHprofiler_set_number_of_variables(const int number_of_variables);

  /// \fn IOHprofiler_set_number_of_variables(int number_of_variables)
  /// 
  /// To set number_of_variables of the problem. When the number_of_variables is updated,
  /// best_variables, lowerbound, upperbound, and optimal need to be updated as well. In case 
  /// the best value for each bit is not staic, another input 'best_variables' is supplied.
  ///
  /// \param number_of_variables, best_variables
  void IOHprofiler_set_number_of_variables(const int number_of_variables, const std::vector<InputType> &best_variables);

  int IOHprofiler_get_number_of_objectives() const;

  void IOHprofiler_set_number_of_objectives(const int number_of_objectives);

  std::vector<double> IOHprofiler_get_raw_objectives() const;

  std::vector<double> IOHprofiler_get_transformed_objectives() const;

  int IOHprofiler_get_transformed_number_of_variables() const;

  std::vector<InputType> IOHprofiler_get_transformed_variables() const;

  std::vector<InputType> IOHprofiler_get_best_variables() const;

  void IOHprofiler_set_best_variables(const InputType best_variables);

  void IOHprofiler_set_best_variables(const std::vector<InputType> &best_variables);

  std::vector<double> IOHprofiler_get_optimal() const;

  void IOHprofiler_set_optimal(const double optimal);

  void IOHprofiler_set_optimal(const std::vector<double> &optimal);

  void IOHprofiler_evaluate_optimal(std::vector<InputType> best_variables);

  void IOHprofiler_evaluate_optimal();

  int IOHprofiler_get_evaluations() const;

  std::vector<double> IOHprofiler_get_best_so_far_raw_objectives() const;

  int IOHprofiler_get_best_so_far_raw_evaluations() const;

  std::vector<double> IOHprofiler_get_best_so_far_transformed_objectives() const;

  int IOHprofiler_get_best_so_far_transformed_evaluations() const;

  IOH_optimization_type IOHprofiler_get_optimization_type() const;

  void IOHprofiler_set_as_maximization();

  void IOHprofiler_set_as_minimization();

private:
  int problem_id; /// < problem id, assigned as being added into a suite.
  int instance_id; /// < evaluate function is validated with instance and dimension. set default to avoid invalid class.
  
  std::string problem_name;
  std::string problem_type;   /// todo. make it as enum.

  IOH_optimization_type maximization_minimization_flag;

  std::size_t number_of_variables; /// < evaluate function is validated with instance and dimension. set default to avoid invalid class.
  std::size_t number_of_objectives;

  std::vector<InputType> lowerbound;
  std::vector<InputType> upperbound;

  std::vector<InputType> best_variables; /// todo. comments, rename?
  std::vector<InputType> best_transformed_variables;
  std::vector<double> optimal; /// todo. How to evluate distance to optima. In global optima case, which optimum to be recorded.
  bool optimalFound;

  std::vector<double> raw_objectives; /// < to record objectives before transformation.
  std::vector<double> transformed_objectives; /// < to record objectives after transformation.
  int transformed_number_of_variables; /// < intermediate variables in evaluate.
  std::vector<InputType> transformed_variables; /// < intermediate variables in evaluate.

  /// todo. constrainted optimization.
  /// std::size_t number_of_constraints;

  std::size_t evaluations; /// < to record optimization process. 
  std::vector<double> best_so_far_raw_objectives; /// < to record optimization process.
  int best_so_far_raw_evaluations; /// < to record optimization process.
  std::vector<double> best_so_far_transformed_objectives; /// < to record optimization process.
  int best_so_far_transformed_evaluations; /// < to record optimization process.
};

#include "IOHprofiler_problem.hpp"

#endif // _IOHPROFILER_PROBLEM_H
