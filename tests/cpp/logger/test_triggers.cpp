#include "../utils.hpp"

#include "ioh/logger/triggers.hpp"

using namespace ioh;

TEST_F(BaseTest, trigger_always_single_objective) {

    problem::MetaData pb(0,0,"fake",2);
    logger::Info<problem::SingleObjective> e0;     e0.evaluations = 0;
    logger::Info<problem::SingleObjective> e10;   e10.evaluations = 10;
    logger::Info<problem::SingleObjective> e100; e100.evaluations = 100;

    trigger::Always always;
    EXPECT_TRUE(always(e0,pb));
    EXPECT_TRUE(always(e10,pb));
    EXPECT_TRUE(always(e100,pb));
}

TEST_F(BaseTest, trigger_always_multi_objective) {

    problem::MetaData pb(0,0,"fake",2);
    logger::Info<problem::MultiObjective> e0;     e0.evaluations = 0;
    logger::Info<problem::MultiObjective> e10;   e10.evaluations = 10;
    logger::Info<problem::MultiObjective> e100; e100.evaluations = 100;

    trigger::Always always;
    EXPECT_TRUE(always(e0,pb));
    EXPECT_TRUE(always(e10,pb));
    EXPECT_TRUE(always(e100,pb));
}

TEST_F(BaseTest, trigger_on_improvement_single_objective)
{

    problem::MetaData pb(0,0,"fake",2); // MIN
    logger::Info<problem::SingleObjective> i;
    i.has_improved = true;

    trigger::OnImprovement t;

    // Trigger only based on cached value of has_improved
    EXPECT_TRUE(t(i,pb)); // Better than infinity.

    i.has_improved = false;
    EXPECT_FALSE(t(i,pb)); // Strict inequality.
}


TEST_F(BaseTest, trigger_on_improvement_multi_objective)
{

    problem::MetaData pb(0,0,"fake",2); // MIN
    logger::Info<problem::MultiObjective> i;
    i.has_improved = true;

    trigger::OnImprovement t;

    // Trigger only based on cached value of has_improved
    EXPECT_TRUE(t(i,pb)); // Better than infinity.

    i.has_improved = false;
    EXPECT_FALSE(t(i,pb)); // Strict inequality.
}



TEST_F(BaseTest, trigger_at_single_objective)
{
    problem::MetaData pb(0,0,"fake",2);
    logger::Info<problem::SingleObjective> e0;     e0.evaluations = 0;
    logger::Info<problem::SingleObjective> e10;   e10.evaluations = 10;
    logger::Info<problem::SingleObjective> e11;   e11.evaluations = 11;
    logger::Info<problem::SingleObjective> e99;   e99.evaluations = 99;
    logger::Info<problem::SingleObjective> e100; e100.evaluations = 100;
    logger::Info<problem::SingleObjective> e101; e101.evaluations = 101;

    // auto t = trigger::At( {999,1,100,10} ); // Classical stack instantiation.
    auto& t = trigger::at( {999,1,10,100} ); // On-the-fly heap instantiation.
    EXPECT_FALSE(t(e0  ,pb));
    EXPECT_TRUE (t(e10 ,pb));
    EXPECT_FALSE(t(e11 ,pb));
    EXPECT_FALSE(t(e99 ,pb));
    EXPECT_TRUE (t(e100,pb));
    EXPECT_FALSE(t(e101,pb));
    delete &t; // Having saved the reference, you can delete its address, as `triggers::at` allocated on the heap.
}

TEST_F(BaseTest, trigger_at_multi_objective)
{
    problem::MetaData pb(0,0,"fake",2);
    logger::Info<problem::MultiObjective> e0;     e0.evaluations = 0;
    logger::Info<problem::MultiObjective> e10;   e10.evaluations = 10;
    logger::Info<problem::MultiObjective> e11;   e11.evaluations = 11;
    logger::Info<problem::MultiObjective> e99;   e99.evaluations = 99;
    logger::Info<problem::MultiObjective> e100; e100.evaluations = 100;
    logger::Info<problem::MultiObjective> e101; e101.evaluations = 101;

    // auto t = trigger::At( {999,1,100,10} ); // Classical stack instantiation.
    auto& t = trigger::at( {999,1,10,100} ); // On-the-fly heap instantiation.
    EXPECT_FALSE(t(e0  ,pb));
    EXPECT_TRUE (t(e10 ,pb));
    EXPECT_FALSE(t(e11 ,pb));
    EXPECT_FALSE(t(e99 ,pb));
    EXPECT_TRUE (t(e100,pb));
    EXPECT_FALSE(t(e101,pb));
    delete &t; // Having saved the reference, you can delete its address, as `triggers::at` allocated on the heap.
}



TEST_F(BaseTest, trigger_each_single_objective)
{
    problem::MetaData pb(0,0,"fake",2);
    logger::Info<problem::SingleObjective> e0;     e0.evaluations = 0;
    logger::Info<problem::SingleObjective> e10;   e10.evaluations = 10;
    logger::Info<problem::SingleObjective> e11;   e11.evaluations = 11;
    logger::Info<problem::SingleObjective> e19;   e19.evaluations = 19;
    logger::Info<problem::SingleObjective> e20;   e20.evaluations = 20;
    logger::Info<problem::SingleObjective> e21;   e21.evaluations = 21;
    logger::Info<problem::SingleObjective> e100; e100.evaluations = 100;
    logger::Info<problem::SingleObjective> e101; e101.evaluations = 101;

    auto t0 = trigger::Each(10,0);
    EXPECT_TRUE (t0(e0,pb));
    EXPECT_TRUE (t0(e10,pb));
    EXPECT_FALSE(t0(e11,pb));
    EXPECT_FALSE(t0(e19,pb));
    EXPECT_TRUE (t0(e20,pb));
    EXPECT_FALSE(t0(e21,pb));
    EXPECT_TRUE (t0(e100,pb));
    EXPECT_FALSE(t0(e101,pb));

    auto t1 = trigger::Each(10,1);
    EXPECT_FALSE(t1(e0,pb));
    EXPECT_FALSE(t1(e10,pb));
    EXPECT_TRUE (t1(e11,pb));
    EXPECT_FALSE(t1(e19,pb));
    EXPECT_FALSE(t1(e20,pb));
    EXPECT_TRUE (t1(e21,pb));
    EXPECT_FALSE(t1(e100,pb));
    EXPECT_TRUE (t1(e101,pb));
}


TEST_F(BaseTest, trigger_each_multi_objective)
{
    problem::MetaData pb(0,0,"fake",2);
    logger::Info<problem::MultiObjective> e0;     e0.evaluations = 0;
    logger::Info<problem::MultiObjective> e10;   e10.evaluations = 10;
    logger::Info<problem::MultiObjective> e11;   e11.evaluations = 11;
    logger::Info<problem::MultiObjective> e19;   e19.evaluations = 19;
    logger::Info<problem::MultiObjective> e20;   e20.evaluations = 20;
    logger::Info<problem::MultiObjective> e21;   e21.evaluations = 21;
    logger::Info<problem::MultiObjective> e100; e100.evaluations = 100;
    logger::Info<problem::MultiObjective> e101; e101.evaluations = 101;

    auto t0 = trigger::Each(10,0);
    EXPECT_TRUE (t0(e0,pb));
    EXPECT_TRUE (t0(e10,pb));
    EXPECT_FALSE(t0(e11,pb));
    EXPECT_FALSE(t0(e19,pb));
    EXPECT_TRUE (t0(e20,pb));
    EXPECT_FALSE(t0(e21,pb));
    EXPECT_TRUE (t0(e100,pb));
    EXPECT_FALSE(t0(e101,pb));

    auto t1 = trigger::Each(10,1);
    EXPECT_FALSE(t1(e0,pb));
    EXPECT_FALSE(t1(e10,pb));
    EXPECT_TRUE (t1(e11,pb));
    EXPECT_FALSE(t1(e19,pb));
    EXPECT_FALSE(t1(e20,pb));
    EXPECT_TRUE (t1(e21,pb));
    EXPECT_FALSE(t1(e100,pb));
    EXPECT_TRUE (t1(e101,pb));
}




TEST_F(BaseTest, trigger_during_single_objective)
{
    problem::MetaData pb(0,0,"fake",2);
    logger::Info<problem::SingleObjective> e0;     e0.evaluations = 0;
    logger::Info<problem::SingleObjective> e10;   e10.evaluations = 10;
    logger::Info<problem::SingleObjective> e18;   e18.evaluations = 18;
    logger::Info<problem::SingleObjective> e19;   e19.evaluations = 19;
    logger::Info<problem::SingleObjective> e100; e100.evaluations = 100;
    logger::Info<problem::SingleObjective> e101; e101.evaluations = 101;
    logger::Info<problem::SingleObjective> e102; e102.evaluations = 102;

    auto t = trigger::During({ {10,18}, {101,101} });
    EXPECT_FALSE(t(e0,pb));
    EXPECT_TRUE (t(e10,pb));
    EXPECT_TRUE (t(e18,pb));
    EXPECT_FALSE(t(e19,pb));
    EXPECT_FALSE(t(e100,pb));
    EXPECT_TRUE (t(e101,pb));
    EXPECT_FALSE(t(e102,pb));
}

TEST_F(BaseTest, trigger_during_multi_objective)
{
    problem::MetaData pb(0,0,"fake",2);
    logger::Info<problem::MultiObjective> e0;     e0.evaluations = 0;
    logger::Info<problem::MultiObjective> e10;   e10.evaluations = 10;
    logger::Info<problem::MultiObjective> e18;   e18.evaluations = 18;
    logger::Info<problem::MultiObjective> e19;   e19.evaluations = 19;
    logger::Info<problem::MultiObjective> e100; e100.evaluations = 100;
    logger::Info<problem::MultiObjective> e101; e101.evaluations = 101;
    logger::Info<problem::MultiObjective> e102; e102.evaluations = 102;

    auto t = trigger::During({ {10,18}, {101,101} });
    EXPECT_FALSE(t(e0,pb));
    EXPECT_TRUE (t(e10,pb));
    EXPECT_TRUE (t(e18,pb));
    EXPECT_FALSE(t(e19,pb));
    EXPECT_FALSE(t(e100,pb));
    EXPECT_TRUE (t(e101,pb));
    EXPECT_FALSE(t(e102,pb));
}


