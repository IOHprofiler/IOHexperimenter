#include "../utils.hpp"

#include "ioh/problem/bbob.hpp"
#include "ioh/problem/pbo.hpp"
#include "ioh/problem/submodular.hpp"

TEST_F(BaseTest, BoxBBOBConstraint)
{
    using namespace ioh::problem::bbob;

    Sphere p(1, 1);

    EXPECT_FLOAT_EQ((float)p({10.}), 174.48792f);
    EXPECT_FLOAT_EQ((float)p.constraints().violation(), 0.f);

    p.enforce_bounds(std::numeric_limits<double>::infinity());
 
	EXPECT_EQ(p({10}), std::numeric_limits<double>::infinity());
    EXPECT_FLOAT_EQ((float)p.constraints().violation(), 25.f);
	
	EXPECT_EQ(p({-10}), std::numeric_limits<double>::infinity());
    EXPECT_FLOAT_EQ((float)p.constraints().violation(), 25.f);

    // Hard penalty, return the value for penalty on violation, and don't call
    // internal evaluate function. So, y and all internally used values are expected to be the penalty
    p.enforce_bounds(1.0, ioh::problem::constraint::Enforced::HARD);
    EXPECT_EQ(p({10}), p.constraints().penalty());
    EXPECT_EQ(p.state().current_internal.y, p.constraints().penalty());
    EXPECT_EQ(p.state().y_unconstrained, p.constraints().penalty());

    // Return only penalize(y), default implementation of penalize(y) is y + p, so inf + p == inf
    p.enforce_bounds(1.0, ioh::problem::constraint::Enforced::OVERRIDE);
    EXPECT_EQ(p({10}), std::numeric_limits<double>::infinity());

}

TEST_F(BaseTest, BoundsAsConstraints) {
    using namespace ioh::problem::bbob;

    Sphere p(1, 1);

    p.enforce_bounds(std::numeric_limits<double>::infinity());
    EXPECT_EQ(p({10.}), std::numeric_limits<double>::infinity());
    
    p.enforce_bounds(1.0);
    EXPECT_FLOAT_EQ((float)p({10.}), (float)p.state().y_unconstrained + 25);

    EXPECT_EQ(p.constraints().n(), 1);
}

TEST_F(BaseTest, BoxPBOConstraint)
{
    using namespace ioh::problem::pbo;

    LeadingOnes p(1, 1);
    EXPECT_FLOAT_EQ((float)p({1}), 1);
    EXPECT_FLOAT_EQ((float)p({2}), 0);
    EXPECT_FLOAT_EQ((float)p.constraints().violation(), 0.f);

    p.enforce_bounds(-std::numeric_limits<double>::infinity());

    EXPECT_EQ(p({2}), -std::numeric_limits<double>::infinity());
    EXPECT_FLOAT_EQ((float)p.constraints().violation(), 1.f);

    EXPECT_EQ(p({-2}), -std::numeric_limits<double>::infinity());
    EXPECT_FLOAT_EQ((float)p.constraints().violation(), 4.f);
}

//! constraint on x: x[i-1] < x[i]
double x_cons(const std::vector<double> &x)
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
    
    auto c1 = std::make_shared<FunctionalConstraint<double>>(x_cons, 10);

    Sphere p(1, 2);
    std::vector<double> x0{10.0, 1.};
    double y0 = p(x0);

    p.add_constraint(c1);

    EXPECT_FLOAT_EQ((float)y0 + 10 * (float)x_cons(x0), (float)p(x0));

    p.remove_constraint(c1);

    EXPECT_FLOAT_EQ((float)y0, (float)p(x0));
}

TEST_F(BaseTest, GraphConstraint) {

    const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::submodular::GraphProblem>::instance();
    auto problem = problem_factory.create(2100, 10, 1);


    // Default behavioir, hard constraint, return only penalty
    const auto x1 = std::vector<int>(problem->meta_data().n_variables, 1);
    const auto y1 = (*problem)(x1);
    EXPECT_FLOAT_EQ((float)y1, (float)problem->constraints().penalty());

    auto x0 = std::vector<int>(problem->meta_data().n_variables, 0);
    x0[0] = 1;
    
    problem->enforce_bounds(1.);
    
    const auto y0 = (*problem)(x0);
    EXPECT_FLOAT_EQ((float)y0, 81.f);
    EXPECT_FLOAT_EQ((float)problem->constraints()[0]->violation(), 1.f);
    EXPECT_FLOAT_EQ((float)problem->constraints().penalty(), 0.f);
    EXPECT_FLOAT_EQ((float)problem->constraints()[1]->violation(), 0.f);
    EXPECT_FLOAT_EQ((float)problem->constraints().violation(), 1.f);

    // Turn of bound constraint
    problem->enforce_bounds(0);
        
    // Turn graph constraint into a hidden constraint, so only compute it but not enforce it. 
    problem->constraints()[0]->enforced = ioh::problem::constraint::Enforced::HIDDEN;
    const auto y2 = (*problem)(x1);

    EXPECT_FLOAT_EQ((float)y2, (float)problem->state().y_unconstrained);
    EXPECT_FLOAT_EQ((float)problem->constraints()[0]->penalty(), -440.f);
    EXPECT_FLOAT_EQ((float)problem->constraints()[1]->violation(), 0.f);
    EXPECT_FLOAT_EQ((float)problem->constraints().violation(), 450.f);

    // Turn contraint to a soft one (y + p)
    problem->constraints()[0]->enforced = ioh::problem::constraint::Enforced::SOFT;
    const auto y3 = (*problem)(x1);
    EXPECT_FLOAT_EQ((float)y3, (float)problem->state().y_unconstrained + (float)problem->constraints()[0]->penalty());


    // Multi-Objective definition
    problem->constraints()[0]->enforced = ioh::problem::constraint::Enforced::HARD;
    problem->constraints()[0]->weight = -1.;
    problem->constraints()[0]->exponent = 0.;

    // Invalid point gets -1
    const auto y4 = (*problem)(x1);
    EXPECT_FLOAT_EQ((float)y4, -1.);
    EXPECT_FLOAT_EQ((float)problem->constraints()[0]->violation(), 450.f);

    // Valid point get f(x)
    EXPECT_FLOAT_EQ((float)(*problem)(x0), 81.f);
    EXPECT_FLOAT_EQ((float)problem->constraints()[0]->violation(), 1.f);
    EXPECT_FLOAT_EQ((float)problem->constraints().penalty(), 0.f);
}