#include <list>
#include <gtest/gtest.h>
#include "ioh.hpp"


TEST(ecdf, example)
{
	using namespace ioh::problem;
	using namespace ioh::logger;
	ioh::common::log::log_level = ioh::common::log::Level::Warning;
	
	size_t sample_size = 100;
	size_t ecdf_width = 20;
	std::vector<int> pbs = { 1,2 };
	std::vector<int> ins = { 1,2 };
	std::vector<int> dims = { 2,10 };


	ioh::suite::BBOB bench(pbs, ins, dims);

	LogRange<double> error(0, 6e7, ecdf_width);
	LogRange<size_t> evals(0, sample_size, ecdf_width);
	ECDF logger(error, evals);

	bench.attach_logger(logger);


	size_t seed = 5;
	std::mt19937 gen(seed);
	std::uniform_real_distribution<> dis(-5, 5);


	std::list<size_t> attainments_sum = { 324, 646, 917, 1181,
		1302, 1487, 1580, 1653};
	
	size_t n = 0;
	for (const auto& p : bench) {
	    size_t d = p->meta_data().n_variables;
		for (size_t s = 0; s < sample_size; ++s) {
			std::vector<double> sol;
			sol.reserve(d);
			std::generate_n(std::back_inserter(sol), d, [&dis, &gen]() {return dis(gen); });
			(*p)(sol);
		}
		ECDFSum sum;
		size_t s = sum(logger.data());
		ASSERT_EQ(s, attainments_sum.front());
		attainments_sum.pop_front();
		n++;
	} 

	size_t i, j, k;
	std::tie(i, j, k) = logger.size();

	ASSERT_TRUE(i == pbs.size());
	ASSERT_TRUE(j == dims.size());
	ASSERT_TRUE(k == ins.size());
}


