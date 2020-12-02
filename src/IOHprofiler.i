%module IOHprofiler

%begin %{
#define SWIG_PYTHON_CAST_MODE
%}

%feature("autodoc", "2");

%include <std_shared_ptr.i>
%include <std_string.i>
%include <std_vector.i>
%include <std_map.i>
%include <stdint.i>
%shared_ptr(IOHprofiler_problem< int >)
%shared_ptr(IOHprofiler_problem< double >)
%shared_ptr(IOHprofiler_suite< double >)
%shared_ptr(IOHprofiler_suite< int >)
%shared_ptr(DoubleValue)
%shared_ptr(IntValue)

%shared_ptr(OneMax)
%shared_ptr(LeadingOnes)
%shared_ptr(Linear)
%shared_ptr(OneMax_Dummy1)
%shared_ptr(OneMax_Dummy2)
%shared_ptr(OneMax_Neutrality)
%shared_ptr(OneMax_Epistasis)
%shared_ptr(OneMax_Ruggedness1)
%shared_ptr(OneMax_Ruggedness2)
%shared_ptr(OneMax_Ruggedness3)
%shared_ptr(LeadingOnes_Dummy1)
%shared_ptr(LeadingOnes_Dummy2)
%shared_ptr(LeadingOnes_Neutrality)
%shared_ptr(LeadingOnes_Epistasis)
%shared_ptr(LeadingOnes_Ruggedness1)
%shared_ptr(LeadingOnes_Ruggedness2)
%shared_ptr(LeadingOnes_Ruggedness3)
%shared_ptr(LABS)
%shared_ptr(MIS)
%shared_ptr(Ising_Ring)
%shared_ptr(Ising_Torus)
%shared_ptr(Ising_Triangular)
%shared_ptr(NQueens)
%shared_ptr(Concatenated_Trap)
%shared_ptr(NK_Landscapes)
%shared_ptr(W_Model_OneMax)
%shared_ptr(W_Model_LeadingOnes)
%shared_ptr(Sphere)
%shared_ptr(Ellipsoid)
%shared_ptr(Rastrigin)
%shared_ptr(Bueche_Rastrigin)
%shared_ptr(Linear_Slope)
%shared_ptr(Attractive_Sector)
%shared_ptr(Step_Ellipsoid)
%shared_ptr(Rosenbrock)
%shared_ptr(Rosenbrock_Rotated)
%shared_ptr(Ellipsoid_Rotated)
%shared_ptr(Discus)
%shared_ptr(Bent_Cigar)
%shared_ptr(Sharp_Ridge)
%shared_ptr(Different_Powers)
%shared_ptr(Rastrigin_Rotated)
%shared_ptr(Weierstrass)
%shared_ptr(Schaffers10)
%shared_ptr(Schaffers1000)
%shared_ptr(Griewank_RosenBrock)
%shared_ptr(Schwefel)
%shared_ptr(Gallagher101)
%shared_ptr(Gallagher21)
%shared_ptr(Katsuura)
%shared_ptr(Lunacek_Bi_Rastrigin)
%shared_ptr(PBO_suite)
%shared_ptr(BBOB_suite)
%{
#include "coco_transformation_objs.hpp"
#include "coco_transformation_vars.hpp"
#include "coco_transformation.hpp"
#include "suite_bbob_legacy_code.hpp"
#include "f_attractive_sector.hpp"
#include "f_bent_cigar.hpp"
#include "f_bueche_rastrigin.hpp"
#include "f_different_powers.hpp"
#include "f_discus.hpp"
#include "f_ellipsoid.hpp"
#include "f_ellipsoid_rotated.hpp"
#include "f_gallagher101.hpp"
#include "f_gallagher21.hpp"
#include "f_griewank_rosenbrock.hpp"
#include "f_rastrigin.hpp"
#include "f_rastrigin_rotated.hpp"
#include "f_rosenbrock.hpp"
#include "f_rosenbrock_rotated.hpp"
#include "f_schaffers1000.hpp"
#include "f_schaffers10.hpp"
#include "f_schwefel.hpp"
#include "f_sharp_ridge.hpp"
#include "f_sphere.hpp"
#include "f_step_ellipsoid.hpp"
#include "f_weierstrass.hpp"
#include "f_linear_slope.hpp"
#include "f_lunacek_bi_rastrigin.hpp"
#include "f_katsuura.hpp"
#include "wmodels.hpp"
#include "f_ising_ring.hpp"
#include "f_ising_torus.hpp"
#include "f_ising_triangular.hpp"
#include "f_labs.hpp"
#include "f_leading_ones_dummy1.hpp"
#include "f_leading_ones_dummy2.hpp"
#include "f_leading_ones_epistasis.hpp"
#include "f_leading_ones.hpp"
#include "f_leading_ones_neutrality.hpp"
#include "f_leading_ones_ruggedness1.hpp"
#include "f_leading_ones_ruggedness2.hpp"
#include "f_leading_ones_ruggedness3.hpp"
#include "f_linear.hpp"
#include "f_MIS.hpp"
#include "f_N_queens.hpp"
#include "f_one_max_dummy1.hpp"
#include "f_one_max_dummy2.hpp"
#include "f_one_max_epistasis.hpp"
#include "f_one_max.hpp"
#include "f_one_max_neutrality.hpp"
#include "f_one_max_ruggedness1.hpp"
#include "f_one_max_ruggedness2.hpp"
#include "f_one_max_ruggedness3.hpp"
#include "f_w_model_one_max.hpp"
#include "f_w_model_leading_ones.hpp"
#include "IOHprofiler_BBOB_suite.hpp"
#include "IOHprofiler_PBO_suite.hpp"
#include "IOHprofiler_class_generator.h"
#include "IOHprofiler_configuration.h"
#include "IOHprofiler_experimenter.h"
#include "IOHprofiler_string.hpp"
#include "IOHprofiler_observer.h"
#include "IOHprofiler_problem.h"
#include "IOHprofiler_suite.h"
#include "IOHprofiler_random.h"
#include "IOHprofiler_transformation.h"
#include "IOHprofiler_csv_logger.h"
#include "IOHprofiler_common.h"
#include "IOHprofiler_platform.h"

typedef unsigned int    uint32_t;
#if defined(SWIGWORDSIZE64)
typedef unsigned long int unit64_t;
#else
typedef unsigned long long int unit64_t;
#endif
typedef  std::map<std::string, int> PROBLEM_NAME_ID; 
typedef  std::map<int, std::string> PROBLEM_ID_NAME; 

%}

