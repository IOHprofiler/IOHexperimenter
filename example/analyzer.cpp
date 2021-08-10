#include <iostream>

#include "ioh.hpp"

using namespace ioh;

template <typename T>
std::vector<T> print(const std::vector<T>& x)
{
    for (const auto &e : x)
        std::cout << e << " ";
    std::cout << std::endl;
    return x;
}


int main()
{
    // clutchlog::logger().threshold(clutchlog::debug);

    auto p0 = problem::bbob::Discus(1, 2);
    auto p1 = problem::bbob::Sphere(1, 4);
    auto p2 = problem::bbob::AttractiveSector(1, 4);

    logger::Analyzer logger({trigger::on_improvement}, {watch::evaluations});

    for (auto *pb : std::vector<problem::BBOB *>({&p0}))
    {
        pb->attach_logger(logger);
        for (auto r = 0; r < 3; ++r)
        {
            for (auto s = 1; s < 4; ++s)
                (*pb)(print(common::random::uniform(pb->meta_data().n_variables, s)));
            pb->reset();
        }
    }
}
