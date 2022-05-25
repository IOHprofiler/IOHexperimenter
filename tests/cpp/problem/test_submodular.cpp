#include "../utils.hpp"

#include "ioh/problem.hpp"


void test_submodular_problems(const std::string &type, const std::vector<double> &results)
{

    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::submodular::GraphProblem>::instance();

    int i = 0;
    for (auto &[id, name] : problem_factory.map())
    {
        if (name.substr(0, type.size()) == type)
        {
            auto problem = problem_factory.create(id, 1, 1);
            ioh::common::random::seed(10);
            auto y0 =
                std::round((*problem)(ioh::common::random::integers(problem->meta_data().n_variables, 0, 1)) * 10.0) /
                10.0;
            EXPECT_DOUBLE_EQ(y0, results.at(i++)) << name;
        }
    }
}


TEST_F(BaseTest, SubmodularMaxCut)
{

    const std::vector<double> results = {9561, 9560, 9573, 9575, 9621};
    test_submodular_problems("MaxCut", results);
}

TEST_F(BaseTest, SubmodularMaxCoverage)
{
    const std::vector<double> results = {-197,   -197,     -263,     -355,     -16058,   -16270,   -25280,
                                         -39624, -1291706, -1319498, -2445316, -4490674, -209.5,   -209.5,
                                         -277.3, -371.5,   -16556.4, -16768.4, -25852.4, -40285.8, -212.4,
                                         -212.4, -280.7,   -375.5,   -16675.5, -16887.5, -25989.1, -40444};

    test_submodular_problems("MaxCoverage", results);
}

TEST_F(BaseTest, SubmodularMaxInfluence)
{
    const std::vector<double> results = {-1980,    -1970,    -1940,   -1890,   -89798,  -89598,   -88998,
                                         -87998,   -2018.6,  -2008.6, -1978.6, -1928.6, -91343.3, -91143.3,
                                         -90543.3, -89543.3, -2027.9, -2017.9, -1987.9, -1937.9,  -91712.6,
                                         -91512.6, -90912.6, -89912.6

    };

    test_submodular_problems("MaxInfluence", results);
}


TEST_F(BaseTest, SubmodularPackWhileTravel)
{
    const std::vector<double> results = {-266230.1, -2117748,  665166.8,   -83935805,   -1014885579.6,
                                         3135759.5, -81782253, -763316471, -228852040.2};


    test_submodular_problems("PackWhileTravel", results);
}
