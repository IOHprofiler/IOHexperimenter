#include <iostream>
#include <iomanip>
#include <numeric>

#include <vector>
#include <algorithm>

#include <cassert>
#include <iostream>
#include <random>
#include <filesystem>

namespace fs = std::filesystem;

#include "ioh.hpp"


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



void algo(std::shared_ptr<ioh::problem::bbob::bbob_base> f, std::shared_ptr<ioh::logger::csv<ioh::problem::bbob::bbob_base>> l)
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


void suite_test()
{

	{
		ioh::suite::bbob bbob;
		std::shared_ptr<ioh::problem::bbob::bbob_base> problem;
		while ((problem = bbob.get_next_problem()) != nullptr)
			std::cout << problem->get_problem_name() << std::endl;
	}
	std::cout << "*****************" << std::endl;
	ioh::suite::bbob bbob({1}, {1, 2}, {5, 6});
	std::shared_ptr<ioh::problem::bbob::bbob_base> problem;
	while ((problem = bbob.get_next_problem()) != nullptr)
		std::cout << problem->get_problem_name() << std::endl <<
			problem->get_instance_id() << std::endl <<
			problem->get_number_of_objectives() << std::endl;
	
	

	// ioh::common::register_in_factory<BaseOneArg, C, int> c("C");
	// ioh::common::register_in_factory<BaseOneArg, D, int> d("D");
	// std::cout << ioh::common::factory<BaseOneArg, int>::get().create("D", 1)->get_name() << std::endl;
}



void ecdf_test()
{
	using namespace ioh::problem;
	using namespace ioh::logger;
	
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
	
	size_t n = 0;
	while ((pb = bench.get_next_problem())) {
		logger.track_problem(*pb);
	
		std::clog << "Problem " << pb->get_problem_id()
			<< " (" << pb->get_problem_name() << ")"
			<< " get " << pb->get_instance_id()
			<< ", optimum: ";
	
		for (double o : pb->get_optimal()) {
			std::clog << o << " ";
		}
		std::clog << std::endl;
	
		size_t d = pb->get_number_of_variables();
		for (size_t s = 0; s < sample_size; ++s) {
			std::vector<double> sol;
			sol.reserve(d);
			std::generate_n(std::back_inserter(sol), d, [&dis, &gen]() {return dis(gen); });
	
			double f = pb->evaluate(sol);
			logger.do_log(pb->loggerInfo());
		}
		n++;
	} // for name_id
	
	std::clog << "Done " << n << " function test" << std::endl;
	
	size_t i, j, k;
	std::tie(i, j, k) = logger.size();
	std::clog << i << " problems × " << j << " dimensions × " << k << " instances" << std::endl;
	assert(i == pbs.size());
	assert(j == dims.size());
	assert(k == ins.size());
	
	
	
	for (int ipb : pbs) {
		for (int idim : dims) {
			for (int iins : ins) {
				std::clog << "Problem " << ipb
					<< ", dimension " << idim
					<< ", get " << iins
					<< ": " << std::endl;
				const auto& m = logger.at(ipb, iins, idim);
				// std::clog << m << std::endl;
				assert(m.size() == ecdf_width);
				assert(m[0].size() == ecdf_width);
			}
		}
	}
	
	ecdf_sum sum;
	size_t s = sum(logger.data());
	std::clog << "Attainments sum: ";
	std::cout << s << std::endl;
	assert(s <= sample_size * ecdf_width * ecdf_width * i * j * k);
}

int main()
{
	// 	
	// ioh::problem::python::ExternPythonProblem<int> e("test_problem", "pb1");
	// size_t d = e.get_number_of_variables();
	//
	// std::vector<int> sol(d, 6);
	// double fit = e.evaluate(sol);
	// std::cout << fit << std::endl;

}


