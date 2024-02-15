#include "../utils.hpp"
#include "ioh/problem/cec.hpp"



TEST_F(BaseTest, test_cec2013)
{
    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::CEC2013>::instance();
    const auto ids = problem_factory.ids();


    EXPECT_EQ(problem_factory.names().size(), 20);
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


        auto instance = problem_factory.create(func_id, ins_id, static_cast<int>(x.size()));
        auto y = (*instance)(x);
        EXPECT_NEAR(f, y, 1e-8)
            << "The fitness of function " << func_id << "( ins "
            << ins_id << " ) is " << f << " ( not " << y << ").";
    }
}

TEST_F(BaseTest, xopt_equals_yopt_cec2013)
{
    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::CEC2013>::instance();
    for (const auto &name : problem_factory.names())
    {
        auto instance = problem_factory.create(name, 1, 10);
        const auto y = (*instance)(instance->optimum().x);

        // std::cout << name << std::endl;
        // ioh::common::print(instance->optimum().x);
        // std::cout << y << std::endl;
        EXPECT_NEAR(instance->optimum().y, y, 1e-8) << *instance;
    }
}
