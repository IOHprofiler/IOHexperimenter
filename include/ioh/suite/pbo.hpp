#pragma once
#include "base.hpp"
#include "ioh/problem/pbo.hpp"


namespace ioh
{
	namespace suite
	{
		class pbo : public base<problem::pbo::pbo_base>
		{
		public:
			using input_type = int;
			pbo(std::vector<int> problem_id = vector<int>(0),
			    const std::vector<int> instance_id = vector<int>({IOH_DEFAULT_INSTANCE}),
			    const std::vector<int> dimension = vector<int>({100}))
			{
				if (problem_id.empty())
				{
					common::log::info("PBO: no problem id provided, applying all functions..");
					problem_id.resize(25);
					std::iota(std::begin(problem_id), std::end(problem_id), 1);
				}
				check_parameter_bounds(problem_id, 0, 25);
				check_parameter_bounds(instance_id, 0, 100);
				check_parameter_bounds(dimension, 0, 20000);

				set_suite_problem_id(problem_id);
				set_suite_instance_id(instance_id);
				set_suite_dimension(dimension);
				set_suite_name("PBO");
				pbo::register_problems();
				load_problem();
			}

			void register_problems() override
			{
				using namespace problem::pbo;
				register_problem<pbo_base, OneMax>("OneMax", 1);
				register_problem<pbo_base, LeadingOnes>("LeadingOnes", 2);
				register_problem<pbo_base, Linear>("Linear", 3);
				register_problem<pbo_base, OneMax_Dummy1>("OneMax_Dummy1", 4);
				register_problem<pbo_base, OneMax_Dummy2>("OneMax_Dummy2", 5);
				register_problem<pbo_base, OneMax_Neutrality>("OneMax_Neutrality", 6);
				register_problem<pbo_base, OneMax_Epistasis>("OneMax_Epistasis", 7);
				register_problem<pbo_base, OneMax_Ruggedness1>("OneMax_Ruggedness1", 8);
				register_problem<pbo_base, OneMax_Ruggedness2>("OneMax_Ruggedness2", 9);
				register_problem<pbo_base, OneMax_Ruggedness3>("OneMax_Ruggedness3", 10);
				register_problem<pbo_base, LeadingOnes_Dummy1>("LeadingOnes_Dummy1", 11);
				register_problem<pbo_base, LeadingOnes_Dummy2>("LeadingOnes_Dummy2", 12);
				register_problem<pbo_base, LeadingOnes_Neutrality>("LeadingOnes_Neutrality", 13);
				register_problem<pbo_base, LeadingOnes_Epistasis>("LeadingOnes_Epistasis", 14);
				register_problem<pbo_base, LeadingOnes_Ruggedness1>("LeadingOnes_Ruggedness1", 15);
				register_problem<pbo_base, LeadingOnes_Ruggedness2>("LeadingOnes_Ruggedness2", 16);
				register_problem<pbo_base, LeadingOnes_Ruggedness3>("LeadingOnes_Ruggedness3", 17);
				register_problem<pbo_base, LABS>("LABS", 18);
				register_problem<pbo_base, MIS>("MIS", 22);
				register_problem<pbo_base, Ising_Ring>("Ising_Ring", 19);
				register_problem<pbo_base, Ising_Torus>("Ising_Torus", 20);
				register_problem<pbo_base, Ising_Triangular>("Ising_Triangular", 21);
				register_problem<pbo_base, NQueens>("NQueens", 23);
				register_problem<pbo_base, Concatenated_Trap>("Concatenated_Trap", 24);
				register_problem<pbo_base, NK_Landscapes>("NK_Landscapes", 25);
			}

			static pbo* create(std::vector<int> problem_id = vector<int>(0),
			                   const std::vector<int> instance_id = vector<int>({IOH_DEFAULT_INSTANCE}),
			                   const std::vector<int> dimension = vector<int>({100}))
			{
				return new pbo(problem_id, instance_id, dimension);
			}
		};
	}
}
