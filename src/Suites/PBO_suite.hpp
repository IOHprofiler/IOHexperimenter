#ifndef _IOHPROFILER_PBO_SUITE_HPP
#define _IOHPROFILER_PBO_SUITE_HPP

#include "../IOHprofiler_suite.hpp"
// This is a suite consists of pseudo-Boolean problmes. By calling problem from a suite, .info
// files will be generated with log files.

class PBO_suite : public IOHprofiler_suite<int> {
public:

  PBO_suite() {
    std::vector<int> problem_id;
    std::vector<int> instance_id;
    std::vector<int> dimension;
    for (int i = 0; i < 23; ++i) {
      problem_id.push_back(i+1);
    }
    for (int i = 0; i < 1; ++i) {
      instance_id.push_back(i+1);
    }
    dimension.push_back(100);
    
    IOHprofiler_set_suite_problem_id(problem_id);
    IOHprofiler_set_suite_instance_id(instance_id);
    IOHprofiler_set_suite_dimension(dimension);
    IOHprofiler_set_suite_name("PBO");
    init_problem_set();
  };

  PBO_suite(std::vector<int> problem_id, std::vector<int> instance_id, std::vector<int> dimension){

    // Need to check if the input values are valid.
    IOHprofiler_set_suite_problem_id(problem_id);
    IOHprofiler_set_suite_instance_id(instance_id);
    IOHprofiler_set_suite_dimension(dimension);
    IOHprofiler_set_suite_name("PBO");
    init_problem_set();
  }

  void add_problem(int problem_id, int instance_id, int dimension){
    switch(problem_id){
      case 1 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new OneMax(instance_id,dimension)));
        break;
      case 2 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new LeadingOnes(instance_id,dimension)));
        break;
      case 3 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new Linear(instance_id,dimension)));
        break;
      case 4 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new OneMax_Dummy1(instance_id,dimension)));
        break;
      case 5 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new OneMax_Dummy2(instance_id,dimension)));
        break;
      case 6 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new OneMax_Neutrality(instance_id,dimension)));
        break;
      case 7 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new OneMax_Epistasis(instance_id,dimension)));
        break;
      case 8 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new OneMax_Ruggedness1(instance_id,dimension)));
        break;
      case 9 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new OneMax_Ruggedness2(instance_id,dimension)));
        break;
      case 10 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new OneMax_Ruggedness3(instance_id,dimension)));
        break;
      case 11 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new LeadingOnes_Dummy1(instance_id,dimension)));
        break;
      case 12 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new LeadingOnes_Dummy2(instance_id,dimension)));
        break;
      case 13 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new LeadingOnes_Neutrality(instance_id,dimension)));
        break;
      case 14 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new LeadingOnes_Epistasis(instance_id,dimension)));
        break;
      case 15 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new LeadingOnes_Ruggedness1(instance_id,dimension)));
        break;
      case 16 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new LeadingOnes_Ruggedness2(instance_id,dimension)));
        break;
      case 17 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new LeadingOnes_Ruggedness3(instance_id,dimension)));
        break;
      case 18 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new LABS(instance_id,dimension)));
        break;
      case 19 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new Ising_1D(instance_id,dimension)));
        break;
      case 20 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new Ising_2D(instance_id,dimension)));
        break;
      case 21 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new Ising_Triangle(instance_id,dimension)));
        break;
      case 22 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new MIS(instance_id,dimension)));
        break;
      case 23 :
        problem_list.push_back(std::shared_ptr<IOHprofiler_problem<int>>(new NQueens(instance_id,dimension)));
        break;
      default : 
        IOH_error("Undefined problem");
        break;
    }
  }
   
};

#endif