#include "common.hpp"

#include <fmt/format.h>

namespace ioh_python::empirical_attainment
{
    template <typename T>
    void define_eah_scale(nb::module_ &m, const std::string &name)
    {
        using namespace ioh::logger::eah;

        nb::class_<Scale<T>>(m, name.c_str())
            .def_prop_ro("min", &Scale<T>::min)
            .def_prop_ro("max", &Scale<T>::max)
            .def_prop_ro("size", &Scale<T>::size)
            .def_prop_ro("length", &Scale<T>::length)
            .def("index", &Scale<T>::index)
            .def("bounds", &Scale<T>::bounds)
            .def("__repr__", [name](const Scale<T> &s) {
                return fmt::format("<{} (({}, {}), {})>", name, s.min(), s.max(), s.size());
            });

        nb::class_<LinearScale<T>, Scale<T>>(m, ("Linear" + name).c_str())
            .def(nb::init<T, T, size_t>())
            .def("step", &LinearScale<T>::step);

        nb::class_<Log2Scale<T>, Scale<T>>(m, ("Log2" + name).c_str()).def(nb::init<T, T, size_t>());

        nb::class_<Log10Scale<T>, Scale<T>>(m, ("Log10" + name).c_str()).def(nb::init<T, T, size_t>());
    }

    //! An attainment matrix in Python: a two-dimensional bool array, error targets (rows) by evaluation targets
    using AttainmentArray = nb::ndarray<nb::numpy, bool, nb::ndim<2>>;

    //! EAH.data in Python: dicts by problem id, dimension, instance and run, of attainment matrices
    using RunsDict = nb::typed<nb::dict, size_t, AttainmentArray>;
    using InstancesDict = nb::typed<nb::dict, size_t, RunsDict>;
    using DimensionsDict = nb::typed<nb::dict, size_t, InstancesDict>;
    using AttainmentDict = nb::typed<nb::dict, size_t, DimensionsDict>;

    //! The attainment matrices of an EAH as nested dicts of new arrays (nanobind's casters would make lists of the
    //! std::vector<bool> rows)
    AttainmentDict attainment_dict(const ioh::logger::eah::AttainmentSuite &suite)
    {
        AttainmentDict problems;
        for (const auto &[problem_id, dimensions] : suite)
        {
            DimensionsDict by_dimension;
            for (const auto &[dimension, instances] : dimensions)
            {
                InstancesDict by_instance;
                for (const auto &[instance, runs] : instances)
                {
                    RunsDict by_run;
                    for (const auto &[run, matrix] : runs)
                        by_run[nb::cast(run)] = nb::cast(make_array_2d(matrix));
                    by_instance[nb::cast(instance)] = by_run;
                }
                by_dimension[nb::cast(dimension)] = by_instance;
            }
            problems[nb::cast(problem_id)] = by_dimension;
        }
        return problems;
    }
} // namespace ioh_python::empirical_attainment

void define_eah(nb::module_ &m)
{
    namespace logger = ioh::logger;
    namespace eah = ioh::logger::eah;
    using logger::EAH;
    using ioh_python::empirical_attainment::attainment_dict;
    using ioh_python::empirical_attainment::define_eah_scale;

    auto eah_module = m.def_submodule("eah");
    define_eah_scale<double>(eah_module, "RealScale");
    define_eah_scale<size_t>(eah_module, "IntegerScale");

    nb::class_<EAH, ioh::Logger>(m, "EAH", "Emperical Attainment Histogram Logger")
        .def(nb::init<double, double, size_t, size_t, size_t, size_t>(), nb::arg("error_min"), nb::arg("error_max"),
             nb::arg("error_buckets"), nb::arg("evals_min"), nb::arg("evals_max"), nb::arg("evals_buckets"))
        .def(nb::init<eah::LinearScale<double> &, eah::LinearScale<size_t> &>(), nb::arg("error_scale"),
             nb::arg("eval_scale"))
        .def(nb::init<eah::Log2Scale<double> &, eah::Log2Scale<size_t> &>(), nb::arg("error_scale"),
             nb::arg("eval_scale"))
        .def(nb::init<eah::Log10Scale<double> &, eah::Log10Scale<size_t> &>(), nb::arg("error_scale"),
             nb::arg("eval_scale"))
        .def(
            "at",
            [](const EAH &l, size_t problem_id, size_t instance_id, size_t dim_id, size_t run) {
                return ioh_python::make_array_2d(l.at(problem_id, instance_id, dim_id, run));
            },
            R"pbdoc(
                The attainment matrix of a run, by problem id, instance, dimension and run: a new two-dimensional
                bool array, error targets (rows) by evaluation targets (columns).
            )pbdoc")
        .def_prop_ro(
            "data", [](const EAH &l) { return attainment_dict(l.data()); },
            "The attainment matrices, in dicts by problem id, dimension, instance and run (new arrays, as at "
            "returns them)")
        .def_prop_ro("size", &EAH::size)
        .def_prop_ro("error_range", &EAH::error_range, nb::rv_policy::reference)
        .def_prop_ro("eval_range", &EAH::eval_range, nb::rv_policy::reference)
        .def("__repr__", [](const EAH &l) { return fmt::format("<EAH {}>", l.size()); });
}

void define_eaf(nb::module_ &m)
{
    namespace logger = ioh::logger;
    namespace eaf = ioh::logger::eaf;
    using logger::EAF;

    auto eaf_module = m.def_submodule("eaf");
    nb::class_<eaf::Point>(eaf_module, "Point")
        .def(nb::init<double, size_t>())
        .def("__repr__", [](const eaf::Point &p) { return fmt::format("<Point {} {}>", p.qual, p.time); });

    nb::class_<eaf::RunPoint, eaf::Point>(eaf_module, "RunPoint")
        .def(nb::init<double, size_t, size_t>())
        .def("__repr__",
             [](const eaf::RunPoint &p) { return fmt::format("<RunPoint {} {} {}>", p.qual, p.time, p.run); });

    nb::class_<EAF, ioh::Logger>(m, "EAF", "Emperical Attainment Function Logger")
        .def(nb::init<>())
        .def_prop_ro("data", nb::overload_cast<>(&EAF::data, nb::const_))
        .def("at", [](EAF &f, std::string suite_name, int pb, int dim, int inst, size_t run) {
            const auto cursor = EAF::Cursor(suite_name, pb, dim, inst, run);
            return f.data(cursor);
        });
}
