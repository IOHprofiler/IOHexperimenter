#include "ioh.hpp"


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
        fs::current_path(ioh::common::file::utils::get_static_root());
        auto problem = std::make_shared<GraphProblem>(instance);
        fs::current_path(path);
        return problem;
    }

    void test_and_compare(const std::shared_ptr<Integer> &o, const std::shared_ptr<Integer> &n)
    {
        ioh::common::random::seed(10);
        auto x0 = ioh::common::random::integers(o->meta_data().n_variables, 0, 1);

        std::cout << n->meta_data() << std::endl;
        std::cout << o->meta_data() << std::endl;


        const int reps = 1;
        {
            ioh::common::CpuTimer t;
            std::cout << "old impl: ";
            for (int i = 0; i < reps; i++)
                (*o)(x0);
        }

        {
            ioh::common::CpuTimer t;
            std::cout << "\tnew impl: ";
            for (int i = 0; i < reps; i++)
                (*n)(x0);
        }


        std::cout << "\nold:" << o->state().current.y << std::endl;
        std::cout << "new:" << n->state().current.y << std::endl;
    }

    void test_all()
    {

        const auto &problem_factory = ProblemRegistry<submodular::v2::GraphProblem>::instance();

        for (auto &[id, name] : problem_factory.map())
        {
            auto problem = problem_factory.create(id, 1, 1);

            if (problem->meta_data().problem_id < 2100)
                ioh::problem::test_and_compare(
                    get_graph_problem<submodular::MaxCut>(problem->meta_data().problem_id - 2000 + 1), problem);

            else if (problem->meta_data().problem_id < 2200)
                ioh::problem::test_and_compare(
                    get_graph_problem<submodular::MaxCoverage>(problem->meta_data().problem_id - 2100 + 1), problem);

            else if (problem->meta_data().problem_id < 2300)
                ioh::problem::test_and_compare(
                    get_graph_problem<submodular::MaxInfluence>(problem->meta_data().problem_id - 2200 + 1), problem);
        }
    }
} // namespace ioh::problem


// Example
int main()
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