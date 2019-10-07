/// \file IOHprofiler_observer.hpp
/// \brief Head file for class IOHprofiler_observer.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _IOHPROFILER_OBSERVER_H
#define _IOHPROFILER_OBSERVER_H

#include "IOHprofiler_common.h"
/// \brief A class of methods of setting triggers recording evaluations.
///
/// Four methods is introduced here:
///   1. Recording evaluations by a static interval.
///   2. Recording complete evaluations.
///   3. Recording evaluations as the best found solution is updated.
///   4. Recording evaluations at pre-defined points or/and with a static number for each exponential bucket.
class IOHprofiler_observer {
public:
  
  IOHprofiler_observer() {};
  ~IOHprofiler_observer() {};

  IOHprofiler_observer(const IOHprofiler_observer &) = delete;
  IOHprofiler_observer &operator = (const IOHprofiler_observer&) = delete;
  
  void set_complete_flag(bool complete_flag) {
    this->observer_complete_flag = complete_flag;
  }

  bool complete_status() {
    return observer_complete_flag;
  }

  bool complete_trigger() {
  	return observer_complete_flag;
  }
  
  void set_interval(int interval) {
    this->observer_interval = interval;  
  }

  bool interval_status() {
    if (observer_interval == 0) {
      return false;
    } else {
      return true;
    }
  }

  bool interval_trigger(size_t evaluations) {
    if (observer_interval == 0) {
      return false;
    }
  	if (evaluations == 1 || evaluations % observer_interval == 0) {
      return true;
    } else {
      return false;
    }
  }
  
  void set_update_flag(bool update_flag) {
    this->observer_update_flag = update_flag;
  }

  bool update_status() {
    return observer_update_flag;
  }

  bool update_trigger(double fitness, int optimization_type) {
    if (observer_update_flag == false) {
      return false;
    }

    if (compareObjectives(fitness, current_best_fitness, optimization_type)) {
    	this->current_best_fitness = fitness;
    	return true;
    }

    return false;
  }
  
  void set_time_points(std::vector<int> time_points, int number_of_evaluations) {
    this->observer_time_points = time_points;
    this->observer_number_of_evaluations = number_of_evaluations;
  }

  bool time_points_status() {
    if (this->observer_time_points.size() > 0) {
      if(!(this->observer_time_points.size() == 1 && this->observer_time_points[0] == 0)) {
        return true;
      }
    }
    if (this->observer_number_of_evaluations > 0) {
      return true;
    }
    return false;
  }

  bool time_points_trigger(size_t evaluations) {
    if (this->time_points_status() == false) {
      return false;
    }
    bool result = false;

    /// evaluations_values is to be set by 10^n * elements of observer_time_points.
    /// For example, observer_time_points = {1,2,5}, the trigger returns true at 1, 2, 5, 10*1, 10*2, 10*5, 100*1, 100*2, 100*5,... 
    if(evaluations == this->evaluations_value1) {
      result = true;
      if (this->time_points_index < this->observer_time_points.size() - 1) {
        this->time_points_index++;
      } else {
        this->time_points_index = 0;
        this->time_points_expi++;
      }
      this->evaluations_value1 = (size_t)(this->observer_time_points[this->time_points_index] * pow(10,this->time_points_expi));
      result = true;
    }
    
    /// evaluations_value2 = floor(10^(i/n), n is observer_number_of_evaluations.
    /// It maintains that in each [10^m, 10^(m+1)], there will be observer_number_of_evaluations evaluations are stored.
    if (evaluations == this->evaluations_value2) {
      while ((size_t)floor(pow(10,(double)this->evaluations_expi/(double)this->observer_number_of_evaluations)) <= this->evaluations_value2) {
        this->evaluations_expi++;
      }
      this->evaluations_value2 = (size_t)floor(pow(10,(double)this->evaluations_expi/(double)this->observer_number_of_evaluations));
      result = true;
    }
    return result;
  }

  void reset_observer(const int optimization_type) {
    if (optimization_type == 1) {
      this->current_best_fitness = -DBL_MAX;
    } else {
      this->current_best_fitness = DBL_MAX;
    }
    this->evaluations_value1 = 1;
    this->time_points_index = 0;
    this->time_points_expi = 0;
    this->evaluations_value2 = 1;
    this->evaluations_expi = 0;
  }

private:
  int observer_interval = 0; /// < variable for recording by a static interval.
  bool observer_complete_flag = false; /// < variable for recording complete optimization process. 
  bool observer_update_flag = true; /// < variable for recording when a better solution is found.
 
  std::vector<int> observer_time_points = {0}; /// < variables for recording at pre-defined points.
  size_t evaluations_value1 = 1; /// < intermediate variables for calculating points with 'observer_time_points'.
  size_t time_points_index = 0; /// < intermediate variables for calculating points with 'observer_time_points'.
  int time_points_expi = 0; /// < intermediate variables for calculating points with 'observer_time_points'.

  int observer_number_of_evaluations; /// < variables for recording with a pre-defined times in each exponential boxplot.
  size_t evaluations_value2 = 1; /// < intermediate variables for calculating points with 'observer_number_of_evaluations'.
  int evaluations_expi = 0; /// < intermediate variables for calculating points with 'observer_number_of_evaluations'.

  /// todo. Currently this is only for single objective optimization.
  double current_best_fitness;
  
};

#endif //_IOHPROFILER_OBSERVER_H