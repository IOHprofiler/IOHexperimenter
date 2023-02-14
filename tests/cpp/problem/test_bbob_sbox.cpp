#include "../utils.hpp"

#include "ioh/suite.hpp"
#include "ioh/problem/bbob.hpp"


TEST_F(BaseTest, SBOX)
{
    const auto& bbob_factory = ioh::problem::ProblemRegistry<ioh::problem::BBOB>::instance();
    const auto& sbox_factory = ioh::problem::ProblemRegistry<ioh::problem::SBOX>::instance();

    EXPECT_EQ(bbob_factory.ids().size(), sbox_factory.ids().size());

    for (auto &[id, name] : bbob_factory.map()){
        auto bbob = bbob_factory.create(id, 1, 2);
        auto sbox = sbox_factory.create(id, 1, 2);
        EXPECT_EQ(bbob->meta_data(), sbox->meta_data());
        EXPECT_EQ(sbox->constraints().n(), 1);
        EXPECT_EQ((*sbox)({-10, 10}), std::numeric_limits<double>::infinity()) << sbox->meta_data();
        EXPECT_NE((*bbob)({-10, 10}), std::numeric_limits<double>::infinity()) << sbox->meta_data();
        EXPECT_FLOAT_EQ(sbox->optimum().y, bbob->optimum().y);
        

        bool check = true;
        for(const auto& pid: {5, 9, 19, 20}) {   
            if (id == pid){
                check = false;
                break;
            }
        }
        if (!check)
            break;

        for(size_t i = 0; i < sbox->optimum().x.size(); i++){
            auto x = sbox->optimum().x[i];
            auto y = bbob->optimum().x[i];
            auto res = x != y;
            EXPECT_TRUE(res) << x <<  " " << y << " " << (x==y) ;
        }
    }
}


TEST_F(BaseTest, sbox_xopt_equals_yopt_bbob)
{
    const auto& problem_factory = ioh::problem::ProblemRegistry<ioh::problem::SBOX>::instance();
    for (const auto& name : problem_factory.names())
    {
        for (auto instance = 1; instance < 5; instance++)
        {
            auto problem = problem_factory.create(name, instance, 16);
            EXPECT_DOUBLE_EQ(problem->optimum().y, (*problem)(problem->optimum().x)) << *problem;
        }
    }
}


TEST_F(BaseTest, problem_suite_sbox)
{
    std::vector<int> ids(24);
    std::iota(ids.begin(), ids.end(), 1);
    ioh::suite::SBOX bbob(ids, {1}, {4});
    for (const auto &problem : bbob)
        EXPECT_NE((*problem)({0,0,0,0}), std::numeric_limits<double>::infinity());
}


TEST_F(BaseTest, sbox_can_create_problem)
{
    using namespace ioh::problem;
    bbob::Sphere bbob(1, 2);
    bbob::Sphere<SBOX> sbox(1, 2);

    const auto x0 = std::vector<double>(2);
    EXPECT_NE(bbob(x0), sbox(x0));
    EXPECT_EQ(sbox.constraints().n(), 1);
}

TEST_F(BaseTest, sbox_can_create_shared_ptr_problem)
{
    using namespace ioh::problem;
    auto bbob = std::make_shared<bbob::Sphere<BBOB>>(1, 2);
    auto sbox = std::make_shared<bbob::Sphere<SBOX>>(1, 2);

    const auto x0 = std::vector<double>(2);
    EXPECT_NE((*bbob)(x0), (*sbox)(x0));
    EXPECT_EQ(sbox->constraints().n(), 1);
}