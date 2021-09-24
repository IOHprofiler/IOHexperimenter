#include "../utils.hpp"

#include "ioh/logger/store.hpp"
#include "ioh/suite.hpp"

TEST_F(BaseTest, store_data_consistency)
{
    using namespace ioh;

    auto sample_size = 10;
    auto nb_runs = 2;

    suite::BBOB suite({1, 2}, {1, 2}, {3, 10}); // problems, instances, dimensions
    logger::Store logger({trigger::always},{watch::transformed_y});

    suite.attach_logger(logger);

    for (const auto &pb : suite) {
        for (auto r = 0; r < nb_runs; r++) {
            for (auto s = 0; s < sample_size; ++s) {
                (*pb)(common::random::pbo::uniform(pb->meta_data().n_variables, 0));
            }
            pb->reset();
        }
    }

    EXPECT_EQ(logger.data().at(suite.name()).size(), 2); // 2 problems
    EXPECT_EQ(logger.data().at(suite.name()).at(/*pb*/1).size(), 2); // 2 dimensions
    EXPECT_EQ(logger.data().at(suite.name()).at(/*pb*/1).at(/*dim*/3).size(), 2); // 2 instances
    EXPECT_EQ(logger.data().at(suite.name()).at(/*pb*/1).at(/*dim*/10).at(/*ins*/2).size(), nb_runs);
    EXPECT_EQ(logger.data().at(suite.name()).at(/*pb*/1).at(/*dim*/10).at(/*ins*/2).at(/*run*/0).size(), sample_size);
    EXPECT_EQ(logger.data().at(suite.name()).at(/*pb*/1).at(/*dim*/10).at(/*ins*/2).at(/*run*/0).at(/*eval*/0).size(), 1); // 1 property watched
    EXPECT_GT(logger.data().at(suite.name()).at(/*pb*/1).at(/*dim*/10).at(/*ins*/2).at(/*run*/0).at(/*eval*/0).at(watch::transformed_y.name()), 0);

}

TEST_F(BaseTest, issue94)
{
    using namespace ioh;

    auto sample_size = 2;
    auto nb_runs = 2;

    suite::BBOB suite({1,2}, {1}, {3}); // problems, instances, dimensions


    double* nullp = nullptr;
    EXPECT_DEBUG_DEATH( watch::Pointer nope("Nope", nullp), "");

    double* p_transient_att = nullptr;
    watch::PointerReference attpr("Att_PtrRef", p_transient_att);
    IOH_DBG(xdebug, "@ " << attpr.ref_ptr_var() << " -> " << p_transient_att);
    
    trigger::Always always;
    logger::Store logger({always},{attpr});
    suite.attach_logger(logger);

    double my_attribute = 100;
    for (const auto &pb : suite) {
        IOH_DBG(progress, "Problem");
        for (auto r = 0; r < nb_runs; r++) {
            IOH_DBG(progress, "Run " << r);
            for (auto s = 0; s < sample_size; ++s) {
                IOH_DBG(progress, "Sample " << s);

                if(s >= sample_size/2) {
                    IOH_DBG(progress, "available attribute");
                    p_transient_att = &my_attribute;
                    IOH_DBG(xdebug, "@ " << p_transient_att << " == " << *p_transient_att << " == " << my_attribute);
                } else {
                    IOH_DBG(progress, "unavailable attribute");
                    p_transient_att = nullptr;
                    IOH_DBG(xdebug, "@ " << p_transient_att << " == nullptr");
                }
                // ref_ptr_var is a member only available under NDEBUG
                IOH_DBG(xdebug, "@ " << attpr.ref_ptr_var() << " -> " << p_transient_att << " -> " << &my_attribute );
                (*pb)(common::random::pbo::uniform(pb->meta_data().n_variables, 0));
                my_attribute++;
            }
            pb->reset();
        }
    }

    logger::Store::Cursor first_eval(suite.name(), /*pb*/1, /*dim*/3, /*ins*/1, /*run*/0, /*eval*/0);

    ASSERT_EQ(logger.at(first_eval, attpr), std::nullopt);
    
    logger::Store::Cursor last_eval(suite.name(), /*pb*/2, /*dim*/3, /*ins*/1, /*run*/1, /*eval*/sample_size-1);
    ASSERT_EQ(logger.at(last_eval, attpr).value(), my_attribute-1);
}

TEST_F(BaseTest, store_properties)
{
    using namespace ioh;

    auto sample_size = 4;
    auto nb_runs = 2;

    suite::BBOB suite({1, 2}, {1, 2}, {3, 10}); // problems, instances, dimensions

    double my_attribute = 0;
    watch::Reference attr("Att_reference", my_attribute);
    watch::Pointer   attp("Att_pointer"  ,&my_attribute);

    double* nullp = nullptr;
    EXPECT_DEBUG_DEATH( watch::Pointer nope("Nope", nullp), "");

    double* p_transient_att = nullptr;
    watch::PointerReference attpr("Att_PtrRef", p_transient_att);
    
    trigger::Always always;
    watch::Evaluations evaluations;
    watch::RawYBest raw_y_best;
    watch::TransformedY transformed_y;
    watch::TransformedYBest transformed_y_best;
    logger::Store logger({always},{evaluations, raw_y_best, transformed_y, transformed_y_best, attr, attp, attpr});
    suite.attach_logger(logger);

    for (const auto &pb : suite) {
        for (auto r = 0; r < nb_runs; r++) {
            for (auto s = 0; s < sample_size; ++s) {
                if(s > sample_size/2) {
                    p_transient_att = &my_attribute;
                } else {
                    p_transient_att = nullptr;
                }
                (*pb)(common::random::pbo::uniform(pb->meta_data().n_variables, 0));
                my_attribute++;
            }
            pb->reset();
        }
    }

    logger::Store::Cursor first_eval(suite.name(), /*pb*/1, /*dim*/3, /*ins*/1, /*run*/0, /*eval*/0);
    auto evals = logger.at(first_eval, evaluations);
    ASSERT_NE(evals, std::nullopt);
    EXPECT_GT(evals, 0);

    ASSERT_NE(logger.at(first_eval, attp ), std::nullopt);
    ASSERT_NE(logger.at(first_eval, attr ), std::nullopt);
    ASSERT_EQ(logger.at(first_eval, attpr), std::nullopt);
    
    ASSERT_EQ(logger.at(first_eval, attp ).value(), 0);
    ASSERT_EQ(logger.at(first_eval, attr ).value(), 0);

    logger::Store::Cursor last_eval(suite.name(), /*pb*/2, /*dim*/10, /*ins*/2, /*run*/nb_runs-1, /*eval*/sample_size-1);
    ASSERT_EQ(logger.at(last_eval, attp ).value(), my_attribute-1);
    ASSERT_EQ(logger.at(last_eval, attr ).value(), my_attribute-1);
    ASSERT_EQ(logger.at(last_eval, attpr).value(), my_attribute-1);
}
