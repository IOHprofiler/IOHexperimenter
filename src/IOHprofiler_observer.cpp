/// \file IOHprofiler_observer.cpp
/// \brief Cpp file for the class IOHprofiler_observer.
///
/// \author Furong Ye

#include "IOHprofiler_observer.h"

void IOHprofiler_observer::set_complete_flag(bool complete_flag) {
  this->observer_complete_flag = complete_flag;
}

bool IOHprofiler_observer::complete_status() const {
  return observer_complete_flag;
}

bool IOHprofiler_observer::complete_trigger() const {
  return observer_complete_flag;
}

void IOHprofiler_observer::set_interval(int interval) {
  this->observer_interval = interval;  
}

bool IOHprofiler_observer::interval_status() const {
  if (observer_interval == 0) {
    return false;
  } else {
    return true;
  }
}

bool IOHprofiler_observer::interval_trigger(size_t evaluations) const {
  if (observer_interval == 0) {
    return false;
  }
  if (evaluations == 1 || evaluations % observer_interval == 0) {
    return true;
  } else {
    return false;
  }
}

void IOHprofiler_observer::set_update_flag(bool update_flag) {
  this->observer_update_flag = update_flag;
}

bool IOHprofiler_observer::update_status() const {
  return observer_update_flag;
}

bool IOHprofiler_observer::update_trigger(double fitness, IOH_optimization_type optimization_type) {
  if (observer_update_flag == false) {
    return false;
  }

  if (compareObjectives(fitness, current_best_fitness, optimization_type)) {
    this->current_best_fitness = fitness;
    return true;
  }

  return false;
}

void IOHprofiler_observer::set_time_points(const std::vector<int> & time_points, const int number_of_evaluations, const int time_points_exp_base1, const int time_points_exp_base2) {
  this->observer_time_points = time_points;
  this->observer_number_of_evaluations = number_of_evaluations;
  this->observer_time_points_exp_base1 = time_points_exp_base1;
  this->observer_time_points_exp_base2 = time_points_exp_base2;
}

bool IOHprofiler_observer::time_points_status() const {
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

bool IOHprofiler_observer::time_points_trigger(size_t evaluations) {
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
    this->evaluations_value1 = (size_t)(this->observer_time_points[this->time_points_index] * pow(observer_time_points_exp_base1,this->time_points_expi));
    while (this->evaluations_value1 <= evaluations) {
      if (this->time_points_index < this->observer_time_points.size() - 1) {
        this->time_points_index++;
      } else {
        this->time_points_index = 0;
        this->time_points_expi++;
      }
      this->evaluations_value1 = (size_t)(this->observer_time_points[this->time_points_index] * pow(observer_time_points_exp_base1,this->time_points_expi)); 
    }
  }
  
  /// evaluations_value2 = floor(10^(i/n), n is observer_number_of_evaluations.
  /// It maintains that in each [10^m, 10^(m+1)], there will be observer_number_of_evaluations evaluations are stored.
  if (evaluations == this->evaluations_value2) {
    while ((size_t)floor(pow(observer_time_points_exp_base2,(double)this->evaluations_expi/(double)this->observer_number_of_evaluations)) <= this->evaluations_value2) {
      this->evaluations_expi++;
    }
    this->evaluations_value2 = (size_t)floor(pow(observer_time_points_exp_base2,(double)this->evaluations_expi/(double)this->observer_number_of_evaluations));
    result = true;
  }
  return result;
}

void IOHprofiler_observer::reset_observer(const IOH_optimization_type optimization_type) {
  if (optimization_type == IOH_optimization_type::Maximization) {
    this->current_best_fitness = std::numeric_limits<double>::lowest();
  } else {
    this->current_best_fitness = (std::numeric_limits<double>::max)();
  }
  this->evaluations_value1 = 1;
  this->time_points_index = 0;
  this->time_points_expi = 0;
  this->evaluations_value2 = 1;
  this->evaluations_expi = 0;
}