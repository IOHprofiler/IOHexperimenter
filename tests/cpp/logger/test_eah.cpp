#include "../utils.hpp"

#include "ioh/logger/eah.hpp"
#include "ioh/problem/bbob.hpp"
#include "ioh/suite.hpp"

TEST_F(BaseTest, eah_bbob)
{
    using namespace ioh::problem;
    using namespace ioh::logger;

    auto sample_size = 100;

    ioh::suite::BBOB suite({1, 2}, {1, 2}, {2, 10});
    ioh::logger::EAH logger(0, 6e7, 20, 0, sample_size, 20);

    suite.attach_logger(logger);

    std::list<size_t> attainments_sum = {
        340, 680, 969, 1258, 1547, 1836, 2091, 2346, 2465, 2584, 2635, 2686, 2737, 2788, 2856, 2924
    };

    for (const auto &p : suite) {
        for (auto r = 0; r < 2; r++) {
            for (auto s = 0; s < sample_size; ++s)
                (*p)(ioh::common::random::pbo::uniform(p->meta_data().n_variables, 0));

            EXPECT_EQ(ioh::logger::eah::stat::sum(logger), attainments_sum.front());
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

TEST_F(BaseTest, eah_size_without_data)
{
    // an EAH without data has size 0 (size() read the first problem of an empty map)
    ioh::logger::EAH logger(0, 6e7, 20, 0, 100, 20);
    EXPECT_EQ(logger.size(), std::make_tuple(size_t{0}, size_t{0}, size_t{0}, size_t{0}));

    // attaching a problem adds its problem, dimension and instance; its first evaluation adds the run
    auto problem = ioh::problem::bbob::Sphere(1, 2);
    problem.attach_logger(logger);
    EXPECT_EQ(logger.size(), std::make_tuple(size_t{1}, size_t{1}, size_t{1}, size_t{0}));
    problem(std::vector<double>{0.0, 0.0});
    EXPECT_EQ(logger.size(), std::make_tuple(size_t{1}, size_t{1}, size_t{1}, size_t{1}));
}
