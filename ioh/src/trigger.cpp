#include "pch.hpp"

void define_triggers(nb::module_ &m)
{
    nb::module_ t = m.def_submodule("trigger");

    nb::class_<logger::Trigger>(t, "Trigger", "Base class for all Triggers")
        .def("__call__", &logger::Trigger::operator())
        .def("reset", &logger::Trigger::reset);

    nb::class_<trigger::Always, logger::Trigger>(t, "Always", "Trigger that always evaluates to true")
        .def(nb::init<>())
        .def("__getstate__", [](const trigger::Always &) { return nb::make_tuple(); })
        .def("__setstate__", [](nb::tuple) { return trigger::Always{}; });

    t.attr("ALWAYS") = nb::cast(trigger::always);

    nb::class_<trigger::OnImprovement, logger::Trigger>(
        t, "OnImprovement", "Trigger that evaluates to true when improvement of the objective function is observed")
        .def(nb::init<>())
        .def("__getstate__", [](const trigger::OnImprovement &) { return nb::make_tuple(); })
        .def("__setstate__", [](nb::tuple) { return trigger::OnImprovement{}; });

    ;
    t.attr("ON_IMPROVEMENT") = nb::cast(trigger::on_improvement);

    nb::class_<trigger::OnDeltaImprovement, logger::Trigger>(
        t, "OnDeltaImprovement",
        "Trigger that evaluates to true when improvement of the objective function is observed of at least greater "
        "than delta")
        .def(nb::init<double>(), nb::arg("delta") = 1e-10)
        .def_rw("delta", &trigger::OnDeltaImprovement::delta)
        .def_ro("best_so_far", &trigger::OnDeltaImprovement::best_so_far)
        .def("__getstate__",
             [](const trigger::OnDeltaImprovement &t) { return nb::make_tuple(t.delta, t.best_so_far); })
        .def("__setstate__",
             [](nb::tuple t) { return trigger::OnDeltaImprovement{nb::cast<double>(t[0]), nb::cast<double>(t[1])}; });

    ;

    t.attr("ON_DELTA_IMPROVEMENT") = nb::cast(trigger::on_delta_improvement);

    nb::class_<trigger::OnViolation, logger::Trigger>(
        t, "OnViolation", "Trigger that evaluates to true when there is a contraint violation")
        .def(nb::init<>())
        .def_ro("violations", &trigger::OnViolation::violations)
        .def("__getstate__", [](const trigger::OnViolation &) { return nb::make_tuple(); })
        .def("__setstate__", [](nb::tuple) { return trigger::OnViolation{}; });
    ;
    t.attr("ON_VIOLATION") = nb::cast(trigger::on_violation);

    nb::class_<trigger::At, logger::Trigger>(t, "At")
        .def(nb::init<std::set<size_t>>(), nb::arg("time_points"),
             R"pbdoc(
                Trigger that evaluates to true at specific time points

                Parameters
                ----------
                time_points: list[int]
                    The time points at which to trigger
            )pbdoc")
        .def_prop_ro("time_points", &trigger::At::time_points)
        .def("__getstate__", [](const trigger::At &t) { return nb::make_tuple(t.time_points()); })
        .def("__setstate__", [](nb::tuple t) { return trigger::At{nb::cast<std::set<size_t>>(t[0])}; });

    nb::class_<trigger::Each, logger::Trigger>(t, "Each")
        .def(nb::init<size_t, size_t>(), nb::arg("interval"), nb::arg("starting_at") = 0,
             R"pbdoc(
                Trigger that evaluates to true at a given interval

                Parameters
                ----------
                interval: int
                    The interval at which to trigger
                starting_at: int
                    The starting time of the interval, defaults to 0.
            )pbdoc")
        .def_prop_ro("interval", &trigger::Each::interval)
        .def_prop_ro("starting_at", &trigger::Each::starting_at)
        .def("__getstate__", [](const trigger::Each &t) { return nb::make_tuple(t.interval(), t.starting_at()); })
        .def("__setstate__", [](nb::tuple t) { return trigger::Each{nb::cast<size_t>(t[0]), nb::cast<size_t>(t[1])}; });


    nb::class_<trigger::During, logger::Trigger>(t, "During")
        .def(nb::init<std::set<std::pair<size_t, size_t>>>(), nb::arg("time_ranges"),
             R"pbdoc(
                Trigger that evaluates to true during a given interval.

                Parameters
                ----------
                time_ranges: set[tuple[int, int]]
                    The ranges/intervals at which the log the data.                    
            )pbdoc"

             )
        .def_prop_ro("time_ranges", &trigger::During::time_ranges)
        .def("__getstate__", [](const trigger::During &t) { return nb::make_tuple(t.time_ranges()); })
        .def("__setstate__",
             [](nb::tuple t) { return trigger::During{nb::cast<std::set<std::pair<size_t, size_t>>>(t[0])}; });
}