%template(intvec) std::vector<int>;
%template(doublevec) std::vector<double>;
%template(stringvec) std::vector<std::string>;

%include "IOHprofiler_configuration.h"
%include "IOHprofiler_experimenter.h"
%include "IOHprofiler_observer.h"

%include "IOHprofiler_problem.h"
%template(IOHprofiler_Problem_int) IOHprofiler_problem<int>;
%template(IOHprofiler_Problem_double) IOHprofiler_problem<double>;
%template(intProblemPtr) std::vector< std::shared_ptr< IOHprofiler_problem< int > > >;
%template(doubleProblemPtr) std::vector< std::shared_ptr< IOHprofiler_problem< double > > >;
%include "IOHprofiler_random.h"
%include "IOHprofiler_string.hpp"

%include "IOHprofiler_transformation.h"
%include "suite_bbob_legacy_code.hpp"
%include "wmodels.hpp"
%include "IOHprofiler_csv_logger.h"
%include "IOHprofiler_common.h"
%include "IOHprofiler_platform.h"
%include "coco_transformation_objs.hpp"
%include "coco_transformation_vars.hpp"
%include "coco_transformation.hpp"

%include "f_attractive_sector.hpp"
%include "f_bent_cigar.hpp"
%include "f_bueche_rastrigin.hpp"
%include "f_different_powers.hpp"
%include "f_discus.hpp"
%include "f_ellipsoid.hpp"
%include "f_ellipsoid_rotated.hpp"
%include "f_gallagher101.hpp"
%include "f_gallagher21.hpp"
%include "f_griewank_rosenbrock.hpp"
%include "f_rastrigin.hpp"
%include "f_rastrigin_rotated.hpp"
%include "f_rosenbrock.hpp"
%include "f_rosenbrock_rotated.hpp"
%include "f_schaffers1000.hpp"
%include "f_schaffers10.hpp"
%include "f_schwefel.hpp"
%include "f_sharp_ridge.hpp"
%include "f_sphere.hpp"
%include "f_step_ellipsoid.hpp"
%include "f_weierstrass.hpp"
%include "f_linear_slope.hpp"
%include "f_lunacek_bi_rastrigin.hpp"
%include "f_katsuura.hpp"
%include "f_ising_ring.hpp"
%include "f_ising_torus.hpp"
%include "f_ising_triangular.hpp"
%include "f_labs.hpp"
%include "f_leading_ones_dummy1.hpp"
%include "f_leading_ones_dummy2.hpp"
%include "f_leading_ones_epistasis.hpp"
%include "f_leading_ones.hpp"
%include "f_leading_ones_neutrality.hpp"
%include "f_leading_ones_ruggedness1.hpp"
%include "f_leading_ones_ruggedness2.hpp"
%include "f_leading_ones_ruggedness3.hpp"
%include "f_linear.hpp"
%include "f_MIS.hpp"
%include "f_N_queens.hpp"
%include "f_one_max_dummy1.hpp"
%include "f_one_max_dummy2.hpp"
%include "f_one_max_epistasis.hpp"
%include "f_one_max.hpp"
%include "f_one_max_neutrality.hpp"
%include "f_one_max_ruggedness1.hpp"
%include "f_one_max_ruggedness2.hpp"
%include "f_one_max_ruggedness3.hpp"
%include "f_w_model_one_max.hpp"
%include "f_w_model_leading_ones.hpp"

%include "IOHprofiler_class_generator.h"


%template (genericGenerator_int_problem) genericGenerator<IOHprofiler_problem<int> >;
%template (genericGenerator_double_problem) genericGenerator<IOHprofiler_problem<double> >;
%include "IOHprofiler_suite.h"
%template(IOHprofiler_suite_int) IOHprofiler_suite< int >;
%template(IOHprofiler_suite_double) IOHprofiler_suite< double >;

%include "IOHprofiler_BBOB_suite.hpp"
%include "IOHprofiler_PBO_suite.hpp"

%apply int{ UInt32 }

%module IOHprofiler
#pragma SWIG nowarn=401
