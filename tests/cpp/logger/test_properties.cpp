#include "../utils.hpp"

#include "ioh/logger/properties.hpp"

using namespace ioh;


TEST_F(BaseTest, test_precision) {
    logger::Info info {};
    info.raw_y_best = 1e-8;
    EXPECT_EQ("0.00000001", watch::raw_y_best.call_to_string(info));
    info.raw_y_best = 1e-11;
    EXPECT_EQ("0.00000000", watch::raw_y_best.call_to_string(info));
}
