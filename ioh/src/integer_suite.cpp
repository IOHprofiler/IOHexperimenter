#include "pch.hpp"

std::string make_docstring(const std::string &name);

template <typename SuiteType>
static void define_base_class(nb::module_ &m, const std::string &name)
{
    nb::class_<SuiteType>(m, name.c_str(),
            fmt::format("{} suite", name).c_str())
        .def("reset", &SuiteType::reset,
             R"pbdoc(
                Reset the state variables of the current problem.
            )pbdoc")
        .def("attach_logger", &SuiteType::attach_logger,
             R"pbdoc(
                Attach a logger to the problem to track the performance and internal variables.
            )pbdoc")
        .def("detach_logger", &SuiteType::detach_logger,
             R"pbdoc(
                Remove the specified logger from the problem
            )pbdoc")
        .def_prop_ro(
            "problem_ids",
            [](const SuiteType &c) {
                const auto problem_ids = c.problem_ids();
                return make_array(problem_ids);
            },
            R"pbdoc(
                The list of all problems ids contained in the current suite.
            )pbdoc")
        .def_prop_ro(
            "dimensions",
            [](const SuiteType &c) {
                const auto dimensions = c.dimensions();
                return make_array(dimensions);
            },
            R"pbdoc(
                The list of all problems ids contained in the current suite.
            )pbdoc")
        .def_prop_ro(
            "instances",
            [](const SuiteType &c) {
                const auto instances = c.instances();
                return make_array(instances);
            },
            R"pbdoc(
                The list of all instance ids contained in the current suite.
            )pbdoc")
        .def_prop_ro("name", &SuiteType::name,
                               R"pbdoc(
                The name of the suite.
            )pbdoc")
        .def("__len__", &SuiteType::size)
        .def(
            "__iter__", [](SuiteType &s) { 
                return nb::make_iterator(nb::type<typename SuiteType::Problem>(), "problem_iterator", s.begin(), s.end()); 
            }, nb::keep_alive<0, 1>());
}


template <typename S, typename P>
static void define_suite(nb::module_ &m, const std::string &name, const int default_dim = 5)
{
    nb::class_<S, P>(m, name.c_str(), make_docstring(name).c_str())
        .def(nb::init<std::vector<int>, std::vector<int>, std::vector<int>>(), nb::arg("problem_ids"),
             nb::arg("instances") = std::vector<int>{1}, nb::arg("dimensions") = std::vector<int>{default_dim});
}

void define_integer_suite(nb::module_ &m)
{
    using IntegerBase = suite::Suite<ioh::problem::IntegerSingleObjective>;
    define_base_class<IntegerBase>(m, "IntegerBase");
    define_suite<suite::Integer, IntegerBase>(m, "Integer");
    define_suite<suite::PBO, IntegerBase>(m, "PBO", 16);
    define_suite<suite::Submodular, IntegerBase>(m, "Submodular", 1);
    define_suite<suite::IntegerStarDiscrepancy, IntegerBase>(m, "IntegerStarDiscrepancy");
}