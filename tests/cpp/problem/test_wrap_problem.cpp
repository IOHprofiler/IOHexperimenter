#include "../utils.hpp" 

#include "ioh/problem.hpp"

template <typename T>
double fn(const std::vector<T> &x) { 
    return std::accumulate(x.begin(), x.end(), static_cast<T>(0)); 
}

template <typename T>
ioh::problem::Solution<T> co(const int iid, const int dim)
{
    return {std::vector<T>(dim, iid), static_cast<double>(iid * dim)};
}

template <typename T>
std::vector<T> tx(std::vector<T> x, const int iid){
    x.at(0) = static_cast<T>(iid);
    return x;
}

double ty(const double y, const int iid){
    return y * iid;
}

TEST_F(BaseTest, test_wrap_real_problem){
    using namespace ioh::common;
    using namespace ioh::problem;
    auto &factory = ProblemRegistry<Real>::instance();
    
    wrap_function<double>(fn<double>, "fn", OptimizationType::MIN, -5, 5, tx<double>, ty, co<double>);
    const std::vector<double> x0 = {1, 0, 2};

    for (auto inst: {1, 2, 3}){
        auto problem = factory.create("fn", inst, 3);
        EXPECT_EQ(-5, problem->bounds().lb.at(0));
        EXPECT_EQ(5, problem->bounds().ub.at(0));

        EXPECT_DOUBLE_EQ(static_cast<double>(inst), problem->optimum().x.at(0));
        EXPECT_DOUBLE_EQ(static_cast<double>(inst) * 3, problem->optimum().y);
        EXPECT_DOUBLE_EQ((fn<double>(x0) + inst - 1) * inst, (*problem)(x0));
    }
}

TEST_F(BaseTest, test_wrap_integer_problem){
    using namespace ioh::common;
    using namespace ioh::problem;
    auto &factory = ProblemRegistry<Integer>::instance();
    
    wrap_function<int>(fn<int>, "fn", OptimizationType::MIN, -5, 5, tx<int>, ty, co<int>);
    const std::vector<int> x0 = {1, 0, 2};

    for (auto inst: {1, 2, 3}){
        auto problem = factory.create("fn", inst, 3);
        EXPECT_EQ(-5, problem->bounds().lb.at(0));
        EXPECT_EQ(5, problem->bounds().ub.at(0));

        EXPECT_EQ(static_cast<double>(inst), problem->optimum().x.at(0));
        EXPECT_DOUBLE_EQ(static_cast<double>(inst) * 3, problem->optimum().y);
        EXPECT_DOUBLE_EQ((fn<int>(x0) + inst - 1) * inst, (*problem)(x0));
    }
}