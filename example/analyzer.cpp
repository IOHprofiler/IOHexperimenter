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

    logger::Analyzer logger({trigger::on_improvement, trigger::each(2)}, {watch::evaluations});
    logger.add_experiment_attribute("hallo", "10");

    double r = 0;
    logger.add_run_attribute("runid", &r);
    logger.add_run_attribute("runid2", &r);

    for (auto *pb : std::vector<problem::BBOB *>({&p0}))
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


int main()
{
    clutchlog::logger().threshold(clutchlog::warning);

    if(fs::exists(fs::current_path() / "ioh_data"))
        fs::remove_all(fs::current_path() / "ioh_data");

    test_single_function();
}   
