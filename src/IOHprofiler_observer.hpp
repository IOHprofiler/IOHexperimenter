#ifndef _IOHPROFILER_OBSERVER_H
#define _IOHPROFILER_OBSERVER_H

#include "common.h"
// Defines a general framework of logger methods for IOHprofiler.
//   init_logger(): Implementation for preparing work for logging process.
//   For example: open/create folder for logging csv file, or connect 
//   database for logging content.
//
//   write_evaluation_info(); Process for recording evaluation info as
//   triggers are targeted. For example, write a line into corresponding
//   csv file, or store info into database.
//
//   close_logger(): For example, close file stream for csv files, or 
//   disconnect from database.
//
// Defines serval trigger methods that determine when to record evaluations.
//   complete_trigger(): working with the variable `observer_complete_flag`,
//   if it is true, recording every evaluations.
//   interval_trigger(): working with the variable `observer_interval`, which 
//   means recording evaluations each observer_interval time.
//   update_trigger(): set as true as a new better solution is found.
class IOHprofiler_observer {
public:
  
  IOHprofiler_observer(){};
  IOHprofiler_observer& operator = (IOHprofiler_observer& observer) {
    this->observer_interval = observer.observer_interval;
    this->observer_complete_flag = observer.observer_complete_flag;
    this->observer_update_flag = observer.observer_update_flag;
    copyVector(observer.observer_time_points,this->observer_time_points);
    this->observer_number_of_evaluations = observer.observer_number_of_evaluations;
    this->evaluations_value1 = observer.evaluations_value1;
    this->evaluations_value2 = observer.evaluations_value2;
    this->time_points_expi = observer.time_points_expi;
    this->time_points_index = observer.time_points_index;
    this->evaluations_expi = observer.evaluations_expi;
    this->current_best_fitness = observer.current_best_fitness;
  };

  // Operations for *.cdat 
  void set_complete_flag(bool complete_flag) {
    this->observer_complete_flag = complete_flag;
  }

  bool complete_status() {
    return observer_complete_flag;
  }

  bool complete_trigger() {
  	return observer_complete_flag;
  }
  // End for *.cdat

  // Operations for *.idat  
  void set_interval(int interval) {
    this->observer_interval = interval;  
  }

  bool interval_status() {
    if(observer_interval == 0) return false;
    else return true;
  }

  bool interval_trigger(size_t evaluations) {
    if(observer_interval == 0) return false;
  	if(evaluations == 1 || evaluations % observer_interval == 0) return true;
  	else return false;
  }
  // End for *.idat

  // Operations for *.dat
  void set_update_flag(bool update_flag) {
    this->observer_update_flag = update_flag;
  }

  bool update_status() {
    return observer_update_flag;
  }

  bool update_trigger(double fitness) {
    if(observer_update_flag == false) return false;

    if(fitness > current_best_fitness) {
    	this->current_best_fitness = fitness;
    	return true;
    }

    return false;
  }
  // End for *dat.

  // Operations for *.tdat
  void set_time_points(std::vector<int> time_points, int number_of_evaluations) {
    copyVector(time_points, this->observer_time_points);
    this->observer_number_of_evaluations = number_of_evaluations;
  }

  bool time_points_status() {
    if(this->observer_time_points.size() > 0) return true;
    if(this->observer_number_of_evaluations > 0) return true;
    return false;
  }

  bool time_points_trigger(size_t evaluations) {
    bool result = false;

    if(evaluations == this->evaluations_value1) {
      result = true;
      if(this->time_points_index < this->observer_time_points.size() - 1) {
        this->time_points_index++;
      }
      else {
        this->time_points_index = 0;
        this->time_points_expi++;
      }
      this->evaluations_value1 = (size_t)(this->observer_time_points[this->time_points_index] * pow(10,this->time_points_expi));
      result = true;
    }


    if(evaluations == this->evaluations_value2) {
      while((size_t)floor(pow(10,(double)this->evaluations_expi/(double)this->observer_number_of_evaluations)) <= this->evaluations_value2) {
        this->evaluations_expi++;
      }
      this->evaluations_value2 = (size_t)floor(pow(10,(double)this->evaluations_expi/(double)this->observer_number_of_evaluations));
      result = true;
    }
    return result;
  }
  // End for *.tdat

  void reset_fitness_for_update_trigger(){
    this->current_best_fitness = DBL_MIN_EXP;
  }
  
  // For *.idat
  int observer_interval;
  
  // For *.cdat
  bool observer_complete_flag;
  
  // For *.dat
  bool observer_update_flag;
 
  // For *.tdat
  std::vector<int> observer_time_points;
  int observer_number_of_evaluations;
  size_t evaluations_value1;
  size_t evaluations_value2;
  int time_points_expi = 0;
  size_t time_points_index = 0;
  int evaluations_expi = 0;

  double current_best_fitness = DBL_MIN_EXP;
  
};

#endif //_IOHPROFILER_OBSERVER_H