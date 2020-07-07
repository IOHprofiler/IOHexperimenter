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
#include "src/coco_transformation_objs.hpp"
#include "src/coco_transformation_vars.hpp"
#include "src/coco_transformation.hpp"
#include "src/suite_bbob_legacy_code.hpp"
#include "src/f_attractive_sector.hpp"
#include "src/f_bent_cigar.hpp"
#include "src/f_bueche_rastrigin.hpp"
#include "src/f_different_powers.hpp"
#include "src/f_discus.hpp"
#include "src/f_ellipsoid.hpp"
#include "src/f_ellipsoid_rotated.hpp"
#include "src/f_gallagher101.hpp"
#include "src/f_gallagher21.hpp"
#include "src/f_griewank_rosenbrock.hpp"
#include "src/f_rastrigin.hpp"
#include "src/f_rastrigin_rotated.hpp"
#include "src/f_rosenbrock.hpp"
#include "src/f_rosenbrock_rotated.hpp"
#include "src/f_schaffers1000.hpp"
#include "src/f_schaffers10.hpp"
#include "src/f_schwefel.hpp"
#include "src/f_sharp_ridge.hpp"
#include "src/f_sphere.hpp"
#include "src/f_step_ellipsoid.hpp"
#include "src/f_weierstrass.hpp"
#include "src/f_linear_slope.hpp"
#include "src/f_lunacek_bi_rastrigin.hpp"
#include "src/f_katsuura.hpp"
#include "src/wmodels.hpp"
#include "src/f_ising_ring.hpp"
#include "src/f_ising_torus.hpp"
#include "src/f_ising_triangular.hpp"
#include "src/f_labs.hpp"
#include "src/f_leading_ones_dummy1.hpp"
#include "src/f_leading_ones_dummy2.hpp"
#include "src/f_leading_ones_epistasis.hpp"
#include "src/f_leading_ones.hpp"
#include "src/f_leading_ones_neutrality.hpp"
#include "src/f_leading_ones_ruggedness1.hpp"
#include "src/f_leading_ones_ruggedness2.hpp"
#include "src/f_leading_ones_ruggedness3.hpp"
#include "src/f_linear.hpp"
#include "src/f_MIS.hpp"
#include "src/f_N_queens.hpp"
#include "src/f_one_max_dummy1.hpp"
#include "src/f_one_max_dummy2.hpp"
#include "src/f_one_max_epistasis.hpp"
#include "src/f_one_max.hpp"
#include "src/f_one_max_neutrality.hpp"
#include "src/f_one_max_ruggedness1.hpp"
#include "src/f_one_max_ruggedness2.hpp"
#include "src/f_one_max_ruggedness3.hpp"
#include "src/f_w_model_one_max.hpp"
#include "src/f_w_model_leading_ones.hpp"
#include "src/IOHprofiler_BBOB_suite.hpp"
#include "src/IOHprofiler_PBO_suite.hpp"
#include "src/IOHprofiler_class_generator.h"
#include "src/IOHprofiler_configuration.h"
#include "src/IOHprofiler_experimenter.h"
#include "src/IOHprofiler_string.hpp"
#include "src/IOHprofiler_observer.h"
#include "src/IOHprofiler_problem.h"
#include "src/IOHprofiler_suite.h"
#include "src/IOHprofiler_random.h"
#include "src/IOHprofiler_transformation.h"
#include "src/IOHprofiler_csv_logger.h"
#include "src/IOHprofiler_common.h"
#include "src/IOHprofiler_platform.h"

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

%include "src/IOHprofiler_configuration.h"
%include "src/IOHprofiler_experimenter.h"
%include "src/IOHprofiler_observer.h"

