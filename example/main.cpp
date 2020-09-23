#include <iostream>
#include <iomanip>
#include <numeric>

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

	std::cout << f->get_problem_id() << ": " << f->get_problem_name() << std::endl;
	std::vector<double> x = ioh::common::random::gauss(4, 42);
	print(f->evaluate(x));
	print("***************");
}

void test_eval(std::unique_ptr<ioh::problem::pbo::pbo_base> f)
{
	print(f->get_problem_name());
	std::vector<double> x = ioh::common::random::gauss(4, 42);
	std::vector<bool> xi(x.begin(), x.end());
	std::vector<int> xj(xi.begin(), xi.end());
	vprint(xj);
	print(f->evaluate(xj));
	print("***************");
}

void test_eval(std::unique_ptr<ioh::problem::wmodel::wmodel_base> f)
{
	print(f->get_problem_name());
	f->reset_problem();
	std::vector<double> x = ioh::common::random::gauss(4, 42);
	std::vector<bool> xi(x.begin(), x.end());
	std::vector<int> xj(xi.begin(), xi.end());
	vprint(xj);
	print(f->evaluate(xj));
	print("***************");
}

void test_problems()
{
	using namespace ioh::problem::bbob;
	test_eval(std::make_unique<Attractive_Sector>()); 
	test_eval(std::make_unique<Bent_Cigar>()); // incorrect 43065603.7200
	test_eval(std::make_unique<Bueche_Rastrigin>()); // incorrect  -377.5170
	test_eval(std::make_unique<Different_Powers>());
	test_eval(std::make_unique<Discus>());
	test_eval(std::make_unique<Ellipsoid>());
	test_eval(std::make_unique<Ellipsoid_Rotated>());
	test_eval(std::make_unique<Gallagher101>()); // 100.6898
	test_eval(std::make_unique<Gallagher21>()); // -924.8833
	test_eval(std::make_unique<Griewank_RosenBrock>());
	test_eval(std::make_unique<Katsuura>()); // however I think in old version this is incorrect
	test_eval(std::make_unique<Linear_Slope>());
	test_eval(std::make_unique<Lunacek_Bi_Rastrigin>());
	test_eval(std::make_unique<Rastrigin>());
	test_eval(std::make_unique<Rastrigin_Rotated>());
	test_eval(std::make_unique<Rosenbrock>());
	test_eval(std::make_unique<Rosenbrock_Rotated>());
	test_eval(std::make_unique<Schaffers10>());
	test_eval(std::make_unique<Schaffers1000>()); // incorrect 39.4152
	test_eval(std::make_unique<Schwefel>());
	test_eval(std::make_unique<Sharp_Ridge>());
	test_eval(std::make_unique<Sphere>());
	test_eval(std::make_unique<Step_Ellipsoid>());
	test_eval(std::make_unique<Weierstrass>());

	// // test PBO
	using namespace ioh::problem::pbo;
	// test_eval(std::make_unique<MIS>());
	// test_eval(std::make_unique<NQueens>());
	// test_eval(std::make_unique<Concatenated_Trap>());
	// test_eval(std::make_unique<Ising_Ring>());
	// test_eval(std::make_unique<Ising_Torus>());
	// test_eval(std::make_unique<Ising_Triangular>());
	// test_eval(std::make_unique<LABS>());
	// test_eval(std::make_unique<LeadingOnes>());
	// test_eval(std::make_unique<LeadingOnes_Dummy1>());
	// test_eval(std::make_unique<LeadingOnes_Dummy2>());
	// test_eval(std::make_unique<LeadingOnes_Epistasis>());
	// test_eval(std::make_unique<LeadingOnes_Neutrality>());
	// test_eval(std::make_unique<LeadingOnes_Ruggedness1>());
	// test_eval(std::make_unique<LeadingOnes_Ruggedness2>());
	// test_eval(std::make_unique<LeadingOnes_Ruggedness3>());
	// test_eval(std::make_unique<Linear>());
	// test_eval(std::make_unique<NK_Landscapes>());
	// test_eval(std::make_unique<OneMax>());
	// test_eval(std::make_unique<OneMax_Dummy1>());
	// test_eval(std::make_unique<OneMax_Dummy2>());
	// test_eval(std::make_unique<OneMax_Epistasis>());
	// test_eval(std::make_unique<OneMax_Neutrality>());
	// test_eval(std::make_unique<OneMax_Ruggedness1>());
	// test_eval(std::make_unique<OneMax_Ruggedness2>());
	// test_eval(std::make_unique<OneMax_Ruggedness3>());
	//
	// // test Wmodel
	// using namespace ioh::problem::wmodel;
	// test_eval(std::make_unique<W_Model_LeadingOnes>());
	// test_eval(std::make_unique<W_Model_OneMax>());

	
}

int main()
{
	// ioh::suite::bbob b;
	test_problems();
	// for (int i = 0; i < 100; i++)
	// {
	// 	auto x = ioh::common::random::integers(10);
	// 	vprint(x);
	// }
	
}

