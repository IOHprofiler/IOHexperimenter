#include "../utils.hpp"
#include "ioh/problem/cec.hpp"



TEST_F(BaseTest, test_cec2013)
{
    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::CEC2013>::instance();
    const auto ids = problem_factory.ids();


    // EXPECT_EQ(problem_factory.names().size(), 20);
    std::ifstream infile;
    const auto file_path = ioh::common::file::utils::find_static_file("cec_problem2013.in");
    infile.open(file_path.c_str());
    
    std::string s;
    while (getline(infile, s))
    {
        auto tmp = split(s, " ");
        if (tmp.empty()) { continue; }
    
        auto func_id = stoi(tmp[0]);
        auto ins_id = stoi(tmp[1]);
        auto x = string_to_vector_double(tmp[2]);
        auto f = stod(tmp[3]);

        if(std::find(ids.begin(), ids.end(), func_id) == ids.end())
            continue;

        auto instance = problem_factory.create(func_id, ins_id, static_cast<int>(x.size()));
        auto y = (*instance)(x);
        EXPECT_NEAR(f, y, 1e-8)
            << "The fitness of function " << func_id << "( ins "
            << ins_id << " ) is " << f << " ( not " << y << ").";
    }
}

TEST_F(BaseTest, inversion_logic) 
{
    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::CEC2013>::instance();
    for (const auto &name : problem_factory.names())
    {
        auto instance = problem_factory.create(name, 1, 10);
        const auto y_opt = (*instance)(instance->optimum().x);

        EXPECT_EQ(instance->meta_data().optimization_type, ioh::common::OptimizationType::MAX);
        instance->invert();
        EXPECT_EQ(instance->meta_data().optimization_type, ioh::common::OptimizationType::MIN);

        EXPECT_NEAR(instance->optimum().y, -y_opt, 1e-8) << *instance;

        for (const auto sol : instance->optima)
        {
            const auto y = (*instance)(sol.x);
            EXPECT_NEAR(instance->optimum().y, y, 1e-8) << *instance;
            EXPECT_NEAR(y_opt, -y, 1e-8) << *instance;
        }
    }
}

TEST_F(BaseTest, test_raw_y_cec2013)
{
    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::CEC2013>::instance();
    for (const auto &name : problem_factory.names())
    {
        auto instance = problem_factory.create(name, 1, 10);
        const auto y = (*instance)(instance->optimum().x);
        EXPECT_NEAR(instance->optimum().y, y, 1e-8) << *instance;
        EXPECT_NEAR(instance->state().current_internal.y, 0.0, 1e-8) << *instance;
        EXPECT_NEAR(instance->state().current.y, instance->optimum().y, 1e-8) << *instance;
        EXPECT_NEAR(instance->log_info().raw_y, 0.0, 1e-8) << *instance;
    }
}

TEST_F(BaseTest, xopt_equals_yopt_cec2013)
{
    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::CEC2013>::instance();
    for (const auto &name : problem_factory.names())
    {
        auto instance = problem_factory.create(name, 1, 10);
        const auto y = (*instance)(instance->optimum().x);

        EXPECT_NEAR(instance->optimum().y, y, 1e-8) << *instance;

        for (const auto sol: instance->optima)
        {
            const auto y = (*instance)(sol.x);
            EXPECT_NEAR(instance->optimum().y, y, 1e-8) << *instance << " " << sol;
        }
    }
}

TEST_F(BaseTest, single_gopt) 
{
    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::CEC2013>::instance();
    for (const auto &name : problem_factory.names())
    {
        auto instance = problem_factory.create(name, 1, 10);
        const auto y_opt = (*instance)(instance->optimum().x);


        for (size_t i = 0; i < instance->n_optima; i++)
        {
            instance->set_optimum(i, true);

            EXPECT_NEAR((*instance)(instance->optimum().x), y_opt, 1e-8) << *instance;

            for (size_t j = 0; j < instance->n_optima; j++)
            {
                if (i == j) continue;
                const auto& sol = instance->optima[j];
                const auto y_sol = (*instance)(sol.x);
                
                // Check that the distance from the true global optimum is correct
                ASSERT_NEAR(instance->optimum().y - instance->sphere_limit, y_sol, 1e-8) << *instance << "\n " << sol;

                // Check that this is correctly changed in each optima.y
                ASSERT_NEAR(sol.y, y_sol, 1e-8) << *instance << "\n " << sol;            
            }   
        }
    }

}