%include "src/IOHprofiler_problem.h"
%template(IOHprofiler_Problem_int) IOHprofiler_problem<int>;
%template(IOHprofiler_Problem_double) IOHprofiler_problem<double>;
%template(intProblemPtr) std::vector< std::shared_ptr< IOHprofiler_problem< int > > >;
%template(doubleProblemPtr) std::vector< std::shared_ptr< IOHprofiler_problem< double > > >;
%include "src/IOHprofiler_random.h"
%include "src/IOHprofiler_string.hpp"

%include "src/IOHprofiler_transformation.h"
%include "src/suite_bbob_legacy_code.hpp"
%include "src/wmodels.hpp"
%include "src/IOHprofiler_csv_logger.h"
%include "src/IOHprofiler_common.h"
%include "src/IOHprofiler_platform.h"
%include "src/coco_transformation_objs.hpp"
%include "src/coco_transformation_vars.hpp"
%include "src/coco_transformation.hpp"

%include "src/f_attractive_sector.hpp"
%include "src/f_bent_cigar.hpp"
%include "src/f_bueche_rastrigin.hpp"
%include "src/f_different_powers.hpp"
%include "src/f_discus.hpp"
%include "src/f_ellipsoid.hpp"
%include "src/f_ellipsoid_rotated.hpp"
%include "src/f_gallagher101.hpp"
%include "src/f_gallagher21.hpp"
%include "src/f_griewank_rosenbrock.hpp"
%include "src/f_rastrigin.hpp"
%include "src/f_rastrigin_rotated.hpp"
%include "src/f_rosenbrock.hpp"
%include "src/f_rosenbrock_rotated.hpp"
%include "src/f_schaffers1000.hpp"
%include "src/f_schaffers10.hpp"
%include "src/f_schwefel.hpp"
%include "src/f_sharp_ridge.hpp"
%include "src/f_sphere.hpp"
%include "src/f_step_ellipsoid.hpp"
%include "src/f_weierstrass.hpp"
%include "src/f_linear_slope.hpp"
%include "src/f_lunacek_bi_rastrigin.hpp"
%include "src/f_katsuura.hpp"
%include "src/f_ising_ring.hpp"
%include "src/f_ising_torus.hpp"
%include "src/f_ising_triangular.hpp"
%include "src/f_labs.hpp"
%include "src/f_leading_ones_dummy1.hpp"
%include "src/f_leading_ones_dummy2.hpp"
%include "src/f_leading_ones_epistasis.hpp"
%include "src/f_leading_ones.hpp"
%include "src/f_leading_ones_neutrality.hpp"
%include "src/f_leading_ones_ruggedness1.hpp"
%include "src/f_leading_ones_ruggedness2.hpp"
%include "src/f_leading_ones_ruggedness3.hpp"
%include "src/f_linear.hpp"
%include "src/f_MIS.hpp"
%include "src/f_N_queens.hpp"
%include "src/f_one_max_dummy1.hpp"
%include "src/f_one_max_dummy2.hpp"
%include "src/f_one_max_epistasis.hpp"
%include "src/f_one_max.hpp"
%include "src/f_one_max_neutrality.hpp"
%include "src/f_one_max_ruggedness1.hpp"
%include "src/f_one_max_ruggedness2.hpp"
%include "src/f_one_max_ruggedness3.hpp"
%include "src/f_w_model_one_max.hpp"
%include "src/f_w_model_leading_ones.hpp"

%include "src/IOHprofiler_class_generator.h"


%template (genericGenerator_int_problem) genericGenerator<IOHprofiler_problem<int> >;
%template (genericGenerator_double_problem) genericGenerator<IOHprofiler_problem<double> >;
%include "src/IOHprofiler_suite.h"
%template(IOHprofiler_suite_int) IOHprofiler_suite< int >;
%template(IOHprofiler_suite_double) IOHprofiler_suite< double >;

%include "src/IOHprofiler_BBOB_suite.hpp"
%include "src/IOHprofiler_PBO_suite.hpp"

%apply int{ UInt32 }

%module IOHprofiler
#pragma SWIG nowarn=401
