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

void compbiner()
{
	using namespace ioh;
	using namespace ioh::problem;


	size_t sample_size = 100;

	std::vector<int> pbs = { 1,2 };
	std::vector<int> ins = { 1,2 };
	std::vector<int> dims = { 2,10 };

	ioh::suite::bbob bench(pbs, ins, dims);
	// bench.loadProblem();

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

	std::string config = "example\\conf.ini";


	
	/// An example for PBO suite.
	// ioh::experiment::experimenter<ioh::problem::bbob::bbob_base> experimenter(config, algo);

	// ioh::experiment::configuration conf(config);
	//
	// std::cout << conf.get_suite_name() << std::endl
	// 	<< conf.get_problem_id() << std::endl
	// 	<< conf.get_instance_id() << std::endl
	// 	<< conf.get_dimension() << std::endl
	// 	<< conf.get_output_directory() << std::endl
	// 	<< conf.get_result_folder() << std::endl
	// 	<< conf.get_algorithm_info() << std::endl
	// 	<< conf.get_algorithm_name() << std::endl
	// 	<< conf.get_base_evaluation_triggers() << std::endl
	// 	<< conf.get_update_triggers() << std::endl
	// 	<< conf.get_complete_triggers() << std::endl
	// 	<< conf.get_number_target_triggers() << std::endl
	// 	<< conf.get_number_interval_triggers() << std::endl;
		
	
	fs::path root;
	for (auto e: fs::current_path())
	{
		root /= e;
		if(fs::exists(root / config))
			std::cout << root/config << std::endl;
	}
		
	
}


