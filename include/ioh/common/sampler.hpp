#pragma once

#include "ioh/common/random.hpp"
#include "ioh/common/sobol.hpp"

namespace ioh::common::random::sampler
{
    
    template <typename T>
    struct Sampler
    {
        using seed_type = long long int;
        static inline seed_type default_seed = 1993;

        Sampler(const size_t n, const seed_type seed, const T lb, const T ub) : n(n), seed(seed), lb(lb), ub(ub) {}

        virtual std::vector<T> next() = 0;

    protected:
        size_t n;
        seed_type seed;
        T lb;
        T ub;
    };


    template <typename T>
    struct Uniform : Sampler<T>
    {
        using seed_type = long long int;
        static inline seed_type default_seed = 1993;

        Uniform(const size_t n, const seed_type seed = default_seed, const T lb = 0, const T ub = 1) :
            Sampler<T>(n, seed, lb, ub), gen_(static_cast<std::mt19937::result_type>(seed)), delta_g_(std::mt19937::max() -
                std::mt19937::min()), delta_(ub - lb)
        {
        }

        virtual std::vector<T> next() override
        {
            std::vector<T> res(this->n);
            for (size_t i = 0; i < this->n; i++)
                res[i] = static_cast<T>((delta_ * (static_cast<double>(gen_()) / static_cast<double>(delta_g_))) + this->lb);
            return res;
        }

    private:
        std::mt19937 gen_;
        size_t delta_g_;
        T delta_;
    };


    struct Sobol : Sampler<double>
    {
        Sobol(const size_t n, const seed_type seed = Sampler::default_seed, const double lb = 0, const double ub = 1) :
            Sampler(n, std::max(seed_type{2}, seed), lb, ub)
        {
        }

        virtual std::vector<double> next() override { 
            std::vector<double> data(n);
            i8_sobol(static_cast<int>(n), &seed, data.data(), lb, ub);
            return data;
        }
    };

    struct Halton : Sampler<double>
    {
        Halton(const size_t n, const seed_type seed = Sampler::default_seed, const double lb = 0, const double ub = 1) :
            Sampler(n, seed, lb, ub), delta(ub - lb)
        {
            primes = sieve(std::max(6, static_cast<int>(n)));
            while (primes.size() < n)
                primes = sieve(static_cast<int>(primes.size() * primes.size()));
            primes.resize(n);
        }
        
        virtual std::vector<double> next() override
        {
            std::vector<double> data(n);

            for (size_t j = 0; j < n; j++)
                data[j] = (delta * next_seq(static_cast<int>(seed), primes[j])) + lb;

            seed++; //TODO: this can overflow
            return data;
        }

        private:
            double delta;
            std::vector<int> primes;

            static inline std::pair<int, int> divmod(const double top, const double bottom) {
                const auto div = static_cast<int>(top / bottom);
                return {div, static_cast<int>(top - div * bottom)};            
            }

            static inline std::vector<int> sieve(const int n_samples) {

                std::vector<int> mask(n_samples + 1, 1);
                
                for (int p = 2; p * p <= n_samples; p++)
                {
                    if (mask[p])
                        for (int i = p * p; i <= n_samples; i += p)
                            mask[i] = 0;
                }
                
                std::vector<int> primes;
                for (int p = 2; p <= n_samples; p++)
                    if (mask[p])
                        primes.push_back(p);
                        
                return primes;
            }

        static double next_seq(int index, const int base)
        {
                double y = 1., x = 0.;
                while (index > 0)
                {
                    const auto dm = divmod(index, base);
                    index = dm.first;
                    y *= static_cast<double>(base);
                    x += static_cast<double>(dm.second) / y;
                }
                return x;
            }
        };      
}