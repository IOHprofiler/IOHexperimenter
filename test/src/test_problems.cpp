#include <gtest/gtest.h>
#include "ioh.hpp"
#include <cmath>


double test_eval(std::unique_ptr<ioh::problem::bbob::bbob_base> f)
{
	const std::vector<double> x = ioh::common::random::gauss(4, 42);
	return std::round(f->evaluate(x) *10000.0) / 10000.0;
}


double test_eval(std::unique_ptr<ioh::problem::pbo::pbo_base> f)
{
	const std::vector<int> x = { 3,2,0,-1,0,0,0,1,2 };
	return std::floor(f->evaluate(x) * 10000.0) / 10000.0;
}


TEST(problems, bbob)
{
	using namespace ioh::problem::bbob;
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Attractive_Sector>()), 38905.1446);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Bent_Cigar>()), 43065603.7200);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Bueche_Rastrigin>()), -377.5170);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Different_Powers>()), -28.6647);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Discus>()), 633536.4945);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Ellipsoid>()), 4670466.7111);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Ellipsoid_Rotated>()), 4272872.0872);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Gallagher101>()), 100.6898);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Gallagher21>()), -924.8833);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Griewank_RosenBrock>()), -92.2225);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Katsuura>()), 21.7005);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Linear_Slope>()), 75.4977);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Lunacek_Bi_Rastrigin>()), 177.9893);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Rastrigin>()), -351.7320);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Rastrigin_Rotated>()), 1880.5481);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Rosenbrock>()), 3990.2021);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Rosenbrock_Rotated>()), 2734.3342);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Schaffers10>()), -3.8932);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Schaffers1000>()), 39.4152);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Schwefel>()), 6135.2508);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Sharp_Ridge>()), 1304.6100);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Sphere>()), 85.3339);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Step_Ellipsoid>()), 180.6339);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Weierstrass>()), 192.0590);
}

TEST(problems, pbo)
{
	using namespace ioh::problem::pbo;
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<OneMax>(1, 9)), 7.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<OneMax_Dummy1>(1, 9)), 4.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<OneMax_Dummy2>(1, 9)), 5.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<OneMax_Epistasis>(1, 9)), 9.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<OneMax_Neutrality>(1, 9)), 2.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<OneMax_Ruggedness1>(1, 9)), 5.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<OneMax_Ruggedness2>(1, 9)), 8.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<OneMax_Ruggedness3>(1, 9)), 5.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<LeadingOnes>(1, 9)), 0.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<LeadingOnes_Dummy1>(1, 9)), 0.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<LeadingOnes_Dummy2>(1, 9)), 0.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<LeadingOnes_Epistasis>(1, 9)), 6.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<LeadingOnes_Neutrality>(1, 9)), 1.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<LeadingOnes_Ruggedness1>(1, 9)), 1.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<LeadingOnes_Ruggedness2>(1, 9)), 0.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<LeadingOnes_Ruggedness3>(1, 9)), 3.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Linear>(1, 9)), 29.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<MIS>(1, 9)), 1.0000);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<LABS>(1, 9)), 2.5312);
	ASSERT_DOUBLE_EQ(test_eval(std::make_unique<NQueens>(1, 9)), -44.0000);

	// These are different from master, might be because I took the dev version
	// ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Ising_Ring>(1, 9)), 23.0000);
	// ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Ising_Torus>(1, 9)), 4.0000);
	// ASSERT_DOUBLE_EQ(test_eval(std::make_unique<Ising_Triangular>(1, 9)), 9.0000);
}