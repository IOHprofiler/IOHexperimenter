// This file is only used for testing source code functions.
// New test examples will be added later.


#include "Problems/f_one_max.hpp"
#include "IOHprofiler_csv_logger.h"
#include "Suites/PBO_suite.hpp"
#include <iostream>

int main(){

  std::vector<int> time_points{1,2,5};
  double best_value;
  IOHprofiler_csv_logger logger;
  logger.init_logger("./","test","alg","alg",true,true,2,time_points,3);
  IOHprofiler_csv_logger logger1;
  logger1.init_logger("./","test","alg","alg",true,true,2,time_points,3);
  OneMax om;
  std::vector<int> x;
  std::vector<int> x_star;
  std::vector<double> y;
   std::vector<int> problem_id = {1,2};
   std::vector<int> instance_id ={1,3,4,5,6};
   std::vector<int> dimension = {100,200,300};
   om.Initilize_problem(1000);
   om.IOHprofiler_set_instance_id(1);

   om.addCSVLogger(logger1);
 
   srand((unsigned)time(NULL)); 

    for(int i = 0; i != om.IOHprofiler_get_number_of_variables(); ++i){
        x.push_back(rand()% 2);
    }
    copyVector(x,x_star);
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
  //IOHprofiler_csv_logger logger("./","test","alg","alg",true,true,2);
  /*logger.target_problem(1,100,1);
  logger.write_line(1,1.0,1.0,1.0,1.0);
  logger.write_line(2,1.0,1.0,3.0,1.0);
  logger.write_line(3,1.0,1.0,2.0,1.0);
  logger.write_line(4,1.0,1.0,3.0,1.0);
  logger.write_line(5,1.0,1.0,4.0,1.0);
  logger.write_line(6,1.0,1.0,5.0,1.0);
  logger.write_line(7,1.0,1.0,6.0,1.0);
  logger.write_line(8,1.0,1.0,1.0,1.0);
  */

  PBO_suite pbo(problem_id,instance_id,dimension);
  pbo.addCSVLogger(logger);
  IOHprofiler_problem<int> *problem;


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