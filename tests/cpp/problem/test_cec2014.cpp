#include "../utils.hpp"
#include "ioh/problem/cec.hpp"

TEST_F(BaseTest, test_cec2014)
{
    std::ifstream infile;
    const auto file_path = ioh::common::file::utils::find_static_file("cec_problem2022.in");
    infile.open(file_path.c_str());

    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::CEC2022>::instance();
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
        EXPECT_LE(abs(y - f) / f, 1.0 / pow(10, 6 - log(10)))
            << "The fitness of function " << func_id << "( ins "
            << ins_id << " ) is " << f << " ( not " << y << ").";
    }
}

TEST_F(BaseTest, xopt_equals_yopt_cec2014)
{
    const auto& problem_factory = ioh::problem::ProblemRegistry<ioh::problem::CEC2022>::instance();
    for (const auto& name : problem_factory.names())
    {
        // Any function's, but the composition function's, optimum is defined.
        if (!(name == "CEC2022CompositionFunction1" || name == "CEC2022CompositionFunction2" || name == "CEC2022CompositionFunction3" || name == "CEC2022CompositionFunction4"))
        {
            auto instance = problem_factory.create(name, 1, 10);
            auto&& x = instance->optimum().x;
            EXPECT_DOUBLE_EQ(instance->optimum().y, (*instance)(x)) << *instance;
        }
    }
}
