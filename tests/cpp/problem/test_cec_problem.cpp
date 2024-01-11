#include "../utils.hpp"
#include "ioh/problem/cec.hpp"
#include "ioh/problem/cec/cec_problem.hpp"

std::unique_ptr<ioh::problem::CEC> create_cec_instance(int meta_problem_id, const int instance, const int n_variables) {
    if (meta_problem_id == ioh::problem::cec::CEC_CompositionFunction1::meta_problem_id) { return std::make_unique<ioh::problem::cec::CEC_CompositionFunction1>(instance, n_variables); }
    else if (meta_problem_id == ioh::problem::cec::CEC_CompositionFunction2::meta_problem_id) { return std::make_unique<ioh::problem::cec::CEC_CompositionFunction2>(instance, n_variables); }
    else if (meta_problem_id == ioh::problem::cec::CEC_CompositionFunction3::meta_problem_id) { return std::make_unique<ioh::problem::cec::CEC_CompositionFunction3>(instance, n_variables); }
    else if (meta_problem_id == ioh::problem::cec::CEC_CompositionFunction4::meta_problem_id) { return std::make_unique<ioh::problem::cec::CEC_CompositionFunction4>(instance, n_variables); }
    else if (meta_problem_id == ioh::problem::cec::CEC_ExpandedSchafferF7::meta_problem_id) { return std::make_unique<ioh::problem::cec::CEC_ExpandedSchafferF7>(instance, n_variables); }
    else if (meta_problem_id == ioh::problem::cec::CEC_HybridFunction1::meta_problem_id) { return std::make_unique<ioh::problem::cec::CEC_HybridFunction1>(instance, n_variables); }
    else if (meta_problem_id == ioh::problem::cec::CEC_HybridFunction2::meta_problem_id) { return std::make_unique<ioh::problem::cec::CEC_HybridFunction2>(instance, n_variables); }
    else if (meta_problem_id == ioh::problem::cec::CEC_HybridFunction3::meta_problem_id) { return std::make_unique<ioh::problem::cec::CEC_HybridFunction3>(instance, n_variables); }
    else if (meta_problem_id == ioh::problem::cec::CEC_Levy::meta_problem_id) { return std::make_unique<ioh::problem::cec::CEC_Levy>(instance, n_variables); }
    else if (meta_problem_id == ioh::problem::cec::CEC_Rastrigin::meta_problem_id) { return std::make_unique<ioh::problem::cec::CEC_Rastrigin>(instance, n_variables); }
    else if (meta_problem_id == ioh::problem::cec::CEC_Rosenbrock::meta_problem_id) { return std::make_unique<ioh::problem::cec::CEC_Rosenbrock>(instance, n_variables); }
    else if (meta_problem_id == ioh::problem::cec::CEC_Zakharov::meta_problem_id) { return std::make_unique<ioh::problem::cec::CEC_Zakharov>(instance, n_variables); }
    else { throw std::invalid_argument("Invalid problem ID"); }
}

TEST_F(BaseTest, CECProblem)
{
    std::ifstream infile;
    try
    {
        const auto file_path = ioh::common::file::utils::find_static_file("cec_problem.in");
        infile.open(file_path.c_str());
        ASSERT_TRUE(infile.is_open());
    }
    catch (const std::runtime_error& e)
    {
        LOG("Could not load cec_problem.in. " << e.what());
        std::exit(EXIT_FAILURE);
    }

    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::CEC>::instance();

    std::cerr << "Registered Names in Problem Factory:" << std::endl;
    for (const auto& name : problem_factory.names()) {
        std::cerr << name << std::endl;
    }
    std::cout << "Registered IDs in Problem Factory:" << std::endl;
    for (const auto& id : problem_factory.ids()) {
        std::cerr << id << std::endl;
    }

    std::string s;
    while (getline(infile, s))
    {
        auto tmp = split(s, " ");
        if (tmp.empty()) { continue; }

        auto parsed_meta_problem_id = stoi(tmp[0]);
        auto ins_id = stoi(tmp[1]);
        auto x = string_to_vector_double(tmp[2]);
        auto f = stod(tmp[3]);

        auto instance = create_cec_instance(parsed_meta_problem_id, ins_id, x.size());
        auto y = (*instance)(x);
        EXPECT_LE(abs(y - f) / f, 1.0 / pow(10, 6 - log(10)))
            << "The fitness of function " << parsed_meta_problem_id << "( ins "
            << ins_id << " ) is " << f << " ( not " << y << ").";
    }
}

TEST_F(BaseTest, xopt_equals_yopt_cec)
{
    std::vector<int> cec_problem_ids = {
        1001, 1002, 1003, 1004, 1005,
        1006, 1007, 1008, 1009, 1010,
        1011, 1012
    };
    const auto& problem_factory = ioh::problem::ProblemRegistry<ioh::problem::CEC>::instance();
    for (const auto& enumerated_meta_problem_id : cec_problem_ids)
    {
        // Any function's, but the composition function's, optimum is defined.
        if (!(enumerated_meta_problem_id == 1009 || enumerated_meta_problem_id == 1010 || enumerated_meta_problem_id == 1011 || enumerated_meta_problem_id == 1012))
        {
            auto instance = create_cec_instance(enumerated_meta_problem_id, 1, 10);
            auto&& x = instance->optimum().x;
            EXPECT_DOUBLE_EQ(instance->optimum().y, (*instance)(x)) << *instance;
        }
    }
}
