#include "ioh.hpp"

namespace operators
{
    template <ioh::common::OptimizationType>
    bool is_worse(const double a, const double b);

    template <>
    bool is_worse<ioh::common::OptimizationType::MIN>(const double a, const double b)
    {
        return a >= b;
    }

    template <>
    bool is_worse<ioh::common::OptimizationType::MAX>(const double a, const double b)
    {
        return a <= b;
    }

    template <ioh::common::OptimizationType>
    bool is_better(const double a, const double b);

    template <>
    bool is_better<ioh::common::OptimizationType::MIN>(const double a, const double b)
    {
        return a <= b;
    }

    template <>
    bool is_better<ioh::common::OptimizationType::MAX>(const double a, const double b)
    {
        return a >= b;
    }
} // namespace operators

template <ioh::common::OptimizationType... Args>
struct Solution
{
    static constexpr int S = sizeof...(Args);
    std::array<double, S> y;
    std::vector<int> x;

    bool operator==(const Solution<Args...> &other) const
    {
        for (size_t i = 0; i < S; i++)
            if (y[i] != other.y[i])
                return false;
        return true;
    }

    bool dominated_by(const Solution<Args...> &other) const
    {
        bool dominated = true;
        int i = 0;
        (
            [&] {
                dominated &= operators::is_worse<Args>(y[i], other.y[i]);
                i++;
            }(),
            ...);
        return dominated;
    }

    bool strictly_dominates(const Solution<Args...> &other) const
    {
        bool dominates = true;
        int i = 0;
        (
            [&] {
                dominates &= operators::is_better<Args>(y[i], other.y[i]);
                i++;
            }(),
            ...);
        return dominates and !(*this == other);
    }

    void print() const
    {
        std::cout << "(";
        for (const auto &fi : y)
            std::cout << fi << ",";
        std::cout << ")";
    }
};

template <ioh::common::OptimizationType... Args>
void print(const std::vector<Solution<Args...>> &p)
{
    std::cout << "{";
    for (const auto &z : p)
        z.print();
    std::cout << "}\n";
}

void bitflip(std::vector<int> &x, const double pm)
{
    for (size_t i = 0; i < x.size(); i++)
    {
        const auto ri = ioh::common::random::real();
        if (ri < pm)
        {
            x[i] = abs(x[i] - 1);
        }
    }
}

void bitflip_binom(std::vector<int> &x, const double pm, const int m = 0)
{
    std::binomial_distribution<> d(x.size(), pm);
    const auto n = std::max(m, d(ioh::common::random::gen));
    for (const auto &pos : ioh::common::random::integers(n, 0, x.size() - 1))
        x[pos] = abs(x[pos] - 1);
}


template <ioh::common::OptimizationType... Args>
struct GSEMO
{
    using GSolution = Solution<Args...>;

    int budget;
    bool force_flip;
    int verbose_rate;

    GSEMO(const int b  = 100000, const bool ff = true, const int v = 0): 
        budget(b), force_flip(ff), verbose_rate(v) {}


    std::vector<GSolution> operator()(const std::shared_ptr<ioh::problem::Integer> &problem)
    {
        const double pm = 1. / problem->meta_data().n_variables;
        std::vector<GSolution> p{eval(std::vector<int>(problem->meta_data().n_variables, 0.0), problem)};

        for (int i = 1; i < budget; i++)
        {
            auto ri = ioh::common::random::integer(0, p.size() - 1);
            auto xi = p[ri].x;

            bitflip_binom(xi, pm, (int)force_flip);
            auto si = eval(xi, problem);

            auto any_dominates = false;
            std::vector<GSolution> pnew{si};

            for (const auto &z : p)
            {
                if (z.strictly_dominates(si))
                {
                    any_dominates = true;
                    break;
                }

                if (!z.dominated_by(si))
                    pnew.push_back(z);
            }
            if (!any_dominates)
                p = pnew;


            if (verbose_rate and i % verbose_rate == 0)
                print(p);
        }
        return p;
    }


    GSolution eval(const std::vector<int> &x, const std::shared_ptr<ioh::problem::Integer> &problem)
    {
        GSolution s;
        s.x = x;
        s.y[0] = (*problem)(x);
        for (size_t i = 1; i < GSolution::S; i++)
            s.y[i] = problem->constraints()[i - 1]->violation();

        return s;
    }
};


int main()
{
    using namespace ioh::common;
    using namespace ioh::problem;
    random::seed(10);
    bool force_flip = true;
    int budget = 100000;
    int runs = 1;

    auto &factory = ioh::problem::ProblemRegistry<ioh::problem::submodular::GraphProblem>::instance();
    auto logger = ioh::logger::Analyzer(
        {ioh::trigger::always},
        {ioh::watch::violation},
        "/home/jacob/code/IOHexperimenter/data",
        force_flip ? "GSEMO0": "GSEMO",
        force_flip ? "GSEMO>0": "GSEMO",
        "binom_bitflip"
    );
    
    for (auto &[id, name] : factory.map())
    {
        if (id < 2100 or id >= 2105) continue;

        auto problem = factory.create(id, 1, 1);
        // Multiobjective setup
        problem->constraints()[0]->weight = -1.0;
        problem->constraints()[0]->exponent = .0;
        
        problem->attach_logger(logger);
        // std::cout << problem->meta_data() << std::endl;

        // for (int i =0; i < runs; ++i){
        //     GSEMO<OptimizationType::MAX, OptimizationType::MIN> opt(budget, force_flip);
        //     auto p = opt(problem);
        //     print(p);        
        //     problem->reset();
        // }
    }
}