#include <iostream>
#include <iomanip>
#include <numeric>

#include "ioh.hpp"
#include "ioh/common/random.hpp"

#define print(x) std::cout << std::fixed << std::setprecision(4) << x << std::endl

auto rng = ioh::common::random(42);

template <typename T>
void vprint(std::vector<T> v)
{
	int i = 0;
	std::cout << "[ ";
	for (auto& e : v)
		std::cout << e << ", ";
	std::cout << " ]" << std::endl;
}

void test_eval(std::shared_ptr<ioh::problem::bbob::bbob_base> f)
{
	std::cout << f->get_problem_id() << ": " << f->get_problem_name() << std::endl;
	std::vector<double> x = ioh::common::random::gauss(4, 42);
	print(f->evaluate(x));
	print("***************");
}

void test_eval(std::shared_ptr<ioh::problem::pbo::pbo_base> f)
{
	print(f->get_problem_name());
	std::vector<int> xj = ioh::common::random::bitstring(100);
	print(f->evaluate(xj));
	print("***************");
}

void test_eval(std::shared_ptr<ioh::problem::wmodel::wmodel_base> f)
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
	test_eval(std::make_shared<Attractive_Sector>());
	test_eval(std::make_shared<Bent_Cigar>()); // incorrect 43065603.7200
	test_eval(std::make_shared<Bueche_Rastrigin>()); // incorrect  -377.5170
	test_eval(std::make_shared<Different_Powers>());
	test_eval(std::make_shared<Discus>());
	test_eval(std::make_shared<Ellipsoid>());
	test_eval(std::make_shared<Ellipsoid_Rotated>());
	test_eval(std::make_shared<Gallagher101>()); // 100.6898
	test_eval(std::make_shared<Gallagher21>()); // -924.8833
	test_eval(std::make_shared<Griewank_RosenBrock>());
	test_eval(std::make_shared<Katsuura>()); // however I think in old version this is incorrect
	test_eval(std::make_shared<Linear_Slope>());
	test_eval(std::make_shared<Lunacek_Bi_Rastrigin>());
	test_eval(std::make_shared<Rastrigin>());
	test_eval(std::make_shared<Rastrigin_Rotated>());
	test_eval(std::make_shared<Rosenbrock>());
	test_eval(std::make_shared<Rosenbrock_Rotated>());
	test_eval(std::make_shared<Schaffers10>());
	test_eval(std::make_shared<Schaffers1000>()); // incorrect 39.4152
	test_eval(std::make_shared<Schwefel>());
	test_eval(std::make_shared<Sharp_Ridge>());
	test_eval(std::make_shared<Sphere>());
	test_eval(std::make_shared<Step_Ellipsoid>());
	test_eval(std::make_shared<Weierstrass>());

	// // test PBO
	using namespace ioh::problem::pbo;
	// test_eval(std::make_shared<MIS>());
	// test_eval(std::make_shared<NQueens>());
	// test_eval(std::make_shared<Concatenated_Trap>());
	// test_eval(std::make_shared<Ising_Ring>());
	// test_eval(std::make_shared<Ising_Torus>());
	// test_eval(std::make_shared<Ising_Triangular>());
	// test_eval(std::make_shared<LABS>());
	// test_eval(std::make_shared<LeadingOnes>());
	// test_eval(std::make_shared<LeadingOnes_Dummy1>());
	// test_eval(std::make_shared<LeadingOnes_Dummy2>());
	// test_eval(std::make_shared<LeadingOnes_Epistasis>());
	// test_eval(std::make_shared<LeadingOnes_Neutrality>());
	// test_eval(std::make_shared<LeadingOnes_Ruggedness1>());
	// test_eval(std::make_shared<LeadingOnes_Ruggedness2>());
	// test_eval(std::make_shared<LeadingOnes_Ruggedness3>());
	// test_eval(std::make_shared<Linear>());
	// test_eval(std::make_shared<NK_Landscapes>());
	// test_eval(std::make_shared<OneMax>());
	// test_eval(std::make_shared<OneMax_Dummy1>());
	// test_eval(std::make_shared<OneMax_Dummy2>());
	// test_eval(std::make_shared<OneMax_Epistasis>());
	// test_eval(std::make_shared<OneMax_Neutrality>());
	// test_eval(std::make_shared<OneMax_Ruggedness1>());
	// test_eval(std::make_shared<OneMax_Ruggedness2>());
	// test_eval(std::make_shared<OneMax_Ruggedness3>());
	//
	// // test Wmodel
	// using namespace ioh::problem::wmodel;
	// test_eval(std::make_shared<W_Model_LeadingOnes>());
	// test_eval(std::make_shared<W_Model_OneMax>());	
}


void run_bbob_suite()
{
	ioh::suite::bbob bbob;
	std::shared_ptr<ioh::problem::bbob::bbob_base> problem;
	while ((problem = bbob.get_next_problem()) != nullptr)
	{
		test_eval(problem);
	}
}

void run_pbo_suite()
{
	ioh::suite::pbo pbo;
	std::shared_ptr<ioh::problem::pbo::pbo_base> problem;
	while ((problem = pbo.get_next_problem()) != nullptr)
	{
		test_eval(problem);
	}
}

void algo(std::shared_ptr<ioh::problem::bbob::bbob_base> f, std::shared_ptr<ioh::logger::csv> l)
{
	size_t n = f->get_number_of_variables();
	std::vector<double> x(n);
	double y;
	int count = 0;
	double best_y = -std::numeric_limits<double>::max();

	while (count++ <= 500)
	{
		for (auto i = 0; i != n; ++i) {
			x[i] = rng.uniform_rand() * 10 - 5;
		}
		y = f->evaluate(x);
		l->do_log(f->loggerCOCOInfo());
		best_y = std::max(y, best_y);
	}
	
}


class Base
{
public:
	virtual void x() { print("Base"); }
};


class Derived: public Base
{
public:
	void x() override { print("Derived"); }
};



class DerivedDerived : public Derived
{
public:
	void x() override { print("DerivedDerived"); }
};

class DerivedDerived2 : public Derived
{
};


int main()
{
	// auto suite = std::make_shared<ioh::suite::bbob>();
	// auto logger = std::make_shared<ioh::logger::csv>("C:\\Users\\Jacob\\Desktop", "tmp", "crazyalg", "crazyinfo");
	// auto experimenter = ioh::experiment::experimenter<ioh::problem::bbob::bbob_base>(suite, logger, algo);
	// experimenter._set_independent_runs(10);
	// experimenter._run();

	// DerivedDerived d;
	// d.x();
	// DerivedDerived2 d2;
	// d2.x();
	// ;
	// vprint(minus_one);
}
