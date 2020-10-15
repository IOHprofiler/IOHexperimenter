#pragma once
#include "base.hpp"
#include "ioh/problem/bbob.hpp"


namespace ioh
{
	namespace suite
	{
		class bbob : public base<problem::bbob::bbob_base>
		{
		public:
			using input_type = double;
			bbob(std::vector<int> problem_id = vector<int>(0),
			     const std::vector<int> instance_id = vector<int>({DEFAULT_INSTANCE}),
			     const std::vector<int> dimension = vector<int>({DEFAULT_DIMENSION}))
			{
				if (problem_id.empty())
				{
					common::log::info("BBOB: no problem id provided, applying all functions..");
					problem_id.resize(24);
					std::iota(std::begin(problem_id), std::end(problem_id), 1);
				}
				check_parameter_bounds(problem_id, 0, 24);
				check_parameter_bounds(instance_id, 0, 100);
				check_parameter_bounds(dimension, 0, 100);

				set_suite_problem_id(problem_id);
				set_suite_instance_id(instance_id);
				set_suite_dimension(dimension);
				set_suite_name("BBOB");
				bbob::register_problems();
				loadProblem();
			}

			void register_problems() override
			{
				using namespace problem::bbob;
				register_problem<bbob_base, Sphere>("Sphere", 1);
				register_problem<bbob_base, Ellipsoid>("Ellipsoid", 2);
				register_problem<bbob_base, Rastrigin>("Rastrigin", 3);
				register_problem<bbob_base, Bueche_Rastrigin>("Bueche_Rastrigin", 4);
				register_problem<bbob_base, Linear_Slope>("Linear_Slope", 5);
				register_problem<bbob_base, Attractive_Sector>("Attractive_Sector", 6);
				register_problem<bbob_base, Step_Ellipsoid>("Step_Ellipsoid", 7);
				register_problem<bbob_base, Rosenbrock>("Rosenbrock", 8);
				register_problem<bbob_base, Rosenbrock_Rotated>("Rosenbrock_Rotated", 9);
				register_problem<bbob_base, Ellipsoid_Rotated>("Ellipsoid_Rotated", 10);
				register_problem<bbob_base, Discus>("Discus", 11);
				register_problem<bbob_base, Bent_Cigar>("Bent_Cigar", 12);
				register_problem<bbob_base, Sharp_Ridge>("Sharp_Ridge", 13);
				register_problem<bbob_base, Different_Powers>("Different_Powers", 14);
				register_problem<bbob_base, Rastrigin_Rotated>("Rastrigin_Rotated", 15);
				register_problem<bbob_base, Weierstrass>("Weierstrass", 16);
				register_problem<bbob_base, Schaffers10>("Schaffers10", 17);
				register_problem<bbob_base, Schaffers1000>("Schaffers1000", 18);
				register_problem<bbob_base, Griewank_RosenBrock>("Griewank_RosenBrock", 19);
				register_problem<bbob_base, Schwefel>("Schwefel", 20);
				register_problem<bbob_base, Gallagher101>("Gallagher101", 21);
				register_problem<bbob_base, Gallagher21>("Gallagher21", 22);
				register_problem<bbob_base, Katsuura>("Katsuura", 23);
				register_problem<bbob_base, Lunacek_Bi_Rastrigin>("Lunacek_Bi_Rastrigin", 24);
			}

			static bbob* create(const std::vector<int> problem_id = vector<int>(0),
			                    const std::vector<int> instance_id = vector<int>({DEFAULT_INSTANCE}),
			                    const std::vector<int> dimension = vector<int>({DEFAULT_DIMENSION}))
			{
				return new bbob(problem_id, instance_id, dimension);
			}
		};
	}
}
