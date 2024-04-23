#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <utility>
#include "ioh.hpp"

namespace py = pybind11;
using namespace ioh;


template <typename T>
void define_eah_scale(py::module &m, const std::string &name)
{
    using namespace logger::eah;

    py::class_<Scale<T>, std::shared_ptr<Scale<T>>>(m, name.c_str())
        .def_property_readonly("min", &Scale<T>::min)
        .def_property_readonly("max", &Scale<T>::max)
        .def_property_readonly("size", &Scale<T>::size)
        .def_property_readonly("length", &Scale<T>::length)
        .def("index", &Scale<T>::index)
        .def("bounds", &Scale<T>::bounds)
        .def("__repr__", [name](const Scale<T> &s) {
            return fmt::format("<{} (({}, {}), {})>", name, s.min(), s.max(), s.size());
        });

    py::class_<LinearScale<T>, Scale<T>, std::shared_ptr<LinearScale<T>>>(m, ("Linear" + name).c_str())
        .def(py::init<T, T, size_t>())
        .def("step", &LinearScale<T>::step);
    py::class_<Log2Scale<T>, Scale<T>, std::shared_ptr<Log2Scale<T>>>(m, ("Log2" + name).c_str())
        .def(py::init<T, T, size_t>());

    py::class_<Log10Scale<T>, Scale<T>, std::shared_ptr<Log10Scale<T>>>(m, ("Log10" + name).c_str())
        .def(py::init<T, T, size_t>());
}
void define_eah(py::module &m)
{
    using namespace logger;
    auto eah = m.def_submodule("eah");
    define_eah_scale<double>(eah, "RealScale");
    define_eah_scale<size_t>(eah, "IntegerScale");

    py::class_<EAH, Logger, std::shared_ptr<EAH>>(m, "EAH", "Emperical Attainment Histogram Logger")
        .def(py::init<double, double, size_t, size_t, size_t, size_t>(), py::arg("error_min"), py::arg("error_max"),
             py::arg("error_buckets"), py::arg("evals_min"), py::arg("evals_max"), py::arg("evals_buckets"))
        .def(py::init<eah::LinearScale<double> &, eah::LinearScale<size_t> &>(), py::arg("error_scale"),
             py::arg("eval_scale"))
        .def(py::init<eah::Log2Scale<double> &, eah::Log2Scale<size_t> &>(), py::arg("error_scale"),
             py::arg("eval_scale"))
        .def(py::init<eah::Log10Scale<double> &, eah::Log10Scale<size_t> &>(), py::arg("error_scale"),
             py::arg("eval_scale"))
        .def("at", &logger::EAH::at)
        .def_property_readonly("data", &logger::EAH::data)
        .def_property_readonly("size", &logger::EAH::size)
        .def_property_readonly("error_range", &logger::EAH::error_range, py::return_value_policy::reference)
        .def_property_readonly("eval_range", &logger::EAH::eval_range, py::return_value_policy::reference)
        .def("__repr__", [](const logger::EAH &l) { return fmt::format("<EAH {}>", l.size()); });
}

void define_eaf(py::module &m)
{
    using namespace logger;

    auto eaf = m.def_submodule("eaf");
    py::class_<eaf::Point, std::shared_ptr<eaf::Point>>(eaf, "Point")
        .def(py::init<double, size_t>())
        .def("__repr__", [](const eaf::Point &p) { return fmt::format("<Point {} {}>", p.qual, p.time); });

    py::class_<eaf::RunPoint, eaf::Point, std::shared_ptr<eaf::RunPoint>>(eaf, "RunPoint")
        .def(py::init<double, size_t, size_t>())
        .def("__repr__",
             [](const eaf::RunPoint &p) { return fmt::format("<RunPoint {} {} {}>", p.qual, p.time, p.run); });

    py::class_<EAF, Logger, std::shared_ptr<EAF>>(m, "EAF", "Emperical Attainment Function Logger")
        .def(py::init<>())
        .def_property_readonly("data", py::overload_cast<>(&EAF::data, py::const_))
        .def("at", [](EAF &f, std::string suite_name, int pb, int dim, int inst, size_t run) {
            const auto cursor = EAF::Cursor(suite_name, pb, dim, inst, run);
            return f.data(cursor);
        });
}