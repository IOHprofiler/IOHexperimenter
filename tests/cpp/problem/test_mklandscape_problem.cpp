#include <gtest/gtest.h>

#include "ioh.hpp"


TEST(MKlandscape, Dummy)
{
    auto cliqueTree = ioh::problem::CliqueTreeC(1, 1, 11,"Test", 5, 3, 1, 2);


    const std::vector<int> x = {0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0};
    EXPECT_EQ(cliqueTree(x), 4.8);
}