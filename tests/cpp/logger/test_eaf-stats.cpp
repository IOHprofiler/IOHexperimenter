#include "ioh.hpp"
#include <gtest/gtest.h>

TEST(eaf, stats)
{
    using namespace ioh::logger;

    ioh::common::log::log_level = ioh::common::log::Level::Warning;

    size_t runs = 10;
    size_t sample_size = 100;
    size_t buckets = 10;

    size_t pb_start = 2;
    size_t pb_end = 10;
    ioh::suite::BBOB suite({ 1, 2 }, { 1, 2 }, { 2, 10 });
    EAF eaf(0, 6e7, buckets, 0, sample_size, buckets);
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

    EXPECT_GT(eaf::stat::sum(eaf), 0);

    // Histogram
    eaf::stat::Histogram histo;
    eaf::stat::Histogram::Mat m = histo(eaf);
    EXPECT_EQ(histo.nb_attainments(), runs * (pb_end - pb_start));
    
    // buckets * buckets matrix
    EXPECT_EQ(m.size(), buckets);
    for (const auto& row : m) {
        EXPECT_EQ(row.size(), buckets);
    }

    EXPECT_EQ(eaf::stat::histogram(eaf), m);
    
    // Distribution
    eaf::stat::Distribution::Mat d = eaf::stat::distribution(eaf);
    
    // buckets * buckets matrix
    EXPECT_EQ(d.size(), buckets);
    for (const auto& row : d) {
        EXPECT_EQ(row.size(), buckets);
    }

    // Volume under curve
    EXPECT_GE(eaf::stat::under_curve::volume(eaf), 0);
    EXPECT_LE(eaf::stat::under_curve::volume(eaf), 1);
}
