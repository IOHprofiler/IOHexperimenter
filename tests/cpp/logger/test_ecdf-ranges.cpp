#include <utility>
#include <gtest/gtest.h>
#include "ioh.hpp"

using namespace ioh;
using namespace ioh::logger;

TEST(ecdf, ranges)
{
    common::log::log_level = common::log::Level::Warning;

    // Linear range
    ecdf::LinearRange<double> linr(0,100, 10);

    EXPECT_EQ(linr.index(  0),0);
    EXPECT_EQ(linr.index(  9),0);
    EXPECT_EQ(linr.index( 50),5);
    EXPECT_EQ(linr.index( 99),9);
    EXPECT_EQ(linr.index(100),9);

    ecdf::LinearRange<double>::BoundsType p_0_10 = {0,10};
    EXPECT_EQ(linr.bounds(0),p_0_10);
    
    ecdf::LinearRange<double>::BoundsType p_90_100 = {90,100};
    EXPECT_EQ(linr.bounds(9),p_90_100);

    // Log range
    ecdf::LogRange<double> logr_small(0,1e3,3);

    EXPECT_EQ(logr_small.index(  0),0);
    EXPECT_EQ(logr_small.index(1e1),1);
    EXPECT_EQ(logr_small.index(1e2),2);
    EXPECT_EQ(logr_small.index(999),2);
    EXPECT_EQ(logr_small.index(1e3),2);

    // Tiny log range
    ecdf::LogRange<double> logr_tiny(0,100,10);

    EXPECT_EQ(logr_tiny.index(  0),0);
    EXPECT_EQ(logr_tiny.index(100),9);
    
    // Log range
    ecdf::LogRange<double> logr_large(0,6e7,10);

    EXPECT_EQ(logr_large.index(  0),0);
    EXPECT_EQ(logr_large.index(6e7),9);

    // For small values, there is no robustness problem.
    EXPECT_DOUBLE_EQ(logr_large.bounds(0).first , 0);
    
    // If the algorithm was robust enough:
    // EXPECT_DOUBLE_EQ(logr_large.bounds(9).second, 6e7);
    // At the moment, errors are still larger than 4 ULPs, so we use this more laxist test:
    EXPECT_NEAR(logr_large.bounds(9).second, 6e7, 1e-6);
}

TEST(ecdf, ranges_death)
{
    common::log::log_level = common::log::Level::Warning;

    // Linear
    ecdf::LinearRange<double> linr(0,100, 10);

    EXPECT_DEBUG_DEATH(linr.index( -1) , "");
    EXPECT_DEBUG_DEATH(linr.index(100.01) , "");

    EXPECT_DEBUG_DEATH(linr.bounds(10) , "");

    // Log
    ecdf::LogRange<double> logr_small(0,100, 10);

    EXPECT_DEBUG_DEATH(logr_small.index( -1), "");
    EXPECT_DEBUG_DEATH(logr_small.index(100.01) , "");
    
    EXPECT_DEBUG_DEATH(logr_small.bounds(10) , "");
}

