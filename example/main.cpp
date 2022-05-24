#include "ioh.hpp"


#include "ioh/problem/submodular/pack_while_travel.hpp"

void show_registered_objects()
{
    {
        const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::Real>::instance();
        const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Real>::instance();

        std::cout << "Registered Real Problems:\n";

        for (auto &[id, name] : problem_factory.map())
            std::cout << id << ", " << name << std::endl;

        std::cout << "\nRegistered Real Suites:\n";
        for (auto &[id, name] : suite_factory.map())
            std::cout << id << ", " << name << std::endl;
    }
    {
        const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::Integer>::instance();
        const auto &suite_factory = ioh::suite::SuiteRegistry<ioh::problem::Integer>::instance();

        std::cout << "\nRegistered Integer Problems:\n";

        for (auto &[id, name] : problem_factory.map())
            std::cout << id << ", " << name << std::endl;

        std::cout << "\nRegistered Integer Suites:\n";
        for (auto &[id, name] : suite_factory.map())
            std::cout << id << ", " << name << std::endl;
    }
}

namespace ioh::problem
{
    template <typename GraphProblem>
    std::shared_ptr<GraphProblem> get_graph_problem(const int instance)
    {
        const fs::path path = fs::current_path();
        current_path(common::file::utils::get_static_root());
        auto problem = std::make_shared<GraphProblem>(instance);
        current_path(path);
        return problem;
    }

    void test_and_compare(const std::shared_ptr<Integer> &o, const std::shared_ptr<Integer> &n)
    {
        common::random::seed(10);
        const auto x0 = common::random::integers(o->meta_data().n_variables, 0, 1);

        std::cout << n->meta_data() << " vs. " << o->meta_data() << std::endl;

        const int reps = 1;
        {
            common::CpuTimer t ("old impl:");
            for (auto i = 0; i < reps; i++)
                (*o)(x0);
        }
        {
            common::CpuTimer t("new impl:");
            for (auto i = 0; i < reps; i++)
                (*n)(x0);
        }

        if(o->state().current.y != n->state().current.y)
        {
            std::cout << o->state().current.y << "!=" << n->state().current.y << std::endl;
            assert(false);
        }
    }
    //
    // void test_all()
    // {
    //     const auto &problem_factory = ProblemRegistry<submodular::v2::GraphProblem>::instance();
    //
    //     for (auto &[id, name] : problem_factory.map())
    //     {
    //         if (id < 2400)
    //             continue;
    //
    //         auto problem = problem_factory.create(id, 1, 1);
    //
    //         if (problem->meta_data().problem_id < 2100)
    //         {
    //             test_and_compare(get_graph_problem<submodular::MaxCut>(problem->meta_data().problem_id - 2000 + 1),
    //                              problem);
    //         }
    //         else if (problem->meta_data().problem_id < 2200)
    //         {
    //             test_and_compare(get_graph_problem<submodular::MaxCoverage>(problem->meta_data().problem_id - 2100 + 1),
    //                              problem);
    //         }
    //         else if (problem->meta_data().problem_id < 2300)
    //         {
    //             test_and_compare(
    //                 get_graph_problem<submodular::MaxInfluence>(problem->meta_data().problem_id - 2200 + 1), problem);
    //         }
    //         else if (problem->meta_data().problem_id < 2400)
    //         {
    //             test_and_compare(
    //                 get_graph_problem<submodular::PackWhileTravel>(problem->meta_data().problem_id - 2300 + 1),
    //                 problem);
    //         }
    //     }
    // }
} // namespace ioh::problem


void example_submodular()
{
    using namespace ioh::problem;

    // we first create a problem factory, only for GraphProblem Types
    const auto &problem_factory = ProblemRegistry<submodular::v2::GraphProblem>::instance();

    // We loop over all problems, for now I only did the 3 types.
    for (auto &[id, name] : problem_factory.map())
    {
        // Creating a problem instance
        auto problem = problem_factory.create(id, 1, 1);
        std::cout << problem->meta_data() << std::endl;

        // We can then call the problem with an array of integers a would be expected
        auto x0 = ioh::common::random::integers(problem->meta_data().n_variables, 0, 1);
        std::cout << "y0: " << (*problem)(x0) << std::endl;
    }
}


// Example
int main()
{
    using namespace ioh;
    using namespace common;
    using namespace problem::submodular::v2;
    
    
    int instance = 1;
    auto pwt_graphs = file::as_text_vector_ptr<graph::pwt::TTPGraph>(
        file::utils::find_static_file("example_list_pwt")
        );
    auto graph = pwt_graphs.at(instance -1);
    
    auto problem = problems::PackWhileTravel(1, 1, graph);
    std::cout << problem.meta_data() << std::endl;
    
    
    // std::cout << problem.meta_data().n_variables << std::endl;
    auto x0 = random::integers(problem.meta_data().n_variables, 0, 1);
    std::cout << problem(x0) << std::endl;
    
    
    auto gp = problem::get_graph_problem<problem::submodular::PackWhileTravel>(instance);
    std::cout << gp->meta_data() << std::endl;
    std::cout << (*gp)(x0) << std::endl;
    
    
    auto ttp_data = std::dynamic_pointer_cast<graph::pwt::TTPGraph>(problem.graph)->ttp_data;
    auto p1 = ttp_data.city_map[0];
    // auto p2 = gp->index_map[0];
    //
    //
    // for (size_t i = 0; i < ttp_data.distances.size(); i++)
    //     assert(ttp_data.distances[i] == (*gp->distances)[i]);
    //
    // assert(ttp_data.capacity == gp->capacity);
    // assert(ttp_data.penalty == gp->penalty);
    // assert(ttp_data.velocity_gap == gp->velocity_gap);



}
