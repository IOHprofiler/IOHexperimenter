#pragma once
#include <numeric>

#include "base.hpp"
#include "ioh/problem/bbob.hpp"


namespace ioh
{
	namespace suite
	{
		class bbob : public base<double>
		{
		public:
			bbob()
			{
				std::vector<int> problem_id(24);
				std::iota(std::begin(problem_id), std::end(problem_id), 1);
				std::cout << "1 " << std::endl;
				set_suite_problem_id(problem_id);
				std::cout << "2 " << std::endl;
				set_suite_instance_id({ 1 });
				std::cout << "3 " << std::endl;
				set_suite_dimension({ 5 });
				std::cout << "4 " << std::endl;
				set_suite_name("BBOB");
				std::cout << "5 " << std::endl;
				registerProblem();
				std::cout << "6 " << std::endl;
				// Load problem, so that the user don't have to think about it.
				this->loadProblem();
				std::cout << "7 " << std::endl;
			}
			
			bbob(std::vector<int> problem_id, std::vector<int> instance_id, std::vector<int> dimension) {
				for (std::size_t i = 0; i < problem_id.size(); ++i) {
					if (problem_id[i] < 0 || problem_id[i] > 24) {
						common::log::error("problem_id " + std::to_string(problem_id[i]) + " is not in BBOB_suite");
					}
				}

				for (std::size_t i = 0; i < instance_id.size(); ++i) {
					if (instance_id[i] < 0 || instance_id[i] > 100) {
						common::log::error("instance_id " + std::to_string(instance_id[i]) + " is not in BBOB_suite");
					}
				}

				for (std::size_t i = 0; i < dimension.size(); ++i) {
					if (dimension[i] < 0 || dimension[i] > 100) {
						common::log::error("dimension " + std::to_string(dimension[i]) + " is not in BBOB_suite");
					}
				}

				set_suite_problem_id(problem_id);
				set_suite_instance_id(instance_id);
				set_suite_dimension(dimension);
				set_suite_name("BBOB");
				registerProblem();
				this->loadProblem();
			}
			

			void registerProblem()
			{
				using namespace common;
				using namespace problem::bbob;
				registerInFactory<bbob_base, Sphere> regSphere("Sphere");
				registerInFactory<bbob_base, Ellipsoid> regEllipsoid("Ellipsoid");
				registerInFactory<bbob_base, Rastrigin> regRastrigin("Rastrigin");
				registerInFactory<bbob_base, Bueche_Rastrigin> regBueche_Rastrigin("Bueche_Rastrigin");
				registerInFactory<bbob_base, Linear_Slope> regLinear_Slope("Linear_Slope");
				registerInFactory<bbob_base, Attractive_Sector> regAttractive_Sector("Attractive_Sector");
				registerInFactory<bbob_base, Step_Ellipsoid> regStep_Ellipsoid("Step_Ellipsoid");
				registerInFactory<bbob_base, Rosenbrock> regRosenbrock("Rosenbrock");
				registerInFactory<bbob_base, Rosenbrock_Rotated>
					regRosenbrock_Rotated("Rosenbrock_Rotated");
				registerInFactory<bbob_base, Ellipsoid_Rotated> regEllipsoid_Rotated("Ellipsoid_Rotated");
				registerInFactory<bbob_base, Discus> regDiscus("Discus");
				registerInFactory<bbob_base, Bent_Cigar> regBent_Ciger("Bent_Cigar");
				registerInFactory<bbob_base, Sharp_Ridge> regSharp_Ridge("Sharp_Ridge");
				registerInFactory<bbob_base, Different_Powers> regDifferent_Powers("Different_Powers");
				registerInFactory<bbob_base, Rastrigin_Rotated> regRastrigin_Rotated("Rastrigin_Rotated");
				registerInFactory<bbob_base, Weierstrass> regWeierstrass("Weierstrass");
				registerInFactory<bbob_base, Schaffers10> regSchaffers10("Schaffers10");
				registerInFactory<bbob_base, Schaffers1000> regSchaffers1000("Schaffers1000");
				registerInFactory<bbob_base, Griewank_RosenBrock> regGriewank_RosenBrock(
					"Griewank_RosenBrock");
				registerInFactory<bbob_base, Schwefel> regSchwefel("Schwefel");
				registerInFactory<bbob_base, Gallagher101> regGallagher101("Gallagher101");
				registerInFactory<bbob_base, Gallagher21> regGallagher21("Gallagher21");
				registerInFactory<bbob_base, Katsuura> regKatsuura("Katsuura");
				registerInFactory<bbob_base, Lunacek_Bi_Rastrigin> regLunacek_Bi_Rastrigin(
					"Lunacek_Bi_Rastrigin");


				mapIDTOName(1, "Sphere");
				mapIDTOName(2, "Ellipsoid");
				mapIDTOName(3, "Rastrigin");
				mapIDTOName(4, "Bueche_Rastrigin");
				mapIDTOName(5, "Linear_Slope");
				mapIDTOName(6, "Attractive_Sector");
				mapIDTOName(7, "Step_Ellipsoid");
				mapIDTOName(8, "Rosenbrock");
				mapIDTOName(9, "Rosenbrock_Rotated");
				mapIDTOName(10, "Ellipsoid_Rotated");
				mapIDTOName(11, "Discus");
				mapIDTOName(12, "Bent_Cigar");
				mapIDTOName(13, "Sharp_Ridge");
				mapIDTOName(14, "Different_Powers");
				mapIDTOName(15, "Rastrigin_Rotated");
				mapIDTOName(16, "Weierstrass");
				mapIDTOName(17, "Schaffers10");
				mapIDTOName(18, "Schaffers1000");
				mapIDTOName(19, "Griewank_RosenBrock");
				mapIDTOName(20, "Schwefel");
				mapIDTOName(21, "Gallagher101");
				mapIDTOName(22, "Gallagher21");
				mapIDTOName(23, "Katsuura");
				mapIDTOName(24, "Lunacek_Bi_Rastrigin");
			}
			static bbob* createInstance() {
				return new bbob();
			};

			static bbob* createInstance(std::vector<int> problem_id, std::vector<int> instance_id, std::vector<int> dimension) {
				return new bbob(problem_id, instance_id, dimension);
			};
			
		};
	}
}
