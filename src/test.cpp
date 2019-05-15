// This file is only used for testing source code functions.
// New test examples will be added later.


#include "Problems/f_one_max.hpp"
#include "IOHprofiler_csv_logger.hpp"
#include "Suites/PBO_suite.hpp"
#include <iostream>

int main(){

  std::vector<int> time_points{1,2,5};
  double best_value;
  IOHprofiler_csv_logger logger;
  logger.init_logger("./","test","alg","alg",true,true,2,time_points,3);
  
  // In this session, the algorithm will be tested on only one problem (OneMax).
  // Therefore we declare a OneMax class, and get the fitness by the statement
  // om.evaluate().
  OneMax om;
  std::vector<int> x;
  std::vector<int> x_star;
  std::vector<double> y;
  om.Initilize_problem(1000);
  om.IOHprofiler_set_instance_id(1);
  // If no logger is added, there will be not any output files, but users
  // can still get fitness values.
  om.addCSVLogger(logger1);

  srand((unsigned)time(NULL)); 

  for(int i = 0; i != om.IOHprofiler_get_number_of_variables(); ++i){
      x.push_back(rand()% 2);
  }
  copyVector(x,x_star);
  // Get fitness.
  om.evaluate(x,y);
  best_value = y[0];
  int tt= 0;
  while(tt < 20) {
    copyVector(x_star,x);
    for(int i = 0; i != om.IOHprofiler_get_number_of_variables(); ++i) {
      if(rand() / double(RAND_MAX) < 0.001) {
        x[i] = (x[i] + 1) % 2;
      }
    }

    om.evaluate(x,y);
    if(y[0] > best_value) {
      best_value = y[0];
      copyVector(x,x_star);
    }
    if(best_value == om.IOHprofiler_get_number_of_variables()) break;
        tt++;
  }


  // In this session multiple problems in PBO suite will be tested.
  // In the example, specfic problems are selected from PBO_suite.
  // User can also get a complete problem collection with default 
  // instances and dimension by the statement '  PBO_suite pbo;'
  std::vector<int> problem_id = {1,2};
  std::vector<int> instance_id ={1,3,4,5,6};
  std::vector<int> dimension = {100,200,300};
  PBO_suite pbo(problem_id,instance_id,dimension);
  IOHprofiler_csv_logger logger1;
  logger1.init_logger("./","test","alg","alg",true,true,2,time_points,3);
  // If no logger is added, there will be not any output files, but users
  // can still get fitness values.
  pbo.addCSVLogger(logger1);
  IOHprofiler_problem<int> *problem;

  // Problems are tested one by one until 'get_next_problem' returns NULL.
  while(problem = pbo.get_next_problem()){

    x.clear();
    x_star.clear();
     srand((unsigned)time(NULL)); 

    for(int i = 0; i != problem->IOHprofiler_get_number_of_variables(); ++i){
        x.push_back(rand()% 2);
    }
    copyVector(x,x_star);
    problem->evaluate(x,y);
    best_value = y[0];
    int tt= 0;
    while(tt < 20) {
      copyVector(x_star,x);
      for(int i = 0; i != problem->IOHprofiler_get_number_of_variables(); ++i) {
        if(rand() / double(RAND_MAX) < 0.001) {
          x[i] = (x[i] + 1) % 2;
        }
      }

      problem->evaluate(x,y);
      if(y[0] > best_value) {
        best_value = y[0];
        copyVector(x,x_star);
      }
      if(best_value == problem->IOHprofiler_get_number_of_variables()) break;
          tt++;
    }
    std::cout << problem->IOHprofiler_get_problem_name() << " " << problem->IOHprofiler_get_instance_id() << " " << problem->IOHprofiler_get_number_of_variables() << std::endl;

  }

  return 0;
}
