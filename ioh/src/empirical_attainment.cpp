#include "pch.hpp"

template <typename T>
void define_eah_scale(nb::module_ &m, const std::string &name)
{
    using namespace logger::eah;
    using BaseScale = Scale<T>;

    nb::class_<BaseScale>(m, name.c_str())
        .def_prop_ro("min", &BaseScale::min)
        .def_prop_ro("max", &BaseScale::max)
        .def_prop_ro("size", &BaseScale::size)
        .def_prop_ro("length", &BaseScale::length)
        .def("index", &BaseScale::index)
        .def("bounds", &BaseScale::bounds)
        .def("__repr__", [name](const BaseScale &s) {
            return fmt::format("<{} (({}, {}), {})>", name, s.min(), s.max(), s.size());
        });

    nb::class_<LinearScale<T>, BaseScale>(m, ("Linear" + name).c_str())
        .def(nb::init<T, T, size_t>())
        .def("step", &LinearScale<T>::step);

    nb::class_<Log2Scale<T>, BaseScale>(m, ("Log2" + name).c_str())
        .def(nb::init<T, T, size_t>());

    nb::class_<Log10Scale<T>, BaseScale>(m, ("Log10" + name).c_str())
        .def(nb::init<T, T, size_t>());
}
void define_eah(nb::module_ &m)
{
    using namespace logger;
    auto eah = m.def_submodule("eah");
    define_eah_scale<double>(eah, "RealScale");
    define_eah_scale<size_t>(eah, "IntegerScale");

    nb::class_<EAH, Logger>(m, "EAH", "Emperical Attainment Histogram Logger")
        .def(nb::init<double, double, size_t, size_t, size_t, size_t>(), nb::arg("error_min"), nb::arg("error_max"),
             nb::arg("error_buckets"), nb::arg("evals_min"), nb::arg("evals_max"), nb::arg("evals_buckets"))
        .def(nb::init<eah::LinearScale<double> &, eah::LinearScale<size_t> &>(), nb::arg("error_scale"),
             nb::arg("eval_scale"))
        .def(nb::init<eah::Log2Scale<double> &, eah::Log2Scale<size_t> &>(), nb::arg("error_scale"),
             nb::arg("eval_scale"))
        .def(nb::init<eah::Log10Scale<double> &, eah::Log10Scale<size_t> &>(), nb::arg("error_scale"),
             nb::arg("eval_scale"))
        .def("at", &logger::EAH::at)
        .def_prop_ro("data", &logger::EAH::data)
        .def_prop_ro("size", &logger::EAH::size)
        .def_prop_ro("error_range", &logger::EAH::error_range, nb::rv_policy::reference)
        .def_prop_ro("eval_range", &logger::EAH::eval_range, nb::rv_policy::reference)
        .def("__repr__", [](const logger::EAH &l) { return fmt::format("<EAH {}>", l.size()); });
}

void define_eaf(nb::module_ &m)
{
    using namespace logger;

    auto eaf = m.def_submodule("eaf");
    nb::class_<eaf::Point>(eaf, "Point")
        .def(nb::init<double, size_t>())
        .def("__repr__", [](const eaf::Point &p) { return fmt::format("<Point {} {}>", p.qual, p.time); });

    nb::class_<eaf::RunPoint, eaf::Point>(eaf, "RunPoint")
        .def(nb::init<double, size_t, size_t>())
        .def("__repr__",
             [](const eaf::RunPoint &p) { return fmt::format("<RunPoint {} {} {}>", p.qual, p.time, p.run); });

    nb::class_<EAF, Logger>(m, "EAF", "Emperical Attainment Function Logger")
        .def(nb::init<>())
        .def_prop_ro("data", nb::overload_cast<>(&EAF::data, nb::const_))
        .def("at", [](EAF &f, std::string suite_name, int pb, int dim, int inst, size_t run) {
            const auto cursor = EAF::Cursor(suite_name, pb, dim, inst, run);
            return f.data(cursor);
        });
}