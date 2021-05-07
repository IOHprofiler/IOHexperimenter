#include <list>
#include <gtest/gtest.h>
#include "ioh.hpp"


TEST(eaf, bbob)
{
    using namespace ioh::problem;
    using namespace ioh::logger;
    ioh::common::log::log_level = ioh::common::log::Level::Warning;
     
    auto sample_size = 100;

    ioh::suite::BBOB suite({1, 2}, {1, 2}, {2, 10});
    ioh::logger::EAF logger(0, 6e7, 20, 0, sample_size, 20);

    suite.attach_logger(logger);

    std::list<size_t> attainments_sum = {
        323, 646, 935, 1224, 1512, 1798, 2053, 2312,
        2554, 2785, 2850, 2915, 2982, 3046, 3148, 3242
    };


    for (const auto &p : suite)
    {
        for (auto r = 0; r < 2; r++)
        {
            for (auto s = 0; s < sample_size; ++s)
                (*p)(ioh::common::Random::uniform(p->meta_data().n_variables));

            EXPECT_EQ(ioh::logger::eaf::stat::sum(logger), attainments_sum.front());
            p->reset();
            attainments_sum.pop_front();
        }
    }
    auto [i, j, k, r] = logger.size();

    EXPECT_EQ(i, 2);
    EXPECT_EQ(j, 2);
    EXPECT_EQ(k, 2);
    EXPECT_EQ(r, 2);
}

