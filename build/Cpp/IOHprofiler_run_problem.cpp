
#include <f_one_max.hpp>
#include <f_w_model_one_max.hpp>
#include <IOHprofiler_csv_logger.h>


std::vector<int> Initialization(int dimension) {
  std::vector<int> x;
  x.reserve(dimension);
  for (int i = 0; i != dimension; ++i){
      x.push_back(rand()% 2);
  }
  return x;
};

int mutation(std::vector<int> &x, double mutation_rate) {
  int result = 0;
  int n = x.size();
  for (int i = 0; i != n; ++i) {
    if (rand() / double(RAND_MAX) < mutation_rate) {
      x[i] = (x[i] + 1) % 2;
      result = 1;
    }
  }
  return result;
}


/// In this session, the algorithm will be tested on a OneMax based w-model.
/// We declare a W_Model_OneMax class, and get fitness values via the function 
/// w_model_om.evaluate(). Since W-model is instance based, before
/// testing any instances, it must be configured by using set_w_setting().
void _run_w_model() {


  std::vector<int> time_points{1,2,5};
  IOHprofiler_csv_logger logger("./","run_w_model","EA","EA");
  logger.set_complete_flag(true);
  logger.set_interval(0);
  logger.set_time_points(time_points,10);
  logger.activate_logger();
  
  /// Configure w_model
  int dimension = 100;
  double w_model_suite_dummy_para = 0.75;
  int w_model_suite_epitasis_para = 4;
  int w_model_suite_neutrality_para = 3;
  int w_model_suite_ruggedness_para = 100;

  W_Model_OneMax w_model_om;
  std::string problem_name = "OneMax";
  problem_name = problem_name 
                + "_D" + std::to_string((int)(w_model_suite_dummy_para * dimension))
                + "_E" + std::to_string(w_model_suite_epitasis_para)
                + "_N" + std::to_string(w_model_suite_neutrality_para)
                + "_R" + std::to_string(w_model_suite_ruggedness_para);
  /// This must be called to configure the w-model to be tested.
  w_model_om.set_w_setting(w_model_suite_dummy_para,w_model_suite_epitasis_para,
                                w_model_suite_neutrality_para,w_model_suite_ruggedness_para);
  /// Set problem_name based on the configuration.
  w_model_om.IOHprofiler_set_problem_name(problem_name);
  /// Set problem_id as 1.
  w_model_om.IOHprofiler_set_problem_id(1);
  /// Set dimension.
  w_model_om.IOHprofiler_set_number_of_variables(dimension);
  int restart_flag = 0;
  while (restart_flag < 10) {
    /// reset_problem must be called before testing the same problem class repeatedly.
    w_model_om.reset_problem();
    logger.track_problem(w_model_om);
  
    
    std::vector<int> x;
    std::vector<int> x_star;
    double y;
    double best_value;
    double mutation_rate = 1.0/dimension;

    x = Initialization(dimension);
    x_star = x;
    y = w_model_om.evaluate(x);
    logger.do_log(w_model_om.loggerInfo());
    best_value = y;

    int budget = 10000;
    while(!w_model_om.IOHprofiler_hit_optimal() && budget > 0) {
      x = x_star;
      if(mutation(x,mutation_rate)) {
        y = w_model_om.evaluate(x);
        logger.do_log(w_model_om.loggerInfo());
        budget--;
      }
      if(y > best_value) {
        best_value = y;
        x_star = x;
      }
    }
    restart_flag++;
  }
          
}


/// In this session, the algorithm will be tested on only one problem (OneMax).
/// Therefore we declare a OneMax class, and get the fitness by the statement
/// om.evaluate().
void _run_problem() {

  /// Allocate the problem to be tested.
  OneMax om;
  int dimension = 1000;
  om.IOHprofiler_set_number_of_variables(dimension);
  
  /// If no logger is added, there will be not any output files, but users
  /// can still get fitness values.
  std::vector<int> time_points{1,2,5};
  IOHprofiler_csv_logger logger("./","run_problem","EA","EA");
  logger.set_complete_flag(true);
  logger.set_interval(0);
  logger.set_time_points(time_points,10);
  logger.activate_logger();
  logger.track_problem(om);
  std::vector<int> x;
  std::vector<int> x_star;
  double y;
  double best_value;
  double mutation_rate = 1.0/dimension;

  x = Initialization(dimension);
  x_star = x;
  y = om.evaluate(x);
  logger.do_log(om.loggerInfo());
  best_value = y;

  while(!om.IOHprofiler_hit_optimal()) {
    x = x_star;
    if(mutation(x,mutation_rate)) {
      y = om.evaluate(x);
      logger.do_log(om.loggerInfo());
    }
    if(y > best_value) {
      best_value = y;
      x_star = x;
    }
  }
}

int main(){
  _run_problem();
  _run_w_model();
  return 0;
}

