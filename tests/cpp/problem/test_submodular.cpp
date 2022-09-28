#include "../utils.hpp"

#include "ioh/problem.hpp"


std::vector<int> half_zero(const int size)
{
    std::vector<int> x(size, 0);
    for (auto i = 0; i < (size / 2); i++)
        x[i] = 1; 
    return x;
}

void test_submodular_problems(const std::string &type, const std::vector<double> &results)
{

    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::submodular::GraphProblem>::instance();

    int i = 0;
    for (auto &[id, name] : problem_factory.map())
    {
        if (name.substr(0, type.size()) == type)
        {
            auto problem = problem_factory.create(id, 1, 1);
            auto y0 = std::round((*problem)(half_zero(problem->meta_data().n_variables)) * 10.0) / 10.0;
            EXPECT_DOUBLE_EQ(y0, results.at(i++)) << name;
            break;
        }
    }
}


TEST_F(BaseTest, SubmodularMaxCut)
{
    const std::vector<double> results = {9586, 9622, 9510, 9548, 9590};
    test_submodular_problems("MaxCut", results);
}

TEST_F(BaseTest, SubmodularMaxCoverage)
{
    const std::vector<double> results = {

        -215,     -215,     -287,   -370,   -17873,   -17762,   -27331,   -40317,   -1482525, -1441376,
        -2617055, -4481295, -228,   -228,   -301.9,   -386.9,   -18392.6, -18281.6, -27928,   -40992.3,
        -231.1,   -231.1,   -305.5, -390.9, -18516.8, -18405.8, -28070.7, -41153.7,

    };

    test_submodular_problems("MaxCoverage", results);
}

TEST_F(BaseTest, SubmodularMaxInfluence)
{
    const std::vector<double> results = {
        -2009,   -1999,   -1969,   -1919,   -85823,   -85623,   -85023,   -84023,
        -2047.9, -2037.9, -2007.9, -1957.9, -87379.5, -87179.5, -86579.5, -85579.5,
        -2057.2, -2047.2, -2017.2, -1967.2, -87751.5, -87551.5, -86951.5, -85951.5,

    };

    test_submodular_problems("MaxInfluence", results);
}

TEST_F(BaseTest, SubmodularPackWhileTravel)
{
    const std::vector<double> results = {
        -257680.1, -2135849, 673789.2, -83966736, -1014996648.6, 3174860.5, -81768787, -763482837, -229277375,
    };

    test_submodular_problems("PackWhileTravel", results);
}
