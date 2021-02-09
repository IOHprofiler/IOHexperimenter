#include <list>
#include <gtest/gtest.h>
#include "ioh.hpp"




TEST(ecdf, example)
{
	using namespace ioh::problem;
	using namespace ioh::logger;
	ioh::common::log::log_level = ioh::common::log::Level::warning;
	
	size_t sample_size = 100;

	std::vector<int> pbs = { 1,2 };
	std::vector<int> ins = { 1,2 };
	std::vector<int> dims = { 2,10 };


	ioh::suite::bbob bench(pbs, ins, dims);
	size_t ecdf_width = 20;
	using Logger = ecdf<bbob::bbob_base>;

	range_log<double> error(0, 6e7, ecdf_width);
	range_log<size_t> evals(0, sample_size, ecdf_width);
	Logger logger(error, evals);

	logger.activate_logger();
	logger.track_suite(bench);


	size_t seed = 5;
	std::mt19937 gen(seed);
	std::uniform_real_distribution<> dis(-5, 5);
	ioh::suite::bbob::problem_ptr pb;


	std::list<size_t> attainments_sum = { 324, 646, 917, 1181,
		1302, 1487, 1580, 1653};
	
	size_t n = 0;
	while ((pb = bench.get_next_problem())) {
		logger.track_problem(*pb);

		size_t d = pb->get_number_of_variables();
		for (size_t s = 0; s < sample_size; ++s) {
			std::vector<double> sol;
			sol.reserve(d);
			std::generate_n(std::back_inserter(sol), d, [&dis, &gen]() {return dis(gen); });

			auto f = pb->evaluate(sol);
			logger.do_log(pb->loggerInfo());
		}
		ecdf_sum sum;
		size_t s = sum(logger.data());
		ASSERT_EQ(s, attainments_sum.front());
		attainments_sum.pop_front();
		n++;
	} // for name_id

	size_t i, j, k;
	std::tie(i, j, k) = logger.size();

	ASSERT_TRUE(i == pbs.size());
	ASSERT_TRUE(j == dims.size());
	ASSERT_TRUE(k == ins.size());
}


