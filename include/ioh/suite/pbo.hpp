#pragma once
#include "base.hpp"
#include "ioh/problem/pbo.hpp"


namespace ioh
{
	namespace suite
	{
        class pbo : public base<problem::pbo::pbo_base> {
        public:
            pbo() {
                std::vector<int> problem_id(23);
                std::iota(std::begin(problem_id), std::end(problem_id), 1);
                set_suite_problem_id(problem_id);
                set_suite_instance_id({DEFAULT_INSTANCE});
                set_suite_dimension({100});
                set_suite_name("PBO");
                registerProblem();
                loadProblem();
            }


            pbo(std::vector<int> problem_id, std::vector<int> instance_id, std::vector<int> dimension) {
                for (int i = 0; i < problem_id.size(); ++i) {
                    if (problem_id[i] < 0 || problem_id[i] > 23) {
                        common::log::error("problem_id " + std::to_string(problem_id[i]) + " is not in PBO_suite");
                    }
                }

                for (int i = 0; i < instance_id.size(); ++i) {
                    if (instance_id[i] < 0 || instance_id[i] > 100) {
                        common::log::error("instance_id " + std::to_string(instance_id[i]) + " is not in PBO_suite");
                    }
                }

                for (int i = 0; i < dimension.size(); ++i) {
                    if (dimension[i] < 0 || dimension[i] > 20000) {
                        common::log::error("dimension " + std::to_string(dimension[i]) + " is not in PBO_suite");
                    }
                }

                set_suite_problem_id(problem_id);
                set_suite_instance_id(instance_id);
                set_suite_dimension(dimension);
                set_suite_name("PBO");
                registerProblem();
            }

            /// \fn void registerProblem()
            /// \brief Implementation of virtual function of base class IOHprofiler_suite.
            /// 
            /// 23 functions are included in the PBO_suite. 
            void registerProblem() {
                using namespace common;
                using namespace problem::pbo;
            	registerInFactory<pbo_base, OneMax> regOneMax("OneMax");
                registerInFactory<pbo_base, OneMax_Dummy1> regOneMax_Dummy1("OneMax_Dummy1");
                registerInFactory<pbo_base, OneMax_Dummy2> regOneMax_Dummy2("OneMax_Dummy2");
                registerInFactory<pbo_base, OneMax_Epistasis> regOneMax_Epistasis("OneMax_Epistasis");
                registerInFactory<pbo_base, OneMax_Neutrality> regOneMax_Neutrality("OneMax_Neutrality");
                registerInFactory<pbo_base, OneMax_Ruggedness1> regOneMax_Ruggedness1("OneMax_Ruggedness1");
                registerInFactory<pbo_base, OneMax_Ruggedness2> regOneMax_Ruggedness2("OneMax_Ruggedness2");
                registerInFactory<pbo_base, OneMax_Ruggedness3> regOneMax_Ruggedness3("OneMax_Ruggedness3");

                registerInFactory<pbo_base, LeadingOnes> regLeadingOnes("LeadingOnes");
                registerInFactory<pbo_base, LeadingOnes_Dummy1> regLeadingOnes_Dummy1("LeadingOnes_Dummy1");
                registerInFactory<pbo_base, LeadingOnes_Dummy2> regLeadingOnes_Dummy2("LeadingOnes_Dummy2");
                registerInFactory<pbo_base, LeadingOnes_Epistasis> regLeadingOnes_Epistasis("LeadingOnes_Epistasis");
                registerInFactory<pbo_base, LeadingOnes_Neutrality> regLeadingOnes_Neutrality("LeadingOnes_Neutrality");
                registerInFactory<pbo_base, LeadingOnes_Ruggedness1> regLeadingOnes_Ruggedness1("LeadingOnes_Ruggedness1");
                registerInFactory<pbo_base, LeadingOnes_Ruggedness2> regLeadingOnes_Ruggedness2("LeadingOnes_Ruggedness2");
                registerInFactory<pbo_base, LeadingOnes_Ruggedness3> regLeadingOnes_Ruggedness3("LeadingOnes_Ruggedness3");

                registerInFactory<pbo_base, Linear> regLinear("Linear");
                registerInFactory<pbo_base, MIS> regMIS("MIS");
                registerInFactory<pbo_base, LABS> regLABS("LABS");
                registerInFactory<pbo_base, NQueens> regNQueens("NQueens");
                registerInFactory<pbo_base, Ising_Ring> regIsing_Ring("Ising_Ring");
                registerInFactory<pbo_base, Ising_Torus> regIsing_Torus("Ising_Torus");
                registerInFactory<pbo_base, Ising_Triangular> regIsing_Triangular("Ising_Triangular");

                mapIDTOName(1, "OneMax");
                mapIDTOName(2, "LeadingOnes");
                mapIDTOName(3, "Linear");
                mapIDTOName(4, "OneMax_Dummy1");
                mapIDTOName(5, "OneMax_Dummy2");
                mapIDTOName(6, "OneMax_Neutrality");
                mapIDTOName(7, "OneMax_Epistasis");
                mapIDTOName(8, "OneMax_Ruggedness1");
                mapIDTOName(9, "OneMax_Ruggedness2");
                mapIDTOName(10, "OneMax_Ruggedness3");
                mapIDTOName(11, "LeadingOnes_Dummy1");
                mapIDTOName(12, "LeadingOnes_Dummy2");
                mapIDTOName(13, "LeadingOnes_Neutrality");
                mapIDTOName(14, "LeadingOnes_Epistasis");
                mapIDTOName(15, "LeadingOnes_Ruggedness1");
                mapIDTOName(16, "LeadingOnes_Ruggedness2");
                mapIDTOName(17, "LeadingOnes_Ruggedness3");
                mapIDTOName(18, "LABS");
                mapIDTOName(22, "MIS");
                mapIDTOName(19, "Ising_Ring");
                mapIDTOName(20, "Ising_Torus");
                mapIDTOName(21, "Ising_Triangular");
                mapIDTOName(23, "NQueens");
            }

            static pbo* createInstance() {
                return new pbo();
            }
            
            static pbo* createInstance(std::vector<int> problem_id, std::vector<int> instance_id, std::vector<int> dimension) {
                return new pbo(problem_id, instance_id, dimension);
            }

		};
	}
}