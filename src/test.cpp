#include "Functions/f_one_max.cpp"
#include "IOHprofiler_csv_logger.h"
#include <iostream>

int main(){


  double best_value;
  IOHprofiler_csv_logger logger;
  logger.init_logger("./","test","alg","alg",true,true,2);
  OneMax om;

  om.Initilize_problem(1000);
  om.instance_id=1;
  std::vector<int> x;
  std::vector<int> x_star;
  std::vector<double> y;
  x.reserve(1000);
  x_star.reserve(1000);
  om.addCSVLogger(logger);
  srand((unsigned)time(NULL)); 

  for(int i = 0; i != 1000; ++i){
      x.push_back(rand()% 2);
  }
  copyVector(x,x_star);
  om.evaluate(x,y);
  best_value = y[0];
  while(0 < 1) {
    copyVector(x_star,x);
    for(int i = 0; i != 1000; ++i) {
      if(rand() / double(RAND_MAX) < 0.001) {
        x[i] = (x[i] + 1) % 2;
      }
    }

    om.evaluate(x,y);
    if(y[0] > best_value) {
      best_value = y[0];
      copyVector(x,x_star);
    }
    if(best_value == 1000.0) break;
  }

  //IOHprofiler_csv_logger logger("./","test","alg","alg",true,true,2);
/*  logger.target_problem(1,100,1);
  logger.write_line(1,1.0,1.0,1.0,1.0);
  logger.write_line(2,1.0,1.0,3.0,1.0);
  logger.write_line(3,1.0,1.0,2.0,1.0);
  logger.write_line(4,1.0,1.0,3.0,1.0);
  logger.write_line(5,1.0,1.0,4.0,1.0);
  logger.write_line(6,1.0,1.0,5.0,1.0);
  logger.write_line(7,1.0,1.0,6.0,1.0);
  logger.write_line(8,1.0,1.0,1.0,1.0);
  */
  return 0;
}