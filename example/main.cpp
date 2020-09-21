#include <iostream>
#include <iomanip>

#include "ioh.hpp"

#define print(x) std::cout << std::fixed << std::setprecision(4) << x << std::endl

template<typename T>
void vprint(std::vector<T> v)
{
	int i = 0;
	std::cout << "[ ";
	for (auto& e : v)
		std::cout << e << ", ";
	std::cout << " ]" << std::endl;
}


void test_eval(std::unique_ptr<ioh::problem::bbob::bbob_base> f)
{
	print(f->get_problem_name());
	std::vector<double> x = ioh::common::random::gauss(4, 42);
	vprint(x);
	print(f->evaluate(x));
	print("***************");
}

int main()
{
	// ioh::transformation::coco::data data;
	using namespace ioh::problem::bbob;
	test_eval(std::make_unique<Attractive_Sector>());
	test_eval(std::make_unique<Bent_Cigar>());
	test_eval(std::make_unique<Bueche_Rastrigin>());
	test_eval(std::make_unique<Different_Powers>());
	test_eval(std::make_unique<Discus>());
	test_eval(std::make_unique<Ellipsoid>());
	test_eval(std::make_unique<Ellipsoid_Rotated>());
	test_eval(std::make_unique<Gallagher101>());
	test_eval(std::make_unique<Gallagher21>());
	test_eval(std::make_unique<Griewank_RosenBrock>());
	test_eval(std::make_unique<Katsuura>());
	test_eval(std::make_unique<Linear_Slope>());
	test_eval(std::make_unique<Lunacek_Bi_Rastrigin>());
	test_eval(std::make_unique<Rastrigin>());
	test_eval(std::make_unique<Rastrigin_Rotated>());
	test_eval(std::make_unique<Rosenbrock>());
	test_eval(std::make_unique<Rosenbrock_Rotated>());
	test_eval(std::make_unique<Schaffers10>());
	test_eval(std::make_unique<Schaffers1000>());
	test_eval(std::make_unique<Schwefel>());
	test_eval(std::make_unique<Sharp_Ridge>());
	test_eval(std::make_unique<Sphere>());
	test_eval(std::make_unique<Step_Ellipsoid>());
	test_eval(std::make_unique<Weierstrass>());
}
