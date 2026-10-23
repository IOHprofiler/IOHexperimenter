:tada: First off, thanks for using and contributing to __IOHexperimenter__!! :tada:

### Table of Contents

Note that this page is __under construction__. More detailed guidelines are being added.

[What should I Know before Getting Started?](#what-should-i-know-before-getting-started)

[How can I Contribute?](#how-can-i-contribute)

* [Reporting bugs](#reporting-bugs)
* [Adding new problems and suites](#adding-new-problems-and-suites)  <!-- * [Creating new logger](#creating-new-loggers) -->
* [Suggesting enhancements](#suggesting-enhancements)
* [Pull request](#pull-request)

[Styleguides](#styleguides)

* [Git commit messages](#git-commit-messages)
* [C++ styleguide](#cpp-styleguide)
  <!-- * [Python styleguide](#python-styleguide)
  * [Additional Notes](#additional-notes) -->

## What should I Know before Getting Started?

<a id="#what-should-i-know-before-getting-started"></a>

## How can I Contribute?

<a id="#how-can-i-contribute"></a>

### Reporting bugs

<a id="reporting-bugs"></a>

This section guides you through submitting a bug for __IOHexperimenter__. Following the guides will help the maintainers and the community understand your report.

The bug reports are tackled as [Github issues](https://github.com/IOHprofiler/IOHexperimenter/issues). After determining which repository the bug is related to, create an issue on the repository and report following the template as below.

<a id="bug-template"></a>

```
Description: 

Versions:

Steps to Reproduce:
1.
2.
3.

Expected Behaviour:

Actual Behavior:

Additional Information:
```

Explain the issue with additional information that can help the maintainers. Please consider the following contents.

* Use a clear and descriptive title for the issue.
* Describe the steps of reproducing the issue in detail. Attachments of your implementation and screenshots are appreciated.
* Explain the behavior (or the result) that you expect.
* Are there additional packages required for reproducing the bug?
* What is your working environment, e.g., operating system, compiler version, etc.?

### Adding new problems and suites

<a id="#adding-new-problems-and-suites"></a>

This section guides you through creating new benchmarking problems and suites for __IOHexperimenter__.  Instead of testing additional problems using the [proxy](https://github.com/IOHprofiler/IOHexperimenter/blob/8ed56649d9b6f6261de74f745468133a414c57ea/example/add_new_problem.cpp#L29) provided in IOHexperimenter, the goal is to integrate new problems as internal packages.

Follow the steps of contributing new problems:

* [Implementing new problem classes](#implementing-new-problem-classes)
* [Using the suite class (optional)](#using-the-suite-class)
* [Pull request](#pull-request)

#### Implementing new problem classes

<a id="implementing-new-problem-classes"></a>

The header files shall locate at the [problem folder](https://github.com/IOHprofiler/IOHexperimenter/tree/master/include/ioh/problem).

We require to create the new problem by subclassing the abstract problem class in IOHexperimenter, taking benefits of implementing more details, e.g., aforementioned transformations.
This can be done by inheriting the corresponding problem class, which is

* `ioh::problem::IntegerSingleObjective` for pseudo-Boolean problems, and
* `ioh::problem::RealSingleObjective` for continuous problems.

In the following example, we show how to do this for pseudo-Boolean problems.

```C++
class NewBooleanProblem final : public ioh::problem::IntegerSingleObjective
{
protected:
    // [mandatory] The evaluate method is mandatory to implement
    double evaluate(const std::vector<int> &x) override
    {
        // the function body goes here
        return 0.0;
    }

    // [optional] If one wish to implement transformations on search variables
    std::vector<int> transform_variables(std::vector<int> x) override
    {
        return x;
    }

    // [optional] If one wish to implement transformations on objective values
    double transform_objectives(const double y) override
    { 
        return y;
    } 

public:
    /// [mandatory] The id and name in the registry, also used by the constructor: a free id of the factories the
    /// problem goes into
    static constexpr int registry_id = 26;
    static constexpr const char *registry_name = "NewBooleanProblem";

    /// [mandatory] This constructor always take `instance` as input even
    /// if it is ineffective by default. `instance` would be effective if and only if
    /// at least one of `transform_objectives` and `transform_objectives` is implemented
    NewBooleanProblem(const int instance, const int n_variables) :
        ioh::problem::IntegerSingleObjective(
          ioh::problem::MetaData(
            registry_id,           // the problem id
            instance,              // the instance id
            registry_name,         // the problem name
            n_variables           // search dimensionality
            ) 
          )
    {
    }
};
```

A class whose data defines several problems (like the CEC2013 and graph problems) lists them in a static `registry_entries()` instead, one `ioh::problem::RegistryEntry` (id, name, and a function creating the problem for an instance and a dimension) per problem.

The built-in problems are registered by the compiled library, in [`src/registry`](src/registry): add the class to a family list in [`src/registry/families.hpp`](src/registry/families.hpp) (or start a new one), and call that list from the `add_builtins` functions of the factories that should contain the problem, e.g. the family's own factory and [`src/registry/single_objective.cpp`](src/registry/single_objective.cpp) for the factory of all integer problems. A new factory type needs its own `add_builtins` declaration in [`include/ioh/common/factory.hpp`](include/ioh/common/factory.hpp). Ids must be new and stay fixed: the registry test (`tests/cpp/problem/test_registry.cpp`) and the golden tests (`tests/python/test_golden.py`) compare the registries with `tests/data/golden`, so extend those fixtures in the same pull request (`python tests/python/golden.py --write`, reviewed by the maintainers).

To try a problem without adding it to the library, register it from your own code instead: `ioh::problem::register_problem<NewBooleanProblem, ioh::problem::IntegerSingleObjective>();`. Please check [the example](example/problem_example.hpp) of adding continuous problems in this manner.

#### Using the suite class (optional)

<a id="using-the-suite-class"></a>

A suite contains a set of problems that share common properties. Therefore, we suggest creating a base class for the new problems, following the template as below:

```cpp
namespace ioh::problem
{
   //! new base class of continuous optimization problems, of which objective is minization.
    class NewBase : public RealSingleObjective
    {
    public:
        /**
         * @brief Construct a new NewBase object
         * 
         * @param problem_id The id of the problem
         * @param instance The instance of the problem
         * @param n_variables the dimension of the problem
         * @param name the name of the problem
         */
        NewBase(const int problem_id, const int instance, const int n_variables, const std::string &name) :
            RealSingleObjective(MetaData(problem_id, instance, name, n_variables, common::OptimizationType::MIN))
        {
        }
    };
}
```

Inherit from the base class when defining the new problems.

```cpp
namespace ioh::problem
{
     //! NewProblemere function problem id 42
    class NewProblem final: public NewBase
    {
    public:
        //! The id and name in the registry, also used by the constructor
        static constexpr int registry_id = 42;
        static constexpr const char *registry_name = "NewProblem";

    protected:
        //! Evaluation method
        double evaluate(const std::vector<double>&) override
        {
            auto result = 0.0;
            return result;
        }
        
    public:
        /**
         * @brief Construct a new NewProblem object
         * 
         * @param instance instance id
         * @param n_variables the dimension of the problem
         */
        NewProblem(const int instance, const int n_variables) :
            NewBase(registry_id, instance, n_variables, registry_name)
        {
        }
    };
}
```

Register the new problems in the factory of `NewBase` (and of `RealSingleObjective`) as described above. They can then be created using factory construction:

```cpp
auto &problem_factory = ioh::problem::ProblemRegistry<ioh::problem::NewBase>::instance();
for (auto xi: problem_factory.names())
    std::cout << xi << "\n";
auto problem = problem_factory.create(42, 1, 10);
```

Or, we can create a suite class, which only contains the problems of the factory of the same parent type (`NewBase`):

```cpp
namespace ioh::suite {

    struct NewProblems final : Suite<problem::RealSingleObjective>
    {
        //! The id and name in the suite registry
        static constexpr int registry_id = 5;
        static constexpr const char *registry_name = "NewProblems";

        /**
         * @brief Construct a new NewProblems object
         * 
         * @param problem_ids List of problem ids
         * @param instances List of problem instances (defaults to first instance)
         * @param dimensions List of problem dimensions (defaults to 5D)
         */
        NewProblems(const std::vector<int> &problem_ids = {}, const std::vector<int> &instances = {1},
                const std::vector<int> &dimensions = {5}) :
            Suite(problem_ids, instances, dimensions, registry_name, 100, 100,
                  problem::ProblemFactoryType<problem::NewBase>::instance())
        {
        }
    };
}
```

Built-in suites are registered in [`src/registry/suites.cpp`](src/registry/suites.cpp) (add the class to the list of `registry::add_suites`); from your own code, `ioh::suite::register_suite<ioh::suite::NewProblems>();` does the same. The new suite (`NewProblems`) is then in the list of available real-valued suites:

```cpp
auto& suitef = ioh::suite::SuiteRegistry<ioh::problem::RealSingleObjective>::instance();
for (auto xi: suitef.names())
    std::cout << xi << "\n";
```

Which we can use to easily loop over the problems:

```cpp
auto suite = suitef.create("NewProblems", {42}, {1}, {2});
for (auto problem: *suite)
    std::cout << *problem << "\n";
```

<!-- ### Creating new logger
<a id="creating-new-loggers"></a>

This section guides you through creating new loggers for **IOHexperimenter**.  -->

#### Make it visible

After implementing the new problem classes, you may make a pull request. Check the [list](#pull-request) of making a pull request, and follow the template as below.

<a id="problem-template"></a>

```md
Problem Name(s):

Added file(s):

Definition(s) of the problem(s):

Description of the implementation:

Additional information (e.g., reference of the problems, additional required packages.)
```

### Suggesting enhancements

<a id="suggesting-enhancements"></a>

This section guides you through suggesting enhancements for __IOHexperimenter__, which includes providing additional features and improving existing functionalities. Before suggesting the enhancements, check the following list.

* Double check if the enhancements have been implemented by __IOHexperimenter__. Make sure that you are using the latest version.
* Check if the enhancements have been suggested in other issues. If so, please comment on that issue instead of creating a new one.

The suggestions are tackled as Github issues. After following the upper list and determining which repository you plan to suggest the enhancements for, create an issue using the template below.

<a id="enhancement-template"></a>

```md
Suggestions: ("New Features" or "Improving Existing functionalities")

Description in detail:

Name of the function and file (if the suggestion is about improving existing functionalities):

Additional information (e.g., any additional packages for implementing the enhancements):
```

### Pull request

<a id="pull-request"></a>

To make sure that your contributions are considered and understood, please follow the list:

* Follow the [styleguides](#styleguides)
* Follow the [templates](#pr-templates)
* Verify that all [status checks](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/collaborating-on-repositories-with-code-quality-features/about-status-checks) are passing after submitting the pull request.

#### Pull request templates

<a id="pr-templates"></a>

* If you make a pull request for [reporting bugs](#reporting-bugs), follow the [bug template](#bug-template).
* If you make a pull request for [suggesting enchancements](#suggesting-enhancements), follow the [enhacement template](#enhancement-template)
* If you make a pull request for [adding new problems](#adding-new-problems-and-suites), follow the [problem template](#problem-template).
* If you update documentations, follow the template as below.

```
Updated files:

Functionalities involved:

Old description (introduce briefly if the content is too long):

New description (introduce briefly if the content is too long):
```

## Styleguides

<a id="#styleguides"></a>

### Git commit messages

* Use the present tense ("Add feature" not "Added feature")
* Use the imperative mood ("Improve the function..." not "Improves the function...")
* When only changing documentation, include "Update README" in the commit title
<!-- * Start the title with:
  * [New Features:] for adding new functionalities
  * [Bug Fix:] for fixing bugs
  * [Enhancement:] for improving data structure, efficiency, etc.
  * [Polish:] for only polishing code styles (i.e., names, layout. etc.)
  * [Update README:] for updating documentations
  * [Maintain:] indicates that the changes are still under development -->

### C++ styleguide

<a id="cpp-styleguide"></a>

We follow the [Bjarne Stroustrup's C++ Style](https://www.stroustrup.com/bs_faq2.html).

<!-- ### Python styleguide
<a id="python-styleguide"></a>

##Additional Notes
<a id="additional-notes"></a> -->