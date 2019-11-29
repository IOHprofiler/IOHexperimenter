/// \file IOHprofiler_all_problems.cpp
/// \brief cpp file for class IOHprofiler_all_problems.
///
/// A head file includes all problems of IOHprofiler.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _IOHPROFILER_ALL_PROBLEMS_HPP
#define _IOHPROFILER_ALL_PROBLEMS_HPP

#include "f_one_max.hpp"
#include "f_leading_ones.hpp"
#include "f_linear.hpp"
#include "f_one_max_dummy1.hpp"
#include "f_one_max_dummy2.hpp"
#include "f_one_max_neutrality.hpp"
#include "f_one_max_epistasis.hpp"
#include "f_one_max_ruggedness1.hpp"
#include "f_one_max_ruggedness2.hpp"
#include "f_one_max_ruggedness3.hpp"
#include "f_leading_ones_dummy1.hpp"
#include "f_leading_ones_dummy2.hpp"
#include "f_leading_ones_neutrality.hpp"
#include "f_leading_ones_epistasis.hpp"
#include "f_leading_ones_ruggedness1.hpp"
#include "f_leading_ones_ruggedness2.hpp"
#include "f_leading_ones_ruggedness3.hpp"
#include "f_labs.hpp"
#include "f_ising_1D.hpp"
#include "f_ising_2D.hpp"
#include "f_ising_triangle.hpp"
#include "f_MIS.hpp"
#include "f_N_queens.hpp"

#include "IOHprofiler_class_generator.hpp"


static registerInFactory<IOHprofiler_problem<int>,OneMax> regOneMax(std::string = "OneMax");
static registerInFactory<IOHprofiler_problem<int>,OneMax_Dummy1> regOneMax_Dummy1(std::string name ="OneMax_Dummy1");
static registerInFactory<IOHprofiler_problem<int>,OneMax_Dummy2> regOneMax_Dummy2(std::string name ="OneMax_Dummy2");
static registerInFactory<IOHprofiler_problem<int>,OneMax_Epistasis> regOneMax_Epistasis(std::string name ="OneMax_Epistasis");
static registerInFactory<IOHprofiler_problem<int>,OneMax_Neutrality> regOneMax_Neutrality(std::string name ="OneMax_Neutrality");
static registerInFactory<IOHprofiler_problem<int>,OneMax_Ruggedness1> regOneMax_Ruggedness1(std::string name ="OneMax_Ruggedness1");
static registerInFactory<IOHprofiler_problem<int>,OneMax_Ruggedness2> regOneMax_Ruggedness2(std::string name ="OneMax_Ruggedness2");
static registerInFactory<IOHprofiler_problem<int>,OneMax_Ruggedness3> regOneMax_Ruggedness3(std::string name ="OneMax_Ruggedness3");

static registerInFactory<IOHprofiler_problem<int>,LeadingOnes> regLeadingOnes(std::string name ="LeadingOnes");
static registerInFactory<IOHprofiler_problem<int>,LeadingOnes_Dummy1> regLeadingOnes_Dummy1(std::string name ="LeadingOnes_Dummy1");
static registerInFactory<IOHprofiler_problem<int>,LeadingOnes_Dummy2> regLeadingOnes_Dummy2(std::string name ="LeadingOnes_Dummy2");
static registerInFactory<IOHprofiler_problem<int>,LeadingOnes_Epistasis> regLeadingOnes_Epistasis(std::string name ="LeadingOnes_Epistasis");
static registerInFactory<IOHprofiler_problem<int>,LeadingOnes_Neutrality> regLeadingOnes_Neutrality(std::string name ="LeadingOnes_Neutrality");
static registerInFactory<IOHprofiler_problem<int>,LeadingOnes_Ruggedness1> regLeadingOnes_Ruggedness1(std::string name ="LeadingOnes_Ruggedness1");
static registerInFactory<IOHprofiler_problem<int>,LeadingOnes_Ruggedness2> regLeadingOnes_Ruggedness2(std::string name ="LeadingOnes_Ruggedness2");
static registerInFactory<IOHprofiler_problem<int>,LeadingOnes_Ruggedness3> regLeadingOnes_Ruggedness3(std::string name ="LeadingOnes_Ruggedness3");

static registerInFactory<IOHprofiler_problem<int>,Linear> regLinear(std::string name ="Linear");
static registerInFactory<IOHprofiler_problem<int>,MIS> regMIS(std::string name ="MIS");
static registerInFactory<IOHprofiler_problem<int>,LABS> regLABS(std::string name ="LABS");
static registerInFactory<IOHprofiler_problem<int>,NQueens> regNQueens(std::string name ="NQueens");
static registerInFactory<IOHprofiler_problem<int>,Ising_1D> regIsing_1D(std::string name ="Ising_1D");
static registerInFactory<IOHprofiler_problem<int>,Ising_2D> regIsing_2D(std::string name ="Ising_2D");
static registerInFactory<IOHprofiler_problem<int>,Ising_Triangle> regIsing_Triangle(std::string name ="Ising_Triangle");

#endif //_IOHPROFILER_ALL_PROBLEMS_HPP