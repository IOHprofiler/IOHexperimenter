/// \file IOHprofiler_observer.hpp
/// \brief Hpp file for the class IOHprofiler_observer.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _IOHPROFILER_OBSERVER_HPP
#define _IOHPROFILER_OBSERVER_HPP

#include "IOHprofiler_common.hpp"
#include "IOHprofiler_problem.hpp"
/// \brief A class of methods of setting triggers recording evaluations.
///
/// Four methods is introduced here:
///   1. Recording evaluations by a static interval.
///   2. Recording complete evaluations.
///   3. Recording evaluations as the best found solution is updated.
///   4. Recording evaluations at pre-defined points or/and with a static number for each exponential bucket.
class IOHprofiler_observer {
public:
  IOHprofiler_observer() :
    observer_interval(0),
    observer_complete_flag(false),
    observer_update_flag(true),
    observer_time_points({0}),
    evaluations_value1(1),
    time_points_index(0),
    time_points_expi(0),
    observer_time_points_exp_base1(10),
    evaluations_value2(1),
    evaluations_expi(0),
    observer_time_points_exp_base2(10){}
  virtual ~IOHprofiler_observer() {};

  IOHprofiler_observer(const IOHprofiler_observer &) = delete;
  IOHprofiler_observer &operator = (const IOHprofiler_observer&) = delete;
  
  void set_complete_flag(bool complete_flag);

  bool complete_status() const;

  bool complete_trigger() const;
  
  void set_interval(int interval);

  bool interval_status() const;

  bool interval_trigger(size_t evaluations) const;
  
  void set_update_flag(bool update_flag);

  bool update_status() const;

  bool update_trigger(double fitness, int optimization_type);
  
  void set_time_points(const std::vector<int> & time_points, const int number_of_evaluations, const int time_points_exp_base1 = 10, const int time_points_exp_base2 = 10);

  bool time_points_status() const;

  bool time_points_trigger(size_t evaluations);

  void reset_observer(const int optimization_type);

  virtual void do_log() {}

  virtual void track_problem(IOHprofiler_problem<int> & problem) {}
  
  virtual void track_problem(IOHprofiler_problem<double> & problem) {}

  // Updated here.
  virtual void track_problem(std::shared_ptr< IOHprofiler_problem<int> >  problem) {}
  
  // Updated here.
  virtual void track_problem(std::shared_ptr< IOHprofiler_problem<double> > problem) {}
  
  // Adding virtual functions for more IuputType IOHprofiler_problem.

private:
  int observer_interval; /// < variable for recording by a static interval.
  bool observer_complete_flag; /// < variable for recording complete optimization process. 
  bool observer_update_flag; /// < variable for recording when a better solution is found.
 
  std::vector<int> observer_time_points; /// < variables for recording at pre-defined points.
  size_t evaluations_value1; /// < intermediate variables for calculating points with 'observer_time_points'.
  size_t time_points_index; /// < intermediate variables for calculating points with 'observer_time_points'.
  int time_points_expi; /// < intermediate variables for calculating points with 'observer_time_points'.
  int observer_time_points_exp_base1;

  int observer_number_of_evaluations; /// < variables for recording with a pre-defined times in each exponential boxplot.
  size_t evaluations_value2; /// < intermediate variables for calculating points with 'observer_number_of_evaluations'.
  int evaluations_expi; /// < intermediate variables for calculating points with 'observer_number_of_evaluations'.
  int observer_time_points_exp_base2;
  
  /// todo. Currently this is only for single objective optimization.
  double current_best_fitness;
};
#endif //_IOHPROFILER_OBSERVER_HPP