#include <iostream>

#include "ioh.hpp"
#include <gtest/gtest.h> 


using ioh::logger::Observer;

TEST(observer, always) {
    const auto o1 = Observer(true);
    EXPECT_TRUE(o1.trigger_always());
    const auto o2 = Observer(false);
    EXPECT_FALSE(o2.trigger_always());
}


TEST(observer, improvement) {
    auto observer = Observer(false);
    EXPECT_TRUE(observer.improvement_trigger(100.0));
    EXPECT_FALSE(observer.improvement_trigger(100.0));
    EXPECT_FALSE(observer.improvement_trigger(101.0));
    EXPECT_TRUE(observer.improvement_trigger(99.0));
}

TEST(observer, interval) {
    const auto observer = Observer(false, 3);
    EXPECT_TRUE(observer.interval_trigger(1));
    EXPECT_FALSE(observer.interval_trigger(2));
    EXPECT_TRUE(observer.interval_trigger(3));
    EXPECT_FALSE(observer.interval_trigger(2));
    EXPECT_TRUE(observer.interval_trigger(6));
    EXPECT_FALSE(observer.interval_trigger(2));
    EXPECT_TRUE(observer.interval_trigger(9));
}


TEST(observer, time_points) {
    auto observer = Observer(false, 0, 0,
                               false, {1, 5}
        );

    EXPECT_TRUE(observer.time_points_trigger(1));
    EXPECT_FALSE(observer.time_points_trigger(2));

    EXPECT_TRUE(observer.time_points_trigger(5));
    EXPECT_FALSE(observer.time_points_trigger(6));

    EXPECT_TRUE(observer.time_points_trigger(10));
    EXPECT_FALSE(observer.time_points_trigger(11));

    EXPECT_TRUE(observer.time_points_trigger(50));
    EXPECT_FALSE(observer.time_points_trigger(51));

    EXPECT_TRUE(observer.time_points_trigger(100));
    EXPECT_FALSE(observer.time_points_trigger(110));

    EXPECT_TRUE(observer.time_points_trigger(500));
    EXPECT_FALSE(observer.time_points_trigger(510));

    auto observer_base5 = Observer(false, 0, 0,
                                     false, {1, 3},
                                     ioh::common::OptimizationType::Minimization, 5);

    EXPECT_TRUE(observer_base5.time_points_trigger(1));
    EXPECT_FALSE(observer_base5.time_points_trigger(2));
    EXPECT_TRUE(observer_base5.time_points_trigger(3));
    EXPECT_FALSE(observer_base5.time_points_trigger(4));

    EXPECT_TRUE(observer_base5.time_points_trigger(15));
    EXPECT_FALSE(observer_base5.time_points_trigger(16));

    EXPECT_TRUE(observer_base5.time_points_trigger(25));
    EXPECT_FALSE(observer_base5.time_points_trigger(26));
    EXPECT_TRUE(observer_base5.time_points_trigger(75));
    EXPECT_FALSE(observer_base5.time_points_trigger(76));
}


TEST(observer, time_range) {
    auto observer = Observer(false, 0, 
        5);

    EXPECT_TRUE(observer.time_range_trigger(1));
    EXPECT_FALSE(observer.time_range_trigger(2));
    EXPECT_TRUE(observer.time_range_trigger(3));
    EXPECT_FALSE(observer.time_range_trigger(4));
    EXPECT_TRUE(observer.time_range_trigger(5));
    EXPECT_FALSE(observer.time_range_trigger(6));
    EXPECT_TRUE(observer.time_range_trigger(7));
    EXPECT_FALSE(observer.time_range_trigger(8));
    EXPECT_TRUE(observer.time_range_trigger(9));



    EXPECT_TRUE(observer.time_range_trigger(10));
    EXPECT_FALSE(observer.time_range_trigger(25));

    EXPECT_TRUE(observer.time_range_trigger(30));
    EXPECT_FALSE(observer.time_range_trigger(40));
    EXPECT_TRUE(observer.time_range_trigger(50));
    EXPECT_FALSE(observer.time_range_trigger(51));
    EXPECT_TRUE(observer.time_range_trigger(70));
    EXPECT_FALSE(observer.time_range_trigger(81));
    EXPECT_TRUE(observer.time_range_trigger(90));
   
}
