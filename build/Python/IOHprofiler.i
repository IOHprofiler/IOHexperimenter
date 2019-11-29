%module IOHprofiler
%{
#include "src/coco_transformation_objs.hpp"
#include "src/coco_transformation_vars.hpp"
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
#include "src/f_ising_1D.hpp"
#include "src/f_ising_2D.hpp"
#include "src/f_ising_triangle.hpp"
#include "src/f_katsuura.hpp"
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
#include "src/f_linear_slope.hpp"
#include "src/f_lunacek_bi_rastrigin.hpp"
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
#include "src/IOHprofiler_all_suites.hpp"
#include "src/IOHprofiler_BBOB_suite.hpp"
#include "src/IOHprofiler_class_generator.hpp"
#include "src/IOHprofiler_all_problems.hpp"
#include "src/IOHprofiler_configuration.hpp"
#include "src/IOHprofiler_experimenter.hpp"
#include "src/IOHprofiler_observer.hpp"
#include "src/IOHprofiler_PBO_suite.hpp"
#include "src/IOHprofiler_string.hpp"
#include "src/IOHprofiler_suite.hpp"
#include "src/IOHprofiler_transformation.hpp"
#include "src/suite_bbob_legacy_code.hpp"
#include "src/wmodels.hpp"
#include "src/coco_transformation.h"
#include "src/IOHprofiler_csv_logger.h"
#include "src/IOHprofiler_common.h"
#include "src/IOHprofiler_platform.h"

typedef unsigned int		uint32_t;
typedef unsigned long int	uint64_t;
typedef  std::map<std::string, int> PROBLEM_NAME_ID; 
typedef  std::map<int, std::string> PROBLEM_ID_NAME; 

%}



namespace boost {
    namespace filesystem {
    }
}

typedef  std::map<std::string, int> PROBLEM_NAME_ID; 
typedef  std::map<int, std::string> PROBLEM_ID_NAME; 



typedef unsigned int uint32_t;

%include <std_shared_ptr.i>
%include <std_string.i>
%include <std_map.i>
%include <stdint.i>
%include "src/coco_transformation_objs.hpp"
%include "src/coco_transformation_vars.hpp"
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
%include "src/f_ising_1D.hpp"
%include "src/f_ising_2D.hpp"
%include "src/f_ising_triangle.hpp"
%include "src/f_katsuura.hpp"
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
%include "src/f_linear_slope.hpp"
%include "src/f_lunacek_bi_rastrigin.hpp"
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
%include "src/IOHprofiler_all_problems.hpp"
%include "src/IOHprofiler_all_suites.hpp"
%include "src/IOHprofiler_BBOB_suite.hpp"
%include "src/IOHprofiler_class_generator.hpp"
%include "src/IOHprofiler_configuration.hpp"
%include "src/IOHprofiler_experimenter.hpp"
%include "src/IOHprofiler_observer.hpp"
%include "src/IOHprofiler_PBO_suite.hpp"
%include "src/IOHprofiler_problem.hpp"
%include "src/IOHprofiler_random.hpp"
%include "src/IOHprofiler_string.hpp"
%include "src/IOHprofiler_suite.hpp"
%include "src/IOHprofiler_transformation.hpp"
%include "src/suite_bbob_legacy_code.hpp"
%include "src/wmodels.hpp"
%include "src/coco_transformation.h"
%include "src/IOHprofiler_csv_logger.h"
%include "src/IOHprofiler_common.h"
%include "src/IOHprofiler_platform.h"


%shared_ptr(std::vector<uint8_t>)
%shared_ptr(std::vector<double>)
%shared_ptr(std::vector<int>)
%shared_ptr(IOHprofiler_problem)
%shared_ptr(IOHprofiler_problem< double >)
%shared_ptr(IOHprofiler_problem< int >)
%shared_ptr(IOHprofiler_suite< double >)
%shared_ptr(IOHprofiler_suite< int >)

%apply int{ UInt32 }
%template(IOHprofiler_Problem_int) IOHprofiler_problem< int >;
%template(IOHprofiler_Problem_double) IOHprofiler_problem< double >;
%template(IOHprofiler_suite_int) IOHprofiler_suite< int >;
%template(IOHprofiler_suite_double) IOHprofiler_suite< double >;
%template (genericGenerator_int_problem) genericGenerator<IOHprofiler_problem<int> >;
%template (genericGenerator_double_problem) genericGenerator<IOHprofiler_problem<double> >;

%template (registerInFactory_onemax) registerInFactory<IOHprofiler_problem< int >, OneMax>;



%module IOHprofiler
#pragma SWIG nowarn=401