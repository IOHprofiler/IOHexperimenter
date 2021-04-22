#include "ioh.hpp"
#include <gtest/gtest.h>

TEST(ecdf, stats)
{
    ioh::common::log::log_level = ioh::common::log::Level::Warning;

    size_t runs = 10;
    size_t sample_size = 10;
    size_t buckets = 10;

    ioh::suite::BBOB suite({ 1, 2 }, { 1, 2 }, { 2, 10 });
    ioh::logger::ECDF logger(0, 6e7, buckets, 0, sample_size, buckets);
    suite.attach_logger(logger);

    for (const auto& pb : suite) {
        for (size_t run = 0; run < runs; ++run) {
            // FIXME how to indicate different runs to the logger?
            // logger.update_run_info(pb->meta_data());
            for (auto s = 0; s < sample_size; ++s) {
                (*pb)(ioh::common::Random::uniform(pb->meta_data().n_variables));
            } // s
            pb->reset();
        } // run
    } // pb

    ioh::logger::ecdf::stat::Sum sum;
    EXPECT_GT(sum(logger.data()), 0);

    ioh::logger::ecdf::stat::Histogram histo;
    ioh::logger::ecdf::stat::Histogram::Mat m = histo(logger.data());
    EXPECT_EQ(m.size(), buckets);
    for (const auto& row : m) {
        EXPECT_EQ(row.size(), buckets);
    }

    ioh::logger::ecdf::stat::VolumeUnderCurve vuc(logger);
    EXPECT_GT(vuc(logger.data()), 0);
    EXPECT_LE(vuc(logger.data()), buckets * buckets);
}
