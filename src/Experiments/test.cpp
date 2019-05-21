#include "IOHprofiler_configuration.hpp"
#include "IOHprofiler_experimenter.hpp"


void random_search(std::shared_ptr<IOHprofiler_problem<int>> problem) {
  std::vector<int> x;
  std::vector<int> x_star;
  std::vector<double> y;
  double best_value;
  srand((unsigned)time(NULL)); 

  for(int i = 0; i != problem->IOHprofiler_get_number_of_variables(); ++i){
    x.push_back(rand()% 2);
  }
  copyVector(x,x_star);
  y = problem->evaluate(x);
  best_value = y[0];
  int tt= 0;
  while(tt < 20) {
    copyVector(x_star,x);
    for(int i = 0; i != problem->IOHprofiler_get_number_of_variables(); ++i) {
      if(rand() / double(RAND_MAX) < 0.001) {
        x[i] = (x[i] + 1) % 2;
      }
    }

    y = problem->evaluate(x);
    if(y[0] > best_value) {
      best_value = y[0];
      copyVector(x,x_star);
    }
    if(best_value == problem->IOHprofiler_get_number_of_variables()) break;
        tt++;
  }
}

int main() {
  // IOHprofiler_configuration conf;
  // std::string a = "./configuration.ini";
  // conf.readcfg(a);
  std::string configName = "./configuration.ini";
  IOHprofiler_experimenter<int> experimenter(configName,random_search);
  experimenter._run();
  return 0;
}