/// \file IOHprofiler_BBOB_suite.cpp
/// \brief cpp file for class IOHprofiler_BBOB_suite.
///
/// A suite of bbob problems (24 problems, 1-100 instances, and dimension <= 100).
///
/// \author Furong Ye
/// \date 2019-09-12
#ifndef _IOHPROFILER_BBOB_SUITE_HPP
#define _IOHPROFILER_BBOB_SUITE_HPP

#include "../Problems/BBOB/f_attractive_sector.hpp"
#include "../Problems/BBOB/f_bent_cigar.hpp"
#include "../Problems/BBOB/f_bueche_rastrigin.hpp"
#include "../Problems/BBOB/f_different_powers.hpp"
#include "../Problems/BBOB/f_discus.hpp"
#include "../Problems/BBOB/f_ellipsoid.hpp"
#include "../Problems/BBOB/f_ellipsoid_rotated.hpp"
#include "../Problems/BBOB/f_gallagher21.hpp"
#include "../Problems/BBOB/f_gallagher101.hpp"
#include "../Problems/BBOB/f_griewank_rosenbrock.hpp"
#include "../Problems/BBOB/f_katsuura.hpp"
#include "../Problems/BBOB/f_linear_slope.hpp"
#include "../Problems/BBOB/f_lunacek_bi_rastrigin.hpp"
#include "../Problems/BBOB/f_rastrigin.hpp"
#include "../Problems/BBOB/f_rastrigin_rotated.hpp"
#include "../Problems/BBOB/f_rosenbrock.hpp"
#include "../Problems/BBOB/f_rosenbrock_rotated.hpp"
#include "../Problems/BBOB/f_schaffers10.hpp"
#include "../Problems/BBOB/f_schaffers1000.hpp"
#include "../Problems/BBOB/f_schwefel.hpp"
#include "../Problems/BBOB/f_sharp_ridge.hpp"
#include "../Problems/BBOB/f_sphere.hpp"
#include "../Problems/BBOB/f_step_ellipsoid.hpp"
#include "../Problems/BBOB/f_weierstrass.hpp"

#include "../Template/IOHprofiler_suite.hpp"

class BBOB_suite : public IOHprofiler_suite<double> {
public:
  BBOB_suite() {
    std::vector<int> problem_id;
    std::vector<int> instance_id;
    std::vector<int> dimension;
    for (int i = 0; i < 24; ++i) {
      problem_id.push_back(i+1);
    }
    for (int i = 0; i < 1; ++i) {
      instance_id.push_back(i+1);
    }
    dimension.push_back(5);
    
    IOHprofiler_set_suite_problem_id(problem_id);
    IOHprofiler_set_suite_instance_id(instance_id);
    IOHprofiler_set_suite_dimension(dimension);
    IOHprofiler_set_suite_name("BBOB");
    registerProblem();
  };

  BBOB_suite(std::vector<int> problem_id, std::vector<int> instance_id, std::vector<int> dimension) {
    for (int i = 0; i < problem_id.size(); ++i) {
      if (problem_id[i] < 0 || problem_id[i] > 24) {
        IOH_error("problem_id " + std::to_string(problem_id[i]) + " is not in BBOB_suite");
      }
    }
    
    for (int i = 0; i < instance_id.size(); ++i) {
      if (instance_id[i] < 0 || instance_id[i] > 100) {
        IOH_error("instance_id " + std::to_string(instance_id[i]) + " is not in BBOB_suite");
      }
    }

    for (int i = 0; i < dimension.size(); ++i) {
      if (dimension[i] < 0 || dimension[i] > 100) {
        IOH_error("dimension " + std::to_string(dimension[i]) + " is not in BBOB_suite");
      }
    }

    IOHprofiler_set_suite_problem_id(problem_id);
    IOHprofiler_set_suite_instance_id(instance_id);
    IOHprofiler_set_suite_dimension(dimension);
    IOHprofiler_set_suite_name("BBOB");
    registerProblem();
  }

  /// \fn void registerProblem()
  /// \brief Implementation of virtual function of base class IOHprofiler_suite.
  /// 
  /// 23 functions are included in the BBOB_suite. 
  void registerProblem() {
    registerInFactory<IOHprofiler_problem<double>,Sphere> regSphere("Sphere");
    registerInFactory<IOHprofiler_problem<double>,Ellipsoid> regEllipsoid("Ellipsoid");
    registerInFactory<IOHprofiler_problem<double>,Rastrigin> regRastrigin("Rastrigin");
    registerInFactory<IOHprofiler_problem<double>,Bueche_Rastrigin> regBueche_Rastrigin("Bueche_Rastrigin");
    registerInFactory<IOHprofiler_problem<double>,Linear_Slope> regLinear_Slope("Linear_Slope");
    registerInFactory<IOHprofiler_problem<double>,Attractive_Sector> regAttractive_Sector("Attractive_Sector");
    registerInFactory<IOHprofiler_problem<double>,Step_Ellipsoid> regStep_Ellipsoid("Step_Ellipsoid");
    registerInFactory<IOHprofiler_problem<double>,Rosenbrock> regRosenbrock("Rosenbrock");
    registerInFactory<IOHprofiler_problem<double>,Rosenbrock_Rotated> regRosenbrock_Rotated("Rosenbrock_Rotated");
    registerInFactory<IOHprofiler_problem<double>,Ellipsoid_Rotated> regEllipsoid_Rotated("Ellipsoid_Rotated");
    registerInFactory<IOHprofiler_problem<double>,Discus> regDiscus("Discus");
    registerInFactory<IOHprofiler_problem<double>,Bent_Ciger> regBent_Ciger("Bent_Ciger");
    registerInFactory<IOHprofiler_problem<double>,Sharp_Ridge> regSharp_Ridge("Sharp_Ridge");
    registerInFactory<IOHprofiler_problem<double>,Different_Powers> regDifferent_Powers("Different_Powers");
    registerInFactory<IOHprofiler_problem<double>,Rastrigin_Rotated> regRastrigin_Rotated("Rastrigin_Rotated");
    registerInFactory<IOHprofiler_problem<double>,Weierstrass> regWeierstrass("Weierstrass");
    registerInFactory<IOHprofiler_problem<double>,Schaffers10> regSchaffers10("Schaffers10");
    registerInFactory<IOHprofiler_problem<double>,Schaffers1000> regSchaffers1000("Schaffers1000");
    registerInFactory<IOHprofiler_problem<double>,Griewank_RosenBrock> regGriewank_RosenBrock("Griewank_RosenBrock");
    registerInFactory<IOHprofiler_problem<double>,Schwefel> regSchwefel("Schwefel");
    registerInFactory<IOHprofiler_problem<double>,Gallagher101> regGallagher101("Gallagher101");
    registerInFactory<IOHprofiler_problem<double>,Gallagher21> regGallagher21("Gallagher21");
    registerInFactory<IOHprofiler_problem<double>,Katsuura> regKatsuura("Katsuura");
    registerInFactory<IOHprofiler_problem<double>,Lunacek_Bi_Rastrigin> regLunacek_Bi_Rastrigin("Lunacek_Bi_Rastrigin");
    
  
    mapIDTOName(1,"Sphere");
    mapIDTOName(2,"Ellipsoid");
    mapIDTOName(3,"Rastrigin");
    mapIDTOName(4,"Bueche_Rastrigin");
    mapIDTOName(5,"Linear_Slope");
    mapIDTOName(6,"Attractive_Sector");
    mapIDTOName(7,"Step_Ellipsoid");
    mapIDTOName(8,"Rosenbrock");
    mapIDTOName(9,"Rosenbrock_Rotated");
    mapIDTOName(10,"Ellipsoid_Rotated");
    mapIDTOName(11,"Discus");
    mapIDTOName(12,"Bent_Ciger");
    mapIDTOName(13,"Sharp_Ridge");
    mapIDTOName(14,"Different_Powers");
    mapIDTOName(15,"Rastrigin_Rotated");
    mapIDTOName(16,"Weierstrass");
    mapIDTOName(17,"Schaffers10");
    mapIDTOName(18,"Schaffers1000");
    mapIDTOName(19,"Griewank_RosenBrock");
    mapIDTOName(20,"Schwefel");
    mapIDTOName(21,"Gallagher101");
    mapIDTOName(22,"Gallagher21");
    mapIDTOName(23,"Katsuura");
    mapIDTOName(24,"Lunacek_Bi_Rastrigin");
  };

  static BBOB_suite * createInstance() {
    return new BBOB_suite();
  };

  static BBOB_suite * createInstance(std::vector<int> problem_id, std::vector<int> instance_id, std::vector<int> dimension) {
    return new BBOB_suite(problem_id, instance_id, dimension);
  };
};

#endif