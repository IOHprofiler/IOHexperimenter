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

  /*IOHprofiler_observer(){};
  virtual void init_logger() ;
  virtual void write_evaluation_info() ;
  virtual void close_logger() ;
*/
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

  void reset_fitness_for_update_trigger(){
    this->current_best_fitness = DBL_MIN_EXP;
  }
  // End for *.ddat


  int observer_interval;
  bool observer_complete_flag;
  bool observer_update_flag;

  double current_best_fitness = DBL_MIN_EXP;

  IOHprofiler_observer& operator = (IOHprofiler_observer& observer) {
    this->observer_interval = observer.observer_interval;
    this->observer_complete_flag = observer.observer_complete_flag;
    this->observer_update_flag = observer.observer_update_flag;
    this->current_best_fitness = observer.current_best_fitness;
  };
  
};

#endif //_IOHPROFILER_OBSERVER_H