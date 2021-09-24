#include "../utils.hpp" 

#include "ioh/problem/pbo.hpp"

double test_eval(const std::shared_ptr<ioh::problem::Integer> &f)
{
    const std::vector<int> x = {1, 1, 0, 1, 0, 0, 0, 1, 1};
    return std::floor((*f)(x) * 10000.0) / 10000.0;
}

TEST_F(BaseTest, PBOfitness_dimension16)
{
    auto& ioh_dbg = clutchlog::logger();
    ioh_dbg.threshold(clutchlog::level::xdebug);

    const auto file_name = find_test_file("pbofitness16.in");

    std::string s;
    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::PBO>::instance();

    std::ifstream infile(file_name.c_str());
    ASSERT_TRUE(infile.is_open());
    while (getline(infile, s))
    {
        auto tmp = split(s, " ");
        auto func_id = stoi(tmp[0]);
        auto ins_id = stoi(tmp[1]);
        auto x = string_to_vector_int(tmp[2]);
        auto f = stod(tmp[3]);

        auto problem = problem_factory.create(func_id, ins_id, 16);
        auto y = (*problem)(x);

        EXPECT_LE(abs(f - y), 0.0001) << "The fitness of function " << func_id << "( ins " << ins_id << " ) is " << f <<
 " ( not " << y << ").";
    }
}

TEST_F(BaseTest, PBOfitness_dimension100)
{
    auto& ioh_dbg = clutchlog::logger();
    ioh_dbg.threshold(clutchlog::level::xdebug);

    const auto file_name = find_test_file("pbofitness100.in");
    std::string s;
    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::PBO>::instance();
    std::ifstream infile(file_name.c_str());
    ASSERT_TRUE(infile.is_open());
    while (getline(infile, s))
    {
        auto tmp = split(s, " ");
        auto func_id = stoi(tmp[0]);
        auto ins_id = stoi(tmp[1]);
        auto x = string_to_vector_int(tmp[2]);
        auto f = stod(tmp[3]);

        auto problem = problem_factory.create(func_id, ins_id, 100);
        auto y = (*problem)(x);
        EXPECT_LE(abs(f - y), 0.0001) << "The fitness of function " << func_id << "( ins " << ins_id << " ) is " << f <<
 " ( not " << y << ").";
    }
}

TEST_F(BaseTest, problems_pbo)
{
    using namespace ioh::problem::pbo;
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<OneMax>(1, 9)), 5.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<OneMaxDummy1>(1, 9)), 3.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<OneMaxDummy2>(1, 9)), 4.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<OneMaxEpistasis>(1, 9)), 6.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<OneMaxNeutrality>(1, 9)), 2.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<OneMaxRuggedness1>(1, 9)), 4.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<OneMaxRuggedness2>(1, 9)), 6.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<OneMaxRuggedness3>(1, 9)), 7.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<LeadingOnes>(1, 9)), 2.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<LeadingOnesDummy1>(1, 9)), 3.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<LeadingOnesDummy2>(1, 9)), 2.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<LeadingOnesEpistasis>(1, 9)), 0.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<LeadingOnesNeutrality>(1, 9)), 1.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<LeadingOnesRuggedness1>(1, 9)), 2.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<LeadingOnesRuggedness2>(1, 9)), 1.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<LeadingOnesRuggedness3>(1, 9)), 1.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<Linear>(1, 9)), 24.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<MIS>(1, 9)), -4.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<LABS>(1, 9)), 2.5312);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<NQueens>(1, 9)), -16.0000);

    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<IsingRing>(1, 9)), 5.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<IsingTorus>(1, 9)), 6.0000);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<IsingTriangular>(1, 9)), 9.0000);
}

TEST_F(BaseTest, xopt_equals_yopt_pbo)
{
    const auto& problem_factory = ioh::problem::ProblemRegistry<ioh::problem::PBO>::instance();
    for (const auto& name : problem_factory.names())
    {
        if (name != "LABS" && name != "NQueens" && name != "LeadingOnesEpistasis" && name != "MIS" && name != "NKLandscapes")
        {
            auto problem = problem_factory.create(name, 1, 16);
            EXPECT_DOUBLE_EQ(problem->objective().y, (*problem)(problem->objective().x)) << *problem;
        }
    }
}