#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <utility>
#include "ioh.hpp"

namespace py = pybind11;
using namespace ioh;

void define_triggers(py::module &m)
{
    py::module t = m.def_submodule("trigger");

    py::class_<logger::Trigger, std::shared_ptr<logger::Trigger>>(t, "Trigger", "Base class for all Triggers")
        .def("__call__", &logger::Trigger::operator())
        .def("reset", &logger::Trigger::reset);

    py::class_<trigger::Always, logger::Trigger, std::shared_ptr<trigger::Always>>(
        t, "Always", "Trigger that always evaluates to true")
        .def(py::init<>())
        .def(py::pickle([](const trigger::Always &) { return py::make_tuple(); },
                        [](py::tuple) { return trigger::Always{}; }));

    t.attr("ALWAYS") = py::cast(trigger::always);

    py::class_<trigger::OnImprovement, logger::Trigger, std::shared_ptr<trigger::OnImprovement>>(
        t, "OnImprovement", "Trigger that evaluates to true when improvement of the objective function is observed")
        .def(py::init<>())
        .def(py::pickle([](const trigger::OnImprovement &) { return py::make_tuple(); },
                        [](py::tuple) { return trigger::OnImprovement{}; }));

    ;
    t.attr("ON_IMPROVEMENT") = py::cast(trigger::on_improvement);

    py::class_<trigger::OnDeltaImprovement, logger::Trigger, std::shared_ptr<trigger::OnDeltaImprovement>>(
        t, "OnDeltaImprovement",
        "Trigger that evaluates to true when improvement of the objective function is observed of at least greater "
        "than delta")
        .def(py::init<double>(), py::arg("delta") = 1e-10)
        .def_readwrite("delta", &trigger::OnDeltaImprovement::delta)
        .def_readonly("best_so_far", &trigger::OnDeltaImprovement::best_so_far)
        .def(py::pickle([](const trigger::OnDeltaImprovement  &t) { return py::make_tuple(t.delta, t.best_so_far); },
                        [](py::tuple t) {
                            return trigger::OnDeltaImprovement{t[0].cast<double>(), t[1].cast<double>()};
                        }));

    ;

    t.attr("ON_DELTA_IMPROVEMENT") = py::cast(trigger::on_delta_improvement);

    py::class_<trigger::OnViolation, logger::Trigger, std::shared_ptr<trigger::OnViolation>>(
        t, "OnViolation", "Trigger that evaluates to true when there is a contraint violation")
        .def(py::init<>()) 
        .def_readonly("violations", &trigger::OnViolation::violations)
        .def(py::pickle([](const trigger::OnViolation &) { return py::make_tuple(); },
                        [](py::tuple) { return trigger::OnViolation{}; }));

    ;
    t.attr("ON_VIOLATION") = py::cast(trigger::on_violation);

    py::class_<trigger::At, logger::Trigger, std::shared_ptr<trigger::At>>(t, "At")
        .def(py::init<std::set<size_t>>(), py::arg("time_points"),
             R"pbdoc(
                Trigger that evaluates to true at specific time points

                Parameters
                ----------
                time_points: list[int]
                    The time points at which to trigger
            )pbdoc")
        .def_property_readonly("time_points", &trigger::At::time_points)
        .def(py::pickle([](const trigger::At &t) { return py::make_tuple(t.time_points()); },
                        [](py::tuple t) { return trigger::At{t[0].cast<std::set<size_t>>()}; }));

    py::class_<trigger::Each, logger::Trigger, std::shared_ptr<trigger::Each>>(t, "Each")
        .def(py::init<size_t, size_t>(), py::arg("interval"), py::arg("starting_at") = 0,
             R"pbdoc(
                Trigger that evaluates to true at a given interval

                Parameters
                ----------
                interval: int
                    The interval at which to trigger
                starting_at: int
                    The starting time of the interval, defaults to 0.
            )pbdoc")
        .def_property_readonly("interval", &trigger::Each::interval)
        .def_property_readonly("starting_at", &trigger::Each::starting_at)
        .def(py::pickle([](const trigger::Each &t) { return py::make_tuple(t.interval(), t.starting_at()); },
                        [](py::tuple t) {
                            return trigger::Each{t[0].cast<size_t>(), t[1].cast<size_t>()};
                        }));

    py::class_<trigger::During, logger::Trigger, std::shared_ptr<trigger::During>>(t, "During")
        .def(py::init<std::set<std::pair<size_t, size_t>>>(), py::arg("time_ranges"),
             R"pbdoc(
                Trigger that evaluates to true during a given interval.

                Parameters
                ----------
                time_ranges: set[tuple[int, int]]
                    The ranges/intervals at which the log the data.                    
            )pbdoc"

             )
        .def_property_readonly("time_ranges", &trigger::During::time_ranges)
        .def(py::pickle([](const trigger::During &t) { return py::make_tuple(t.time_ranges()); },
                        [](py::tuple t) { return trigger::During{t[0].cast<std::set<std::pair<size_t, size_t>>>()}; }));
}
