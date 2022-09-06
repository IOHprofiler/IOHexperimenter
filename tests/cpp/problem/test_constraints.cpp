#include "../utils.hpp"

#include "ioh/problem/bbob.hpp"
#include "ioh/problem/pbo.hpp"
#include "ioh/problem/submodular.hpp"

TEST_F(BaseTest, BoxBBOBConstraint)
{

    using namespace ioh::problem::bbob;

    Sphere p(1, 1);

    EXPECT_FLOAT_EQ(p({10.}), 174.48792f);
    EXPECT_FLOAT_EQ(p.constraints().violation(), 0);

    p.enforce_bounds(std::numeric_limits<double>::infinity());
 
	EXPECT_EQ(p({10}), std::numeric_limits<double>::infinity());
    EXPECT_FLOAT_EQ(p.constraints().violation(), 25);
	
	EXPECT_EQ(p({-10}), std::numeric_limits<double>::infinity());
    EXPECT_FLOAT_EQ(p.constraints().violation(), 25);
}

TEST_F(BaseTest, BoundsAsConstraints) {
    using namespace ioh::problem::bbob;

    Sphere p(1, 1);

    p.enforce_bounds(std::numeric_limits<double>::infinity());
    EXPECT_EQ(p({10.}), std::numeric_limits<double>::infinity());
    
    p.enforce_bounds(1.0);
    EXPECT_FLOAT_EQ(p({10.}), p.state().y_unconstrained + 25);

    EXPECT_EQ(p.constraints().n(), 1);
}

TEST_F(BaseTest, BoxPBOConstraint)
{

    using namespace ioh::problem::pbo;

    LeadingOnes p(1, 1);
    EXPECT_FLOAT_EQ(p({1}), 1);
    EXPECT_FLOAT_EQ(p({2}), 0);
    EXPECT_FLOAT_EQ(p.constraints().violation(), 0);

    p.enforce_bounds(-std::numeric_limits<double>::infinity());

    EXPECT_EQ(p({2}), -std::numeric_limits<double>::infinity());
    EXPECT_FLOAT_EQ(p.constraints().violation(), 1);

    EXPECT_EQ(p({-2}), -std::numeric_limits<double>::infinity());
    EXPECT_FLOAT_EQ(p.constraints().violation(), 4);
}

//! constraint on y: y < 35
double y_cons(const std::vector<double> &x, const double y) { 
    return std::max(0.0, y - 35.); 
}

//! constraint on x: x[i-1] < x[i]
double x_cons(const std::vector<double> &x, const double y)
{
    double violation = 0.0;
    for (size_t i = 1; i < x.size(); i++)
        violation += std::max(0.0, x[i] - x[i - 1]);
    return violation;
}

TEST_F(BaseTest, FunctionalConstraint) {
    using namespace ioh::problem::bbob;
    using namespace ioh::problem;
    using namespace ioh::problem::constraint;
    
    auto c1 = std::make_shared<FunctionalConstraint<double>>(y_cons, 1.0, Enforced::SOFT,"c1");
    auto c2 = std::make_shared<FunctionalConstraint<double>>(x_cons, 10, Enforced::SOFT, "c2");

    Sphere p(1, 2);
    std::vector<double> x0{10.0, 1.};
    double y0 = p(x0);
    
    p.add_constraint(c1);

    EXPECT_FLOAT_EQ(y0 + y_cons(x0, y0), p(x0));

    p.remove_constraint(c1);
    
    EXPECT_FLOAT_EQ(y0, p(x0));

    p.add_constraint(c2);

    EXPECT_FLOAT_EQ(y0 + 10 * x_cons(x0, y0), p(x0));

    p.add_constraint(c1);

    EXPECT_FLOAT_EQ(y0 + (10 * x_cons(x0, y0)) + y_cons(x0, y0), p(x0));
}



TEST_F(BaseTest, GraphConstraint) {

    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::submodular::GraphProblem>::instance();
    auto problem = problem_factory.create(2100, 10, 1);

    const auto x1 = std::vector<int>(problem->meta_data().n_variables, 1);
    const auto y1 = (*problem)(x1);
    EXPECT_FLOAT_EQ(y1, problem->constraints().violation());

    auto x0 = std::vector<int>(problem->meta_data().n_variables, 0);
    x0[0] = 1;
    problem->enforce_bounds(1.);
    
    const auto y0 = (*problem)(x0);

    EXPECT_FLOAT_EQ(y0, 81.);
    EXPECT_FLOAT_EQ(problem->constraints()[0]->violation(), 1.);
    EXPECT_FLOAT_EQ(problem->constraints()[1]->violation(), 0.);
    EXPECT_FLOAT_EQ(problem->constraints().violation(), 1.);
}