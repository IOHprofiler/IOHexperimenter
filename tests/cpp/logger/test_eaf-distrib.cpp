#include "ioh.hpp"
#include <gtest/gtest.h>

template<class RR, class RV>
void do_test()
{
    using namespace ioh::logger;

    size_t runs = 13;
    size_t sample_size = 1000;
    size_t buckets = 100;

    size_t pb_start = 2;
    size_t pb_end = 10;
    ioh::suite::BBOB suite({ 1, 2 }, { 1, 2 }, { 2, 10 });

    RR r_error(0,6e7,buckets);
    RV r_evals(0,sample_size,buckets);
    
    EAF eaf(r_error, r_evals);
    suite.attach_logger(eaf);

    for (const auto& pb : suite) {
        for (size_t run = 0; run < runs; ++run) {
            // FIXME how to indicate different runs to the logger?
             // eaf.update_run_info(pb->meta_data());
            for (auto s = 0; s < sample_size; ++s) {
                (*pb)(ioh::common::Random::uniform(pb->meta_data().n_variables));
            } // s
            pb->reset();
        } // run
    } // pb

    auto d = eaf::stat::distribution(eaf);
    std::clog << "Log-log joint cumulative distribution for errors and evaluations:" << std::endl;
    std::clog << eaf::colormap(d, {&r_error, &r_evals}, true) << std::endl;
    // std::clog << eaf::colormap(d, {&r_error, &r_evals}) << std::endl;
    // std::clog << eaf::colormap(d) << std::endl;
    // 
    auto h = eaf::stat::histogram(eaf);
    std::clog << "Log-log joint cumulative histogram for errors and evaluations:" << std::endl;
    std::clog << eaf::colormap(h, {&r_error, &r_evals}, true) << std::endl;
    // std::clog << eaf::colormap(h, {&r_error, &r_evals}) << std::endl;
    // std::clog << eaf::colormap(h) << std::endl;
}

TEST(eaf, stats)
{
    using namespace ioh::logger::eaf;

    ioh::common::log::log_level = ioh::common::log::Level::Warning;

    do_test<LinearScale<double>, LinearScale<size_t>>();
    do_test<  Log2Scale<double>, LinearScale<size_t>>();
    do_test<LinearScale<double>,   Log2Scale<size_t>>();
    do_test<  Log2Scale<double>,   Log2Scale<size_t>>();
}
