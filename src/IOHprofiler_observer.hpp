#ifndef _IOHPROFILER_OBSERVER_H
#define _IOHPROFILER_OBSERVER_H


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
  virtual void init_logger();
  virtual void write_evaluation_info();
  virtual void close_logger();

  bool complete_trigger() {
  	return observer_complete_flag;
  }
  
  bool interval_trigger(size_t evaluations) {
  	if(evaluations == 1 && evaluations % observer_interval == 0) return true;
  	else return false;
  }

  bool update(double fitness) {
    if(fitness > current_best_fitness) {
    	this->current_best_fitness = fitness;
    	return true;
    }

    return false;
  }

private:
  int observer_interval;
  bool observer_complete_flag;

  double current_best_fitness = DBL_MIN;
};

#endif #_IOHPROFILER_OBSERVER_H