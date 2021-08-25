#include "../utils.hpp"

#include "ioh/suite.hpp"
 

TEST_F(BaseTest, suite_factory)
{
    using namespace ioh;

    std::vector<int> problem_ids(24);
    std::iota(std::begin(problem_ids), std::end(problem_ids), 1);

    auto suite = suite::SuiteRegistry<problem::Real>::instance().create("BBOB", problem_ids, {1}, {2});

    auto problems = suite->problem_ids();

    for (size_t i = 0; i < problems.size(); i++)
        EXPECT_EQ(problem_ids.at(i), problems.at(i));
}

TEST_F(BaseTest, problem_suite_bbob)
{
    std::vector<int> ids(24);
    std::iota(ids.begin(), ids.end(), 1);
    ioh::suite::BBOB bbob(ids, {1}, {4});
    auto results = std::list<double>(
        {85.3339,   4670466.7111, -351.7320,   -377.5170,     75.4977,   38905.1446, 180.6339,  3990.2021,
         2734.3342, 4272872.0872, 633536.4945, 43065603.7200, 1304.6100, -28.6647,   1880.5481,
         192.2602, // 192.0590,
         -3.8932,   39.4152,      -92.2225,    6135.2508,     100.6898,  -924.8833,  21.7005,   177.9893});

    for (const auto &problem : bbob)
    {
        EXPECT_DOUBLE_EQ(std::round((*problem)(ioh::common::random::pbo::normal(4, 42)) * 10000.0) / 10000.0,
                         results.front());
        results.pop_front();
    }
}

TEST_F(BaseTest, suite_pbo)
{
    std::vector<int> problem_id;
    std::vector<int> instance_id;
    std::vector<int> dimension = {16, 64};

    for (auto i = 1; i != 26; ++i)
    {
        problem_id.push_back(i);
    }

    for (auto i = 1; i != 6; ++i)
    {
        instance_id.push_back(i);
    }
    ioh::suite::PBO pbo(problem_id, instance_id, dimension);

    EXPECT_EQ(pbo.problem_ids().size(), 25);
    EXPECT_EQ(pbo.instances().size(), 5);
    EXPECT_EQ(pbo.dimensions().size(), 2);
    EXPECT_EQ(pbo.name(), "PBO");

    auto tmp_p_index = 0;
    auto tmp_i_index = 0;
    auto tmp_d_index = 0;

    for (const auto &problem : pbo)
    {
        EXPECT_LT(tmp_p_index, 25);
        EXPECT_EQ(problem->meta_data().problem_id, problem_id[tmp_p_index]);
        EXPECT_EQ(problem->meta_data().instance, instance_id[tmp_i_index]);
        EXPECT_EQ(problem->meta_data().n_variables, dimension[tmp_d_index]);

        tmp_i_index++;
        if (tmp_i_index == 5)
        {
            tmp_i_index = 0;
            tmp_d_index++;
            if (tmp_d_index == 2)
            {
                tmp_d_index = 0;
                tmp_p_index++;
            }
        }
    }
}

TEST_F(BaseTest, suite_bbob)
{
    std::vector<int> problem_id;
    std::vector<int> instance_id;
    std::vector<int> dimension = {2, 10};

    for (auto i = 1; i != 25; ++i)
    {
        problem_id.push_back(i);
    }

    for (auto i = 1; i != 6; ++i)
    {
        instance_id.push_back(i);
    }
    ioh::suite::BBOB bbob(problem_id, instance_id, dimension);

    EXPECT_EQ(bbob.problem_ids().size(), 24);
    EXPECT_EQ(bbob.instances().size(), 5);
    EXPECT_EQ(bbob.dimensions().size(), 2);
    EXPECT_EQ(bbob.name(), "BBOB");

    auto tmp_p_index = 0;
    auto tmp_i_index = 0;
    auto tmp_d_index = 0;

    for (const auto &problem : bbob)
    {
        EXPECT_LT(tmp_p_index, 25);
        EXPECT_EQ(problem->meta_data().problem_id, problem_id[tmp_p_index]);
        EXPECT_EQ(problem->meta_data().instance, instance_id[tmp_i_index]);
        EXPECT_EQ(problem->meta_data().n_variables, dimension[tmp_d_index]);

        tmp_i_index++;
        if (tmp_i_index == 5)
        {
            tmp_i_index = 0;
            tmp_d_index++;
            if (tmp_d_index == 2)
            {
                tmp_d_index = 0;
                tmp_p_index++;
            }
        }
    }
}
