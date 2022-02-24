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

This section guides you through submitting a bug for **IOHexperimenter**. Following the guides will help the maintainers and the community understand your report.

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

This section guides you through creating new benchmarking problems and suites for **IOHexperimenter**.  Instead of testing additional problems using the [proxy](https://github.com/IOHprofiler/IOHexperimenter/blob/8ed56649d9b6f6261de74f745468133a414c57ea/example/add_new_problem.cpp#L29) provided in IOHexperimenter, the goal is to integrate new problems as internal packages.

Follow the steps of contributing new problems:
* [Implementing new problem classes](#implementing-new-problem-classes)
* [Using the suite class (optional)](#using-the-suite-class)
* [Pull request](#pull-request)

#### Implementing new problem classes
<a id="implementing-new-problem-classes"></a>

The header files shall locate at the [problem folder](https://github.com/IOHprofiler/IOHexperimenter/tree/master/include/ioh/problem).

We require to create the new problem by subclassing the abstract problem class in IOHexperimenter, taking benefits of implementing more details, e.g., aforementioned transformations.
This can be done by inheriting the corresponding problem registration class, which is

* `ioh::problem::IntegerProblem` for pseudo-Boolean problems, and
* `ioh::problem::RealProblem` for continuous problems.

In the following example, we show how to do this for pseudo-Boolean problems.

```C++
class NewBooleanProblem final : public ioh::problem::IntegerProblem<NewBooleanProblem>
{
protected:
    // [mandatory] The evaluate method is mandatory to implement
    std::vector<int> evaluate(const std::vector<int> &x) override
    {
        // the function body goes here
    }

    // [optional] If one wish to implement transformations on objective values
    std::vector<double> transform_objectives(std::vector<double> y) override
    {

    }

    // [optional] If one wish to implement transformations on search variables
    std::vector<double> transform_objectives(std::vector<double> y) override
    {

    }

public:
    /// [mandatory] This constructor always take `instance` as input even
    /// if it is ineffective by default. `instance` would be effective if and only if
    /// at least one of `transform_objectives` and `transform_objectives` is implemented
    NewBooleanProblem(const int instance, const int n_variables) :
        IntegerProblem(
          ioh::problem::MetaData(
            1,                     // problem id, which will be overwritten when registering this class in all pseudo-Boolean problems
            instance,              // the instance id
            "NewBooleanProblem",   // problem name
            n_variables,           // search dimensionality
            1,                     // number of objectives, only support 1 for now
            ioh::common::OptimizationType::Minimization
            )
          )
    {
    }
};
```

Please check [the example](https://github.com/IOHprofiler/IOHexperimenter/blob/759750759331fff1243ef9e121209cde450b9726/example/problem_example.h#L51) of adding continuous problems in this manner.

#### Using the suite class (optional)
<a id="using-the-suite-class"></a>

A suite contains a set of problems that share common properties. Therefore, we suggest creating a base class and a CRTP class for the new problems (see [the example of bbob problems](https://github.com/IOHprofiler/IOHexperimenter/blob/master/include/ioh/problem/bbob/bbob_problem.hpp)), following the template as below:

```cpp
namespace ioh::problem
{
    //! new base class of continuous optimization problems, of which objective is minization.
    class NewBase : public Real
    {
    protected:
       ...
    public:
        /**
         * @brief Construct a new NewBase object
         * 
         * @param problem_id The id of the problem
         * @param instance The instance of the problem
         * @param n_variables the dimension of the problem
         * @param name the name of the problem
         */
        NewBase(const int problem_id, const int instance, const int n_variables, const std::string &name)
            Real(MetaData(problem_id, instance, name, n_variables, common::OptimizationType::Minimization))
        {
            objective_ = ...
        }
    };
    
    /**
     * @brief CRTP class for NewBase problems. Inherit from this class when defining new NewBase problems
     * 
     * @tparam ProblemType The New NewBase problem class
     */
    template <typename ProblemType>
    class NewBaseProblem : public NewBase,
                       AutomaticProblemRegistration<ProblemType, NewBase>,
                       AutomaticProblemRegistration<ProblemType, Real>
    {
    public:
        /**
         * @brief Construct a new NewBaseProblem object
         * 
         * @param problem_id The id of the problem
         * @param instance The instance of the problem
         * @param n_variables the dimension of the problem
         * @param name the name of the problem
         */
        NewBaseProblem(const int problem_id, const int instance, const int n_variables, const std::string &name) :
            NewBase(problem_id, instance, n_variables, name)
        {
        }
    };
}
```

Inherit from the CRTP class when defining the new problems.

```cpp
namespace ioh::problem::NewBase
{
    //! SphNewProblemere function problem id 1
    class NewProblem final: public NewBaseProblem<NewProblem>
    {
    protected:
        //! Evaluation method
        double evaluate(const std::vector<double>& x) override
        {
            auto result = 0.0;
            ...
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
            NewBaseProblem(1, instance, n_variables, "NewProblem")
        {
        }
    };
}
```

After implementing the new problem classes, suites can be created using either factory construction, or direct object construction.
```cpp
td::shared_ptr<ioh::suite::Suite<ioh::problem::Real>> create_suite(const bool using_factory = true)
{
    const std::vector<int> problems;
    const std::vector<int> instances;
    const std::vector<int> dimensions;

    ...

    if (using_factory)
        return ioh::suite::SuiteRegistry<ioh::problem::Real>::instance()
            .create("NewBase", problems, instances, dimensions);
    return std::make_shared<ioh::suite::NewBase>(problems, instances, dimensions);
}
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

This section guides you through suggesting enhancements for **IOHexperimenter**, which includes providing additional features and improving existing functionalities. Before suggesting the enhancements, check the following list.
*  Double check if the enhancements have been implemented by **IOHexperimenter**. Make sure that you are using the latest version.
*  Check if the enhancements have been suggested in other issues. If so, please comment on that issue instead of creating a new one.

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