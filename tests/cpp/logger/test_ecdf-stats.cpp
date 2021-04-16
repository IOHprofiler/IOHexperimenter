#include "ioh.hpp"
#include <gtest/gtest.h>

TEST(ecdf, stats)
{
    ioh::common::log::log_level = ioh::common::log::Level::Warning;

    size_t sample_size = 100;
    size_t buckets = 20;

    ioh::suite::BBOB suite({ 1, 2 }, { 1, 2 }, { 2, 10 });
    ioh::logger::ECDF logger(0, 6e7, buckets, 0, sample_size, buckets);
    suite.attach_logger(logger);

    for (const auto& pb : suite) {
        for (auto s = 0; s < sample_size; ++s)
            (*pb)(ioh::common::Random::uniform(pb->meta_data().n_variables));

        pb->reset();
    }

    ioh::logger::ecdf::stat::Sum sum;
    EXPECT_GT(sum(logger.data()), 0);

    ioh::logger::ecdf::stat::Histogram histo;
    ioh::logger::ecdf::stat::Histogram::Mat m = histo(logger.data());
    EXPECT_EQ(m.size(), buckets);
    for(const auto& row : m) {
        EXPECT_EQ(row.size(), buckets);
    }
    
}
