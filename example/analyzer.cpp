#include <iostream>

#include "ioh.hpp"

using namespace ioh;

template <typename T>
std::vector<T> print(const std::vector<T>& x)
{
    for (const auto &e : x)
        std::cout << e << " ";
    std::cout << std::endl;
    return x;
}

void test_single_function(){
    auto p0 = problem::bbob::Discus(1, 2);
    auto p1 = problem::bbob::Discus(1, 5);

    logger::Analyzer logger({trigger::on_improvement, trigger::each(3)}, {watch::evaluations});
    logger.add_experiment_attribute("hallo", "10");

    double r = 0;
    logger.add_run_attribute("runid", &r);
    logger.add_run_attribute("runid2", &r);

    for (auto *pb : std::vector<problem::BBOB *>({&p0, &p1}))
    {
        pb->attach_logger(logger);
        for (r = 0; r < 3; ++r)
        {
            for (auto s = 1; s < 11; ++s)
                (*pb)(common::random::uniform(pb->meta_data().n_variables, s));
            pb->reset();
        }
    }
}

void test_suite(){
    suite::BBOB suite(common::range(1, 25), common::range(1, 6), common::range(2, 6));

    logger::Analyzer logger({trigger::on_improvement}, {});
    suite.attach_logger(logger);
 
    for (auto & pb: suite){
        for (auto r = 0; r < 2; ++r)
        {
            for (auto s = 1; s < 11; ++s)
                (*pb)(common::random::uniform(pb->meta_data().n_variables, s));
            pb->reset();
        }
    }
}

void test_single_function_new(){
    auto p0 = problem::bbob::Discus(1, 2);
    auto p1 = problem::bbob::Discus(1, 5);

    logger::analyzer::v2::Analyzer logger({trigger::on_improvement, trigger::each(3)}, {watch::evaluations});
    logger.add_experiment_attribute("hallo", "10");

    double r = 0;
    logger.add_run_attribute("runid", &r);
    logger.add_run_attribute("runid2", &r);

    for (auto *pb : std::vector<problem::BBOB *>({&p0, &p1}))
    {
        pb->attach_logger(logger);
        for (r = 0; r < 3; ++r)
        {
            for (auto s = 1; s < 11; ++s)
                (*pb)(common::random::uniform(pb->meta_data().n_variables, s));
            pb->reset();
        }
    }


}

void test_new(){
    using namespace logger::analyzer::structures;

    problem::bbob::Schaffers1000 problem {1, 5};
    AlgorithmInfo algorithm{"PSO", "A1"};

    double x = 1.0;
    std::vector<Attribute<double>> rv{{"r1", x}};
    std::vector<Attribute<std::string>> rx{{"ra", "b"}};
    BestPoint bp {67, {std::vector<double>(5), 10.0}};
    RunInfo r1 {1, 1000, bp, rv};

    std::vector<RunInfo> runs{r1, r1};

    ScenarioInfo d5 = {5, "/tmp/txt.csv", runs}; 
    std::vector<ScenarioInfo> ds {d5, d5, d5};

    ExperimentInfo e1{"suite", problem.meta_data(), algorithm, rx, {"r1", "r1"}, {}, ds};

    std::cout << fmt::format("{}", e1) << std::endl;
}


int main()
{
    clutchlog::logger().threshold(clutchlog::warning);

    if(fs::exists(fs::current_path() / "ioh_data"))
        fs::remove_all(fs::current_path() / "ioh_data");

    test_single_function();
}   
