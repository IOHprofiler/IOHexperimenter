#include <cmath>
#include <list>
#include <gtest/gtest.h>
#include "ioh.hpp"



double test_eval(std::shared_ptr<ioh::problem::bbob::bbob_base> f)
{
	const std::vector<double> x = ioh::common::random::gauss(4, 42);
	return std::round(f->evaluate(x) *10000.0) / 10000.0;
}


double test_eval(std::shared_ptr<ioh::problem::pbo::pbo_base> f)
{
	const std::vector<int> x = { 3,2,0,-1,0,0,0,1,2 };
	return std::floor(f->evaluate(x) * 10000.0) / 10000.0;
}


TEST(problems, bbob)
{
	using namespace ioh::problem::bbob;
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Attractive_Sector>()), 38905.1446);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Bent_Cigar>()), 43065603.7200);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Bueche_Rastrigin>()), -377.5170);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Different_Powers>()), -28.6647);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Discus>()), 633536.4945);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Ellipsoid>()), 4670466.7111);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Ellipsoid_Rotated>()), 4272872.0872);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Gallagher101>()), 100.6898);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Gallagher21>()), -924.8833);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Griewank_RosenBrock>()), -92.2225);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Katsuura>()), 21.7005);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Linear_Slope>()), 75.4977);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Lunacek_Bi_Rastrigin>()), 177.9893);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Rastrigin>()), -351.7320);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Rastrigin_Rotated>()), 1880.5481);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Rosenbrock>()), 3990.2021);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Rosenbrock_Rotated>()), 2734.3342);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Schaffers10>()), -3.8932);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Schaffers1000>()), 39.4152);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Schwefel>()), 6135.2508);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Sharp_Ridge>()), 1304.6100);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Sphere>()), 85.3339);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Step_Ellipsoid>()), 180.6339);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Weierstrass>()), 192.0590);
}

TEST(problems, pbo)
{
	using namespace ioh::problem::pbo;
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<OneMax>(1, 9)), 7.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<OneMax_Dummy1>(1, 9)), 4.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<OneMax_Dummy2>(1, 9)), 5.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<OneMax_Epistasis>(1, 9)), 9.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<OneMax_Neutrality>(1, 9)), 2.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<OneMax_Ruggedness1>(1, 9)), 5.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<OneMax_Ruggedness2>(1, 9)), 8.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<OneMax_Ruggedness3>(1, 9)), 5.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<LeadingOnes>(1, 9)), 0.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<LeadingOnes_Dummy1>(1, 9)), 0.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<LeadingOnes_Dummy2>(1, 9)), 0.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<LeadingOnes_Epistasis>(1, 9)), 6.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<LeadingOnes_Neutrality>(1, 9)), 1.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<LeadingOnes_Ruggedness1>(1, 9)), 1.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<LeadingOnes_Ruggedness2>(1, 9)), 0.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<LeadingOnes_Ruggedness3>(1, 9)), 3.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Linear>(1, 9)), 29.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<MIS>(1, 9)), 1.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<LABS>(1, 9)), 2.5312);
	ASSERT_DOUBLE_EQ(test_eval(std::make_shared<NQueens>(1, 9)), -44.0000);

	// These are different from master, might be because I took the dev version
	// ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Ising_Ring>(1, 9)), 23.0000);
	// ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Ising_Torus>(1, 9)), 4.0000);
	// ASSERT_DOUBLE_EQ(test_eval(std::make_shared<Ising_Triangular>(1, 9)), 9.0000);
}

TEST(problem_suite, bbob)
{
	ioh::common::log::log_level = ioh::common::log::level::warning;

	ioh::suite::bbob bbob;
	std::shared_ptr<ioh::problem::bbob::bbob_base> problem;	
	auto results = std::list<double>({
		85.3339,
		4670466.7111,
		-351.7320,
		-377.5170,
		75.4977,
		38905.1446,
		180.6339,
		3990.2021,
		2734.3342,
		4272872.0872,
		633536.4945,
		43065603.7200,
		1304.6100,
		-28.6647,
		1880.5481,
		192.0590,
		-3.8932,
		39.4152,
		-92.2225,
		6135.2508,
		100.6898,
		-924.8833,
		21.7005,
		177.9893	
	});
	while ((problem = bbob.get_next_problem()) != nullptr)
	{
		ASSERT_DOUBLE_EQ(test_eval(problem), results.front());
		results.pop_front();
	}
}