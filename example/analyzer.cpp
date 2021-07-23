#include <iostream>

#include "ioh.hpp"

using namespace ioh;

int main()
{
    // clutchlog::logger().threshold(clutchlog::debug);

    auto p0 = problem::bbob::Discus(1, 2);
    auto p1 = problem::bbob::Sphere(1, 4);
    auto p2 = problem::bbob::AttractiveSector(1, 4);

    logger::Analyzer logger({trigger::on_improvement}, {watch::evaluations});

    for (auto *pb : std::vector<problem::BBOB *>({&p0}))
    {
        std::cout << "call attach\n";
        pb->attach_logger(logger);
        for (auto r = 0; r < 3; ++r)
        {
            for (auto s = 0; s < 3; ++s)
                (*pb)(common::random::uniform(pb->meta_data().n_variables, s));
            pb->reset();
        }
    }
}
