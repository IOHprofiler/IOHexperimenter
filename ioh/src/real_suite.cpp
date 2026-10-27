#include "common.hpp"

namespace ioh_python::real_suite
{
    template <typename SuiteType>
    void define_base_class(nb::module_ &m, const std::string &name)
    {
        nb::class_<SuiteType>(m, name.c_str(), fmt::format("{} suite", name).c_str())
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
            // new arrays, which take over the vectors that the accessors return (automatic: the default,
            // reference_internal, fails for an array with an owner, and move would copy it again)
            .def_prop_ro(
                "problem_ids", [](const SuiteType &c) { return make_array(c.problem_ids()); }, nb::rv_policy::automatic,
                R"pbdoc(
                The ids of all problems contained in the current suite, as a new numpy array (int32).
            )pbdoc")
            .def_prop_ro(
                "dimensions", [](const SuiteType &c) { return make_array(c.dimensions()); }, nb::rv_policy::automatic,
                R"pbdoc(
                The dimensions of all problems contained in the current suite, as a new numpy array (int32).
            )pbdoc")
            .def_prop_ro(
                "instances", [](const SuiteType &c) { return make_array(c.instances()); }, nb::rv_policy::automatic,
                R"pbdoc(
                The instance ids of all problems contained in the current suite, as a new numpy array (int32).
            )pbdoc")
            .def_prop_ro("name", &SuiteType::name,
                         R"pbdoc(
                The name of the suite.
            )pbdoc")
            .def("__len__", &SuiteType::size)
            // the iterator holds a pointer to the suite, which it keeps alive; its type is registered in the scope of
            // the base class (pybind11 registered it nowhere)
            .def(
                "__iter__",
                [](SuiteType &s) {
                    return nb::make_iterator<nb::rv_policy::reference_internal>(nb::type<SuiteType>(), "iterator",
                                                                                s.begin(), s.end());
                },
                nb::keep_alive<0, 1>());
    }

    //! A suite class S, derived from the base class P. Its constructor only takes lists (any sequence, numpy arrays
    //! included): it is not called often enough to need an array overload.
    template <typename S, typename P>
    void define_suite(nb::module_ &m, const std::string &name, const int default_dim = 5)
    {
        nb::class_<S, P>(m, name.c_str(), suite_docstring(name).c_str())
            .def(nb::init<std::vector<int>, std::vector<int>, std::vector<int>>(), nb::arg("problem_ids"),
                 nb::arg("instances") = std::vector<int>{1}, nb::arg("dimensions") = std::vector<int>{default_dim});
    }
} // namespace ioh_python::real_suite

void define_real_suite(nb::module_ &m)
{
    using namespace ioh_python::real_suite;
    using namespace ioh::suite;
    using RealBase = Suite<ioh::problem::RealSingleObjective>;
    define_base_class<RealBase>(m, "RealBase");
    define_suite<Real, RealBase>(m, "Real");
    define_suite<BBOB, RealBase>(m, "BBOB");
    define_suite<SBOX, RealBase>(m, "SBOX");
    define_suite<RealStarDiscrepancy, RealBase>(m, "RealStarDiscrepancy");
}
