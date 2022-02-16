#include "../utils.hpp"

#include "ioh/logger/properties.hpp"

using namespace ioh;


TEST_F(BaseTest, test_precision) {
    logger::Info info {};
    info.raw_y_best = 1e-8;
    EXPECT_EQ("0.0000000100", watch::raw_y_best.call_to_string(info));
    info.raw_y_best = 1e-9;
    EXPECT_EQ("0.0000000010", watch::raw_y_best.call_to_string(info));
    info.raw_y_best = 1e-10;
    EXPECT_EQ("0.0000000001", watch::raw_y_best.call_to_string(info));
    info.raw_y_best = 1e-11;
    EXPECT_EQ("0.0000000000", watch::raw_y_best.call_to_string(info));
}
