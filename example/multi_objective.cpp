#include "ioh.hpp"

int main()
{
    auto p = std::make_shared<ioh::problem::OneMinMax>(1, 10);
    auto logger = ioh::logger::analyzer::mo::Analyzer({ioh::trigger::always}, {},"./data", "ioh", "test", "binom_bitflip");
    p->attach_logger(logger);
    int i = 0;
    int r = 0;
    while (r < 10)
    {
        while (i < 100)
        {
            std::vector<int> a(10);
            for (auto j = 0; j != a.size(); ++j)
            {
                a[j] = (int)rand() % 2;
            }
            std::vector<double> f = (*p)(a);
            std::cout << f[0] << "," << f[1];
            std::cout << std::endl;
            ++i;
        }
        p->reset();
        ++r;
    }
    return 0;
}