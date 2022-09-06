#include "../utils.hpp"

#include "ioh/problem/bbob.hpp"

double test_eval(const std::shared_ptr<ioh::problem::Real> &f)
{
    return std::round((*f)(ioh::common::random::pbo::normal(4, 42)) * 10000.0) / 10000.0;
}

TEST_F(BaseTest, BBOBfitness_dimension5)
{
    const auto file_name = ioh::common::file::utils::find_static_file("bbobfitness5.in");
    std::string s;

    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::BBOB>::instance();

    std::ifstream infile(file_name.c_str());
    ASSERT_TRUE(infile.is_open());
    while (getline(infile, s))
    {
        auto tmp = split(s, " ");
        auto func_id = stoi(tmp[0]);
        auto ins_id = stoi(tmp[1]);
        auto x = string_to_vector_double(tmp[2]);
        auto f = stod(tmp[3]);
        auto problem = problem_factory.create(func_id, ins_id, 5);
        auto y = (*problem)(x);
        EXPECT_LE(abs(y - f) / f, 1.0/pow(10,6-log(10))) << "The fitness of function " 
             << func_id << "( ins " << ins_id << " ) is " << f << " ( not " << y << ").";
    }
}


TEST_F(BaseTest, BBOBfitness_dimension20)
{
    const auto file_name = ioh::common::file::utils::find_static_file("bbobfitness20.in");
    std::string s;

    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::BBOB>::instance();

    std::ifstream infile(file_name.c_str());
    ASSERT_TRUE(infile.is_open());
    while (getline(infile, s))
    {
        auto tmp = split(s, " ");
        auto func_id = stoi(tmp[0]);
        auto ins_id = stoi(tmp[1]);
        auto x = string_to_vector_double(tmp[2]);
        auto f = stod(tmp[3]);

        auto problem = problem_factory.create(func_id, ins_id, 20);
        auto y = (*problem)(x);
        EXPECT_LE(abs(y - f) / f, 1.0 / pow(10, 6 - log(10))) << "The fitness of function " << func_id << "( ins " <<
 ins_id << " ) is " << f << " ( not " << y << ").";
    }
}

TEST_F(BaseTest, problems_bbob4d)
{
    using namespace ioh::problem::bbob;
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<AttractiveSector>(1, 4)), 38905.1446);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<BentCigar>(1, 4)), 43065603.7200);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<BuecheRastrigin>(1, 4)), -377.5170);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<DifferentPowers>(1, 4)), -28.6647);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<Discus>(1, 4)), 633536.4945);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<Ellipsoid>(1, 4)), 4670466.7111);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<EllipsoidRotated>(1, 4)), 4272872.0872);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<Gallagher101>(1, 4)), 100.6898);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<Gallagher21>(1, 4)), -924.8833);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<GriewankRosenBrock>(1, 4)), -92.2225);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<Katsuura>(1, 4)), 21.7005);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<LinearSlope>(1, 4)), 75.4977);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<LunacekBiRastrigin>(1, 4)), 177.9893);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<Rastrigin>(1, 4)), -351.7320);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<RastriginRotated>(1, 4)), 1880.5481);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<Rosenbrock>(1, 4)), 3990.2021);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<RosenbrockRotated>(1, 4)), 2734.3342);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<Schaffers10>(1, 4)), -3.8932);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<Schaffers1000>(1, 4)), 39.4152);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<Schwefel>(1, 4)), 6135.2508);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<SharpRidge>(1, 4)), 1304.6100);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<Sphere>(1, 4)), 85.3339);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<StepEllipsoid>(1, 4)), 180.6339);
    EXPECT_DOUBLE_EQ(test_eval(std::make_shared<Weierstrass>(1, 4)), 192.2602);
}

TEST_F(BaseTest, problems_bbob5d)
{
    const auto& the_factory = ioh::problem::ProblemRegistry<ioh::problem::BBOB>::instance();
    const std::vector<double> expected{
       161.17445568,
       12653420.412225708,
       -97.06158297486468,
       8845.53524810093,
       121.9133638725643,
       423021.00682286796,
       278.3290201933729,
       4315.032550201522,
       95168.253629631741,
       969025.0451803299,
       20911310.769634742,
       187251637.66430587,
       2198.7155122256763,
       261.60997479957,
       1103.1553337856833,
       273.87870697835791,
       35.410739487431208,
       107.64134358767826,
       -40.269198932753994,
       49739.05388887795,
       116.29585727504872,
       -914.902473409051,
       18.635078550302751,
       1782.2733296400438,
    };
    const auto dimension = 5;
    const std::vector<double> x0{ 0.1, 1., 2.,4., 5.4 };
    for (const auto& name : the_factory.names())
    {
        const auto item = the_factory.create(name, 1, dimension);
        EXPECT_FLOAT_EQ(
            static_cast<float>(expected.at(item->meta_data().problem_id - 1)), 
            static_cast<float>((*item)(x0)))
        << *item;
    }
}

TEST_F(BaseTest, xopt_equals_yopt_bbob)
{
    const auto& problem_factory = ioh::problem::ProblemRegistry<ioh::problem::BBOB>::instance();
    for (const auto& name : problem_factory.names())
    {
        for (auto instance = 1; instance < 5; instance++)
        {
            auto problem = problem_factory.create(name, instance, 16);
            EXPECT_DOUBLE_EQ(problem->optimum().y, (*problem)(problem->optimum().x)) << *problem;
        }
    }
}