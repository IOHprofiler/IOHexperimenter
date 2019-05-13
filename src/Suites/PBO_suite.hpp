#include "../Problems/f_one_max.hpp"
#include "../Problems/f_leading_ones.hpp"
#include "../Problems/f_linear.hpp"
#include "../Problems/f_one_max_dummy1.hpp"
#include "../Problems/f_one_max_dummy2.hpp"
#include "../Problems/f_one_max_neutrality.hpp"
#include "../Problems/f_one_max_epistasis.hpp"
#include "../Problems/f_one_max_ruggedness1.hpp"
#include "../Problems/f_one_max_ruggedness2.hpp"
#include "../Problems/f_one_max_ruggedness3.hpp"
#include "../Problems/f_leading_ones_dummy1.hpp"
#include "../Problems/f_leading_ones_dummy2.hpp"
#include "../Problems/f_leading_ones_neutrality.hpp"
#include "../Problems/f_leading_ones_epistasis.hpp"
#include "../Problems/f_leading_ones_ruggedness1.hpp"
#include "../Problems/f_leading_ones_ruggedness2.hpp"
#include "../Problems/f_leading_ones_ruggedness3.hpp"
#include "../Problems/f_labs.hpp"
#include "../Problems/f_ising_1D.hpp"
#include "../Problems/f_ising_2D.hpp"
#include "../Problems/f_ising_triangle.hpp"
#include "../Problems/f_MIS.hpp"
#include "../Problems/f_N_queens.hpp"
#include "../IOHprofiler_problem.hpp"
#include "../common.h"
#include "../IOHprofiler_csv_logger.hpp"
// This is a suite consists of pseudo-Boolean problmes. By calling problem from a suite, .info
// files will be generated with log files.

class PBO_suite {
public:
  std::string suite_name = "PBO";
  int number_of_problems;
  int number_of_instances;
  int number_of_dimensions;

  std::vector<int> problem_id;
  std::vector<int> instance_id;
  std::vector<int> dimension;

  int problem_id_index = 0;
  int instance_id_index = 0;
  int dimension_id_index = 0;

  // Common variables for different classes of problems.
  OneMax *onemax;
  LeadingOnes *leadingones;
  Linear * linear;
  OneMax_Dummy1 *onemax_dummy1;
  OneMax_Dummy2 *onemax_dummy2;
  OneMax_Neutrality *onemax_neutrality;
  OneMax_Epistasis *onemax_epistasis;
  OneMax_Ruggedness1 *onemax_ruggedness1;
  OneMax_Ruggedness2 *onemax_ruggedness2;
  OneMax_Ruggedness3 *onemax_ruggedness3;
  LeadingOnes_Dummy1 *leadingones_dummy1;
  LeadingOnes_Dummy2 *leadingones_dummy2;
  LeadingOnes_Neutrality *leadingones_neutrality;
  LeadingOnes_Epistasis *leadingones_epistasis;
  LeadingOnes_Ruggedness1 *leadingones_ruggedness1;
  LeadingOnes_Ruggedness2 *leadingones_ruggedness2;
  LeadingOnes_Ruggedness3 *leadingones_ruggedness3;
  LABS *labs;
  Ising_1D *ising_1d;
  Ising_2D *ising_2d;
  Ising_Triangle *ising_triangle;
  MIS *mis;
  NQueens *nqueens;
   

  // An common interface of the problem to be tested.
  IOHprofiler_problem<int> *current_problem = NULL;

  IOHprofiler_csv_logger csv_logger;

  PBO_suite() {
    number_of_problems = 23;
    number_of_instances = 1;
    number_of_dimensions = 1;
    for (int i = 0; i < number_of_problems; ++i) {
      problem_id.push_back(i+1);
    }
    for (int i = 0; i < number_of_instances; ++i) {
      instance_id.push_back(i+1);
    }
    dimension.push_back(100);
  };

  PBO_suite(std::vector<int> problem_id, std::vector<int> instance_id, std::vector<int> dimension){

    // Need to check if the input values are valid.
    number_of_problems = problem_id.size();
    number_of_instances = instance_id.size();
    number_of_dimensions = dimension.size();

    copyVector(problem_id,this->problem_id);
    copyVector(instance_id,this->instance_id);
    copyVector(dimension,this->dimension);
  }

