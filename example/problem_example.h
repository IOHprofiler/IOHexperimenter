#pragma once

#include "ioh.hpp" 

/// Create a OneMax problem. We can use either factory construction, or direct object construction.
inline std::shared_ptr<ioh::problem::Integer> create_one_max_problem(const int instance, const int dimension,
                                                                     const bool using_factory = true)
{
    if (using_factory)
    {
        const auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::Integer>::instance();
        return problem_factory.create("OneMax", instance, dimension);
    }
    return std::make_shared<ioh::problem::pbo::OneMax>(instance, dimension);
}


/// An example of using a problem
inline void simple_problem_example()
{
    std::vector<int> instance{1, 2, 3, 51, 52};
    std::vector<int> dimension{50, 100};

    std::cout << "==========\nAn example of testing OneMax\n==========" << std::endl;

    for (const auto d : dimension)
    {
        for (const auto i : instance)
        {
            const auto om = create_one_max_problem(i, d, d == 50);
            for (auto ri = 0; ri < 1; ri++)
            {
                /// To output information of the current problem.
                std::cout << om->meta_data() << std::endl;
                std::cout << "bounds of variables :  " << om->bounds() << std::endl;

                /// Random search on the problem with the given budget 100.
                for (auto budget = 100; budget > 0; budget--)
                    /// Generate a random vector of integers of size n_variables and evaluate problem
                    (*om)(ioh::common::random::integers(om->meta_data().n_variables, 0, 1));

                std::cout << "result: " << om->state().current_best.y << std::endl;
            }
        }
    }
}

////////////////// Extending Problems //////////////////

/// A new problem need to inherit from the problem class of the correct type: e.g. RealProblem for
/// double type problems. Note that this class uses a CRTP pattern. 
class AnotherRealProblem final : public ioh::problem::RealProblem<AnotherRealProblem>
{
protected:
    // The evaluate method is required, in this case the value of x0 is return as objective value
    double evaluate(const std::vector<double> &x) override
    {
        return x.at(0);
    }

public:
    /// This constructor is required(i.e. (int, int), even if the newly create problem does not have a way to handle different
    /// instances/dimensions.
    AnotherRealProblem(const int instance, const int n_variables) :
        RealProblem(ioh::problem::MetaData(1, instance, "AnotherRealProblem", n_variables,
                                           ioh::common::OptimizationType::MIN))
    {
    }
};

//! Compute the sum of a vector of double
inline double sum_vector(const std::vector<double> &x) { 
    return std::accumulate(x.begin(), x.end(), 0.0); 
}

//! Method to compute the optimum for a given problem instance (iid)
ioh::problem::Solution<double> calculate_objective(const int iid, const int dim)
{
    return {std::vector<double>(dim, iid), static_cast<double>(iid * dim)};
}

//! Example of a variables transformation function
std::vector<double> tx(std::vector<double> x, const int iid){
    x.at(0) = static_cast<double>(iid);
    return x;
}

//! Example of a objective transformation function
double ty(const double y, const int iid){
    return y * iid;
}

inline void extending_problems_example()
{
    /// Note that problems which extend RealProblem are automatically avaible in the corresponsing Factory:
    auto &factory = ioh::problem::ProblemRegistry<ioh::problem::Real>::instance();
    
    const int instance = 10;
    const int dimension = 10;

    auto another_problem = factory.create("AnotherRealProblem", 1, 10);
    std::cout << another_problem->meta_data() << std::endl;

    // We provide convenience wrapper functions for wrapping 'standalone' functions, which 
    // register a function in the factory. Only the first two arguments are required.
    ioh::problem::wrap_function<double>(
        sum_vector, "Sum", ioh::common::OptimizationType::MIN, 
        -1, // the lower bound for the constraint
        10, // the upper bound for the constraint
        tx, // variables transformation function, transforms x prior to calling f(x)
        ty, // objective transformation function, transforms the output of f(x), i.e. t(f(x))
        calculate_objective // compute the value for the objective for this function
    );
    
    // We can now create new Sum objects from the factory
    auto problem = factory.create("Sum", instance, dimension);
    std::cout << (*problem).meta_data() << std::endl;
}


///////////////////// Extending Problems //////////////////
inline void problem_example()
{
    simple_problem_example();
    extending_problems_example();
}
