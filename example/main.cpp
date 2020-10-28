#include <iostream>
#include <iomanip>
#include <numeric>

#include <vector>
#include <algorithm>

#include <cassert>
#include <iostream>
#include <random>
#include <filesystem>

#include "ioh/experiment/experimenter.hpp"
 
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


std::vector<int> Initialization(int dimension) {
	std::vector<int> x;
	x.reserve(dimension);
	
	for (int i = 0; i != dimension; ++i) {
		x.push_back((int)(rng.uniform_rand() * 2));
	}
	return x;
};

int mutation(std::vector<int>& x, double mutation_rate) {
	int result = 0;
	int n = x.size();
	for (int i = 0; i != n; ++i) {
		if (rng.uniform_rand() < mutation_rate) {
			x[i] = (x[i] + 1) % 2;
			result = 1;
		}
	}
	return result;
}

void pboalgo(std::shared_ptr<ioh::problem::pbo::pbo_base> problem, std::shared_ptr<ioh::logger::csv<ioh::problem::pbo::pbo_base>> logger)
{
	size_t n = problem->get_number_of_variables();
	const int budget_scale = 100;

	std::vector<int> x;
	std::vector<int> x_star;
	double y;
	double best_value;
	double* mutation_rate = new double(1);
	*mutation_rate = 1.0 / n;
	int budget = budget_scale * n * n;

	std::vector<std::shared_ptr<double> > parameters;
	parameters.push_back(std::shared_ptr<double>(mutation_rate));
	std::vector<std::string> parameters_name;
	parameters_name.push_back("mutation_rate");
	logger->set_parameters(parameters, parameters_name);

	x = Initialization(n);
	x_star = x;
	y = problem->evaluate(x);
	logger->do_log(problem->loggerInfo());

	best_value = y;

	int count = 0;
	while (count <= budget && !problem->hit_optimal()) {
		x = x_star;
		if (mutation(x, *mutation_rate)) {
			y = problem->evaluate(x);
			logger->do_log(problem->loggerInfo());
			count++;
		}
		if (y >= best_value) {
			best_value = y;
			x_star = x;
		}
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

template < class T >
std::ostream& operator << (std::ostream& os, const std::vector<T>& v)
{
	os << "[";
	for (typename std::vector<T>::const_iterator ii = v.begin(); ii != v.end(); ++ii)
	{
		os << " " << *ii;
	}
	os << "]";
	return os;
}

void combiner()
{
	using namespace ioh;
	using namespace ioh::problem;


	size_t sample_size = 100;

	std::vector<int> pbs = { 1,2 };
	std::vector<int> ins = { 1,2 };
	std::vector<int> dims = { 2,10 };

	ioh::suite::bbob bench(pbs, ins, dims);
	// bench.load_problem();

	logger::ecdf<bbob::bbob_base> log_ecdf(0, 6e7, 20, 0, sample_size, 20);
	logger::csv<bbob::bbob_base> log_csv; // Use default arguments.

	logger::observer_combine<bbob::bbob_base> loggers({ &log_ecdf, &log_csv });
	//
	loggers.track_suite(bench);

	int seed = 5;
	std::mt19937 gen(seed);
	// std::mt19937 gen(time(0));
	std::uniform_real_distribution<> dis(-5, 5);


	ioh::suite::bbob::problem_ptr pb;
	size_t n = 0;
	while ((pb = bench.get_next_problem())) {
		loggers.track_problem(*pb);

		std::clog << "Problem " << pb->get_problem_id()
			<< " (" << pb->get_problem_name() << ")"
			<< " instance " << pb->get_instance_id()
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
			loggers.do_log(pb->loggerInfo());
		}

		n++;
	} // for name_id
	std::clog << "Done " << n << " function test" << std::endl;

}



int main()
{

	std::string config = "C:\\Users\\Jacob\\Source\\Repos\\IOHprofiler\\IOHexperimenter\\example\\conf.ini";
	ioh::experiment::experimenter<ioh::problem::pbo::pbo_base> pexperiment(config, pboalgo);
	pexperiment.set_independent_runs(10);
	pexperiment.run();

	using namespace ioh;
	std::vector<int> pbs = { 1,2 };
	std::vector<int> ins = { 1,2 };
	std::vector<int> dims = { 2,10 };
	auto suite = std::make_shared<suite::bbob>(pbs, ins, dims);
	auto logger = std::make_shared<logger::csv<problem::bbob::bbob_base>>();
	auto bexperiment = experiment::experimenter<problem::bbob::bbob_base>(suite, logger, algo);
	
	bexperiment.set_independent_runs(10);
	bexperiment.run();
}


