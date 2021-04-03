#include <list>
#include <gtest/gtest.h>
#include "ioh.hpp"


TEST(ecdf, example)
{
	using namespace ioh::problem;
	using namespace ioh::logger;
	ioh::common::log::log_level = ioh::common::log::Level::Warning;
	
	auto sample_size = 100;

	ioh::suite::BBOB suite({1, 2}, {1, 2}, {2, 10});
    ECDF logger(
        0, 6e7, 20, 
        0, sample_size, 20
    );

	suite.attach_logger(logger);

	// TODO: Check if these values are correct
	std::list<size_t> attainments_sum = {
	    334, 662, 951, 1240, 1528, 1816, 2071, 2326,
	    2535, 2771, 2834, 2901, 2966, 3033, 3128, 3223
	};
	
	for (const auto& p : suite) {
        for (auto r = 0; r < 2; r++)
        {
            for (auto s = 0; s < sample_size; ++s)
                (*p)(ioh::common::Random::uniform(p->meta_data().n_variables));
            
            ASSERT_EQ(ECDFSum()(logger.data()), attainments_sum.front());
            p->reset();
            attainments_sum.pop_front();
        }
	} 
    auto [i, j, k, r] = logger.size();

    ASSERT_EQ(i, 2);
    ASSERT_EQ(j, 2);
    ASSERT_EQ(k, 2);
    ASSERT_EQ(r, 2);
}


