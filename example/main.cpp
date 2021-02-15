#include <iostream>

#include <algorithm>
#include <vector>

#include <iostream>
#include <random>

#include "ioh/experiment/experimenter.hpp"


#include "ioh.hpp"
#include "ioh/common/random.hpp"


void bbob_random_search(std::shared_ptr<ioh::problem::bbob::bbob_base> problem,
                        std::shared_ptr<ioh::logger::Csv<ioh::problem::bbob::bbob_base>> logger) {
    const auto n = problem->get_number_of_variables();
    std::vector<double> x(n);
    auto count = 0;
    auto best_y = -std::numeric_limits<double>::infinity();
    const auto seed = 42;
    while (count++ <= 500) {
        ioh::common::Random::uniform(n, seed, x);
        best_y = std::max(problem->evaluate(x), best_y);
        logger->do_log(problem->loggerCOCOInfo());
    }
}


void pbo_random_search(std::shared_ptr<ioh::problem::pbo::pbo_base> problem,
                       std::shared_ptr<ioh::logger::Csv<ioh::problem::pbo::pbo_base>> logger) {
    const auto n = problem->get_number_of_variables();
    auto count = 0;
    auto best_y = -std::numeric_limits<double>::infinity();
    while (count++ <= 500) {
        const auto x = ioh::common::Random::integers(n, 0, 5);
        best_y = std::max(problem->evaluate(x), best_y);
        logger->do_log(problem->loggerCOCOInfo());
    }
}

void suite_test() {
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

template <class T>
std::ostream &operator <<(std::ostream &os, const std::vector<T> &v) {
    os << "[";
    for (typename std::vector<T>::const_iterator ii = v.begin(); ii != v.end(); ++ii) {
        os << " " << *ii;
    }
    os << "]";
    return os;
}

void combiner() {
    using namespace ioh;
    using namespace problem;

    const size_t sample_size = 100;

    const std::vector<int> pbs = {1, 2};
    const std::vector<int> ins = {1, 2};
    const std::vector<int> dims = {2, 10};

    suite::bbob bench(pbs, ins, dims);
    // bench.load_problem();

    logger::ecdf<bbob::bbob_base> log_ecdf(0, 6e7, 20, 0, sample_size, 20);
    logger::Csv<bbob::bbob_base> log_csv; // Use default arguments.

    logger::LoggerCombine<bbob::bbob_base> loggers({&log_ecdf, &log_csv});
    //
    loggers.track_suite(bench);

    const auto seed = 5;
    std::mt19937 gen(seed);
    // std::mt19937 gen(time(0));
    std::uniform_real_distribution<> dis(-5, 5);

    suite::bbob::problem_ptr pb;
    size_t n = 0;
    while ((pb = bench.get_next_problem())) {
        loggers.track_problem(*pb);

        std::clog << "Problem " << pb->get_problem_id()
            << " (" << pb->get_problem_name() << ")"
            << " instance " << pb->get_instance_id()
            << ", optimum: ";
        for (auto o : pb->get_optimal()) {
            std::clog << o << " ";
        }
        std::clog << std::endl;

        const size_t d = pb->get_number_of_variables();
        for (size_t s = 0; s < sample_size; ++s) {
            std::vector<double> sol;
            sol.reserve(d);
            std::generate_n(std::back_inserter(sol), d, [&dis, &gen]() { return dis(gen); });

            pb->evaluate(sol);
            loggers.do_log(pb->loggerInfo());
        }

        n++;
    } // for name_id
    std::clog << "Done " << n << " function test" << std::endl;
}

void pbo_config_experiment() {
    fs::path config = "C:\\Users\\Jacob\\Source\\Repos\\IOHprofiler\\IOHexperimenter\\example\\conf.ini";

    using namespace ioh;
    experiment::Experimenter<problem::pbo::pbo_base> pexperiment(config, pbo_random_search);
    pexperiment.set_independent_runs(10);
    pexperiment.run();

    std::vector<int> pbs = {1, 2};
    std::vector<int> ins = {1, 2};
    std::vector<int> dims = {2, 10};
    const auto suite = std::make_shared<suite::bbob>(pbs, ins, dims);
    auto logger = std::make_shared<logger::Csv<problem::bbob::bbob_base>>();
    auto bexperiment = experiment::Experimenter<problem::bbob::bbob_base>(suite, logger, bbob_random_search, 10);
    bexperiment.run();
}

void test_logger() {
    using namespace ioh;
    auto p = problem::bbob::Sphere(1, 3);
    
    std::vector<double> x = { std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()
    };
    std::cout << p.evaluate(x);

    auto l = logger::Default<problem::bbob::bbob_base>("ioh_data", "algorithm_name", "algorithm_info", true, 10);
    auto l2 = logger::Csv<problem::bbob::bbob_base>();
    
    std::cout << l.experiment_folder() << std::endl; 
    l.add_experiment_attribute("T", 10);
    l2.add_attribute("T", 10);
    
    l.create_run_attributes({ "run_id" });
    l2.set_dynamic_attributes_name({ "run_id" });
    
    l.create_logged_attributes({ "x1"});
    l2.set_parameters_name({ "x1" });
    
    logger::LoggerCombine<problem::bbob::bbob_base> loggers({&l, &l2});
    
    for (auto count = 0; 5 > count; ++count) {
        loggers.track_problem(p);
        l.set_run_attributes({"run_id"}, {static_cast<double>(count)});
        l2.set_dynamic_attributes({"run_id"}, {static_cast<double>(count)});
        p.reset_problem();
    
        for (auto i = 0; i < 50; i++) {
            const auto x = common::Random::uniform(p.get_number_of_variables());
    
            l.set_logged_attributes({ "x1" }, { x[0] });
            l2.set_parameters({ "x1" }, { x[0] });
    
            p.evaluate(x);
            loggers.do_log(p.loggerCOCOInfo());
        }
    }
}







class IntegerProblem : public ioh::problem::Problem<int> {
protected:
    std::vector<double> evaluate(std::vector<int>& x) override {
        return { 0.0 };
    }

public:
    IntegerProblem(const int n_variables = 1)
        : Problem("IntegerProblem", n_variables) {

    }
};

std::vector<double> null_function(std::vector<double>& x) {
    return { 0.0 };
}

 
int main() {
    using namespace ioh::problem;
    
    // auto f = wrap_function<double>(&null_function, "Nuller", 5);
    //
    // std::cout << f << std::endl;
    //
    // const auto x = std::vector<double>{ 1,2,3,4,6 };
    // f(x);
    //
    // std::cout << f << std::endl;


    auto s = Sphere(1, 10);
    std::cout << s << std::endl;

    auto x = s.state();

    //
    // s(s.meta_data().objective.x);
    //
    // std::cout << s << std::endl;

     
    // auto integer_problem = IntegerProblem(4);
    //
    // std::cout << integer_problem << std::endl;



    std::cout << "done";
}
