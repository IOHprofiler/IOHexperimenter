#include "../utils.hpp" 

#include "ioh/problem.hpp"

#include "ioh/problem/submodular/graph_problem.hpp"
#include "ioh/problem/submodular/max_coverage.hpp"
#include "ioh/problem/submodular/max_cut.hpp"
#include "ioh/problem/submodular/max_influence.hpp"
#include "ioh/problem/submodular/pack_while_travel.hpp"


template <typename GraphProblem>
std::shared_ptr<GraphProblem> get_graph_problem(const int instance)
{
    const auto path = fs::current_path();
    current_path(ioh::common::file::utils::get_static_root());
    auto problem = std::make_shared<GraphProblem>(instance);
    current_path(path);
    return problem;
}


void test_and_compare(const std::shared_ptr<ioh::problem::Integer> &o, const std::shared_ptr<ioh::problem::Integer> &n)
{
    ioh::common::random::seed(10);
    const auto x0 = ioh::common::random::integers(o->meta_data().n_variables, 0, 1);

    ioh::common::random::seed(10);
    (*o)(x0);
    ioh::common::random::seed(10);
    (*n)(x0);

    EXPECT_DOUBLE_EQ(o->state().current.y, n->state().current.y) << n->meta_data()
        <<  o->state().current.y << "!=" << n->state().current.y;
}


template <typename  T>
void test_submodular_problems(const std::string& type, const int default_id = 2000)
{
    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::submodular::v2::GraphProblem>::instance();
    for (auto &[id, name] : problem_factory.map())
    {
        if (name.substr(0, type.size()) == type)
        {
            auto problem = problem_factory.create(id, 1, 1);
            test_and_compare(get_graph_problem<T>(problem->meta_data().problem_id - default_id + 1),
                problem);
        }
    }
}


TEST_F(BaseTest, SubmodularMaxCut)
{
    test_submodular_problems<ioh::problem::submodular::MaxCut>("MaxCut", 2000);
}

TEST_F(BaseTest, SubmodularMaxCoverage)
{
    test_submodular_problems<ioh::problem::submodular::MaxCoverage>("MaxCoverage", 2100);
}

TEST_F(BaseTest, SubmodularMaxInfluence)
{
    test_submodular_problems<ioh::problem::submodular::MaxInfluence>("MaxInfluence", 2200);
}


TEST_F(BaseTest, SubmodularPackWhileTravel)
{
    test_submodular_problems<ioh::problem::submodular::PackWhileTravel>("PackWhileTravel", 2300);
    
}