  // The function to acquire problems of the suite one by one until NULL returns.
  IOHprofiler_problem<int> * get_next_problem() {
    if(problem_id_index == number_of_problems) {
      csv_logger.write_info(current_problem->IOHprofiler_get_instance_id(),current_problem->IOHprofiler_get_best_so_far_transformed_objectives()[0],current_problem->IOHprofiler_get_best_so_far_transformed_evaluations());
      current_problem->clearLogger();
      return NULL;
    }
    if(current_problem != NULL) {
      csv_logger.write_info(current_problem->IOHprofiler_get_instance_id(),current_problem->IOHprofiler_get_best_so_far_transformed_objectives()[0],current_problem->IOHprofiler_get_best_so_far_transformed_evaluations());
      current_problem->clearLogger();
    }

    // More problems need to be added here
    if(problem_id[problem_id_index] == 1) {
      onemax = new OneMax(instance_id[instance_id_index],dimension[dimension_id_index]);
      current_problem = onemax;
    } else if(problem_id[problem_id_index] == 2 ) {
      leadingones = new LeadingOnes(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = leadingones;
    } else if(problem_id[problem_id_index] == 3 ) {
      linear = new Linear(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = linear;
    } else if(problem_id[problem_id_index] == 4 ) {
      onemax_dummy1 = new OneMax_Dummy1(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = onemax_dummy1;
    } else if(problem_id[problem_id_index] == 5 ) {
      onemax_dummy2 = new OneMax_Dummy2(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = onemax_dummy2;
    } else if(problem_id[problem_id_index] == 6 ) {
      onemax_neutrality = new OneMax_Neutrality(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = onemax_neutrality;
    } else if(problem_id[problem_id_index] == 7 ) {
      onemax_epistasis = new OneMax_Epistasis(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = onemax_epistasis;
    } else if(problem_id[problem_id_index] == 8 ) {
      onemax_ruggedness1 = new OneMax_Ruggedness1(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = onemax_ruggedness1;
    } else if(problem_id[problem_id_index] == 9 ) {
      onemax_ruggedness2 = new OneMax_Ruggedness2(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = onemax_ruggedness2;
    } else if(problem_id[problem_id_index] == 10 ) {
      onemax_ruggedness3 = new OneMax_Ruggedness3(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = onemax_ruggedness3;
    } else if(problem_id[problem_id_index] == 11 ) {
      leadingones_dummy1 = new LeadingOnes_Dummy1(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = leadingones_dummy1;
    } else if(problem_id[problem_id_index] == 12 ) {
      leadingones_dummy2 = new LeadingOnes_Dummy2(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = leadingones_dummy2;
    } else if(problem_id[problem_id_index] == 13 ) {
      leadingones_neutrality = new LeadingOnes_Neutrality(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = leadingones_neutrality;
    } else if(problem_id[problem_id_index] == 14 ) {
      leadingones_epistasis = new LeadingOnes_Epistasis(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = leadingones_epistasis;
    } else if(problem_id[problem_id_index] == 15 ) {
      leadingones_ruggedness1 = new LeadingOnes_Ruggedness1(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = leadingones_ruggedness1;
    } else if(problem_id[problem_id_index] == 16 ) {
      leadingones_ruggedness2 = new LeadingOnes_Ruggedness2(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = leadingones_ruggedness2;
    } else if(problem_id[problem_id_index] == 17 ) {
      leadingones_ruggedness3 = new LeadingOnes_Ruggedness3(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = leadingones_ruggedness3;
    } else if(problem_id[problem_id_index] == 18 ) {
      labs = new LABS(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = labs;
    } else if(problem_id[problem_id_index] == 19 ) {
      ising_1d = new Ising_1D(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = ising_1d;
    } else if(problem_id[problem_id_index] == 20 ) {
      ising_2d = new Ising_2D(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = ising_2d;
    } else if(problem_id[problem_id_index] == 21 ) {
      ising_triangle = new Ising_Triangle(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = ising_triangle;
    } else if(problem_id[problem_id_index] == 22 ) {
      mis = new MIS(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = mis;
    } else if(problem_id[problem_id_index] == 23 ) {
      nqueens = new NQueens(instance_id[instance_id_index],dimension[dimension_id_index]); 
      current_problem = nqueens;
    }
     
    instance_id_index++;
    if(instance_id_index == instance_id.size()){
      instance_id_index= 0; 
      dimension_id_index++;
      if(dimension_id_index == number_of_dimensions) {
        dimension_id_index = 0;
        problem_id_index++;
      }
    }
    current_problem->addCSVLogger(csv_logger);
    csv_logger.openInfo(current_problem->IOHprofiler_get_problem_id(),current_problem->IOHprofiler_get_number_of_variables());
    return current_problem;
  };

  // Add a csvLogger for the suite. 
  // The logger of the suite will only control .info files.
  // To output files for evaluation of problems, this logger needs to be added to problems.
  void addCSVLogger(IOHprofiler_csv_logger &logger) {
    csv_logger = logger;
    csv_logger.target_suite(suite_name);
  };
};