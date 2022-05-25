#include "../utils.hpp"

#include "ioh/logger/flatfile.hpp"
#include "ioh/problem/bbob/sphere.hpp"
#include "ioh/problem/bbob/attractive_sector.hpp"

using namespace ioh;

TEST_F(BaseTest, logger_flatfile)
{
    auto p0 = problem::bbob::Sphere(1, 2);
    auto p1 = problem::bbob::Sphere(1, 4);
    auto p2 = problem::bbob::AttractiveSector(1, 4);

    trigger::Always always;
    watch::TransformedY transformed_y;
    {
        auto logger = logger::FlatFile( {always}, {transformed_y}, "IOH.dat", "." );

        const int runs = 3;
        const int samples = 3;
        
        for(auto pb : std::array<problem::BBOB*,3>({&p0,&p1,&p2})) {
            pb->attach_logger(logger);
            for(auto r = 0; r < runs; ++r) {
                for(auto s = 0; s < samples; ++s) {
                    (*pb)(common::random::pbo::uniform(pb->meta_data().n_variables, s));
                }
                pb->reset();
            }
        }
    }
    EXPECT_TRUE(fs::exists("./IOH.dat"));
    EXPECT_GT(ioh::common::file::as_string("./IOH.dat").size(),0);
    fs::remove("./IOH.dat");
    EXPECT_TRUE(!fs::exists("./IOH.dat"));
}
