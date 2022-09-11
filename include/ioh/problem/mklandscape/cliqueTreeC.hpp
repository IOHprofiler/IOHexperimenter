#pragma once

#include "ioh/problem/problem.hpp"
#include "ioh/problem/utils.hpp"
#include <set>

#ifdef MK_COMPILED

#include "problem_generator.h"

namespace ioh::problem
{
    class CliqueTreeC : public Integer
    {
    protected:
        const double FITNESS_EPSILON = 0.0000000001;

        CliqueTree* cliqueTree;

        InputParameters inputParameters;

        double globOptScore;
        std::set<std::vector<int>> globOptimaSet;

        std::vector<int> transform_variables(std::vector<int> x) override
        {
            return x;
        }

        double transform_objectives(const double y) override
        {
            using namespace transformation::objective;
            if (meta_data_.instance > 1)
                return uniform(shift, uniform(scale, y, meta_data_.instance, -0.2, 4.8), meta_data_.instance, 1e3, 2e3);
            return y;
        }

        double evaluate(const std::vector<int> &x) override
        {
            double fitness = evaluate_solution(this->cliqueTree, x.data(), x.size());
            this->globalOptimumFound = this->isGlobalOptimum(x, fitness);
            return fitness;
        }

    public:
        bool globalOptimumFound;

        CliqueTreeC(const int problem_id, const int instance, const int n_variables, const std::string &name,
               const int m, const int k, const int o, const int b, const uint64_t clique_seed = 2398) :
            Integer(MetaData(problem_id, instance, name, n_variables, common::OptimizationType::MAX)),
            inputParameters()
        {
            inputParameters.m = m;
            inputParameters.k = k;
            inputParameters.o = o;
            inputParameters.b = b;

            CodomainFunction codomainFunction = CodomainFunction();
            codomainFunction.tag = CodomainFunction::Tag::DeceptiveTrap; 

            ChaChaRng* chaChaRng;
            chaChaRng = get_rng_c(&clique_seed);

            uintptr_t length = (inputParameters.m - 1) * (inputParameters.k - inputParameters.o) + inputParameters.k;

            this->cliqueTree = construct_clique_tree(inputParameters, codomainFunction, chaChaRng);

            uintptr_t num_glob_opt = get_number_of_global_optima(this->cliqueTree);
            this->globOptScore = get_score_of_global_optima(this->cliqueTree);

            this->globOptimaSet = this->getGlobalOptima(num_glob_opt, length);
            this->globalOptimumFound = false;
        }

        ~CliqueTreeC(){
            free_clique_tree(this->cliqueTree);
        }

        std::set<std::vector<int>> getGlobalOptima(uintptr_t numGlobOpt, uintptr_t length) {

            int** globOptimaSolutions = new int*[numGlobOpt];
            for(int i = 0; i < numGlobOpt; i++) {
                globOptimaSolutions[i] = new int[length];
            }

            write_global_optima_to_pointer(this->cliqueTree, globOptimaSolutions);
            std::set<std::vector<int>> globOptimaSet;

            for(int i = 0; i < numGlobOpt; i++) {
                std::vector<int> glob_opt(globOptimaSolutions[i], globOptimaSolutions[i] + length);
                auto result = globOptimaSet.insert(glob_opt);
                if (!result.second) {
                    throw std::logic_error("Global optima are not unique...");
                }
            }

            for(int i = 0; i < numGlobOpt; i++) {
                delete [] globOptimaSolutions[i];
            }
            delete [] globOptimaSolutions;

            return globOptimaSet;
        }

        bool isGlobalOptimum(const std::vector<int> &x, double score) {
            return (score == this->globOptScore || (
                std::abs(score - this->globOptScore) < FITNESS_EPSILON && this->globOptimaSet.find(x) != this->globOptimaSet.end() 
            ));
        }

    };
}

#endif
