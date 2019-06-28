#ifndef _IOHPROFILER_PBO_SUITE_HPP
#define _IOHPROFILER_PBO_SUITE_HPP

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

#include "../IOHprofiler_suite.hpp"

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
    registerProblem();
  };

  PBO_suite(std::vector<int> problem_id, std::vector<int> instance_id, std::vector<int> dimension){

    // Need to check if the input values are valid.
    IOHprofiler_set_suite_problem_id(problem_id);
    IOHprofiler_set_suite_instance_id(instance_id);
    IOHprofiler_set_suite_dimension(dimension);
    IOHprofiler_set_suite_name("PBO");
    registerProblem();
  }

  void registerProblem() {
    registerInFactory<IOHprofiler_problem<int>,OneMax> regOneMax("OneMax");
    registerInFactory<IOHprofiler_problem<int>,OneMax_Dummy1> regOneMax_Dummy1("OneMax_Dummy1");
    registerInFactory<IOHprofiler_problem<int>,OneMax_Dummy2> regOneMax_Dummy2("OneMax_Dummy2");
    registerInFactory<IOHprofiler_problem<int>,OneMax_Epistasis> regOneMax_Epistasis("OneMax_Epistasis");
    registerInFactory<IOHprofiler_problem<int>,OneMax_Neutrality> regOneMax_Neutrality("OneMax_Neutrality");
    registerInFactory<IOHprofiler_problem<int>,OneMax_Ruggedness1> regOneMax_Ruggedness1("OneMax_Ruggedness1");
    registerInFactory<IOHprofiler_problem<int>,OneMax_Ruggedness2> regOneMax_Ruggedness2("OneMax_Ruggedness2");
    registerInFactory<IOHprofiler_problem<int>,OneMax_Ruggedness3> regOneMax_Ruggedness3("OneMax_Ruggedness3");

    registerInFactory<IOHprofiler_problem<int>,LeadingOnes> regLeadingOnes("LeadingOnes");
    registerInFactory<IOHprofiler_problem<int>,LeadingOnes_Dummy1> regLeadingOnes_Dummy1("LeadingOnes_Dummy1");
    registerInFactory<IOHprofiler_problem<int>,LeadingOnes_Dummy2> regLeadingOnes_Dummy2("LeadingOnes_Dummy2");
    registerInFactory<IOHprofiler_problem<int>,LeadingOnes_Epistasis> regLeadingOnes_Epistasis("LeadingOnes_Epistasis");
    registerInFactory<IOHprofiler_problem<int>,LeadingOnes_Neutrality> regLeadingOnes_Neutrality("LeadingOnes_Neutrality");
    registerInFactory<IOHprofiler_problem<int>,LeadingOnes_Ruggedness1> regLeadingOnes_Ruggedness1("LeadingOnes_Ruggedness1");
    registerInFactory<IOHprofiler_problem<int>,LeadingOnes_Ruggedness2> regLeadingOnes_Ruggedness2("LeadingOnes_Ruggedness2");
    registerInFactory<IOHprofiler_problem<int>,LeadingOnes_Ruggedness3> regLeadingOnes_Ruggedness3("LeadingOnes_Ruggedness3");
    
    registerInFactory<IOHprofiler_problem<int>,Linear> regLinear("Linear");
    registerInFactory<IOHprofiler_problem<int>,MIS> regMIS("MIS");
    registerInFactory<IOHprofiler_problem<int>,LABS> regLABS("LABS");
    registerInFactory<IOHprofiler_problem<int>,NQueens> regNQueens("NQueens");
    registerInFactory<IOHprofiler_problem<int>,Ising_1D> regIsing_1D("Ising_1D");
    registerInFactory<IOHprofiler_problem<int>,Ising_2D> regIsing_2D("Ising_2D");
    registerInFactory<IOHprofiler_problem<int>,Ising_Triangle> regIsing_Triangle("Ising_Triangle");
  
    mapIDTOName(1,"OneMax");
    mapIDTOName(2,"LeadingOnes");
    mapIDTOName(3,"Linear");
    mapIDTOName(4,"OneMax_Dummy1");
    mapIDTOName(5,"OneMax_Dummy2");
    mapIDTOName(6,"OneMax_Neutrality");
    mapIDTOName(7,"OneMax_Epistasis");
    mapIDTOName(8,"OneMax_Ruggedness1");
    mapIDTOName(9,"OneMax_Ruggedness2");
    mapIDTOName(10,"OneMax_Ruggedness3");
    mapIDTOName(11,"LeadingOnes_Dummy1");
    mapIDTOName(12,"LeadingOnes_Dummy2");
    mapIDTOName(13,"LeadingOnes_Neutrality");
    mapIDTOName(14,"LeadingOnes_Epistasis");
    mapIDTOName(15,"LeadingOnes_Ruggedness1");
    mapIDTOName(16,"LeadingOnes_Ruggedness2");
    mapIDTOName(17,"LeadingOnes_Ruggedness3");
    mapIDTOName(18,"LABS");
    mapIDTOName(22,"MIS");
    mapIDTOName(19,"Ising_1D");
    mapIDTOName(20,"Ising_2D");
    mapIDTOName(21,"Ising_Triangle");
    mapIDTOName(23,"NQueens");
  };
};

#endif