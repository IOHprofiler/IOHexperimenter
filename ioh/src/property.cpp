#include "pyproperty.hpp"

// Trampoline
struct AbstractProperty : logger::Property {
    AbstractProperty(const std::string &name) : logger::Property(name) {}

    // Overloaded call_to_string
    std::string call_to_string(const logger::Info<double> &log_info, const std::string &nan = "") const override {
        PYBIND11_OVERRIDE(std::string, logger::Property, call_to_string, log_info, nan);
    }

    std::string call_to_string(const logger::Info<std::vector<double>> &log_info, const std::string &nan = "") const override {
        PYBIND11_OVERRIDE(std::string, logger::Property, call_to_string, log_info, nan);
    }

    // Overloaded operator()
    std::optional<double> operator()(const logger::Info<double> &info) const override {
        PYBIND11_OVERRIDE_PURE_NAME(std::optional<double>, logger::Property, "__call__", operator(), info);
    }

    std::optional<double> operator()(const logger::Info<std::vector<double>> &info) const override {
        PYBIND11_OVERRIDE_PURE_NAME(std::optional<double>, logger::Property, "__call__", operator(), info);
    }
};

template <typename P>
void define_property(py::module &m, std::string name, P predef)
{

    py::class_<P, logger::Property, std::shared_ptr<P>>(m, name.c_str(), py::buffer_protocol())
        .def(py::init<std::string, std::string>(), py::arg("name"), py::arg("format"),
             ("Property which tracks the " + name).c_str())
        .def(py::pickle([](const P &t) { return py::make_tuple(t.name(), t.format()); },
                        [](py::tuple t) {
                            return P{t[0].cast<std::string>(), t[1].cast<std::string>()};
                        }));

    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
    m.attr(name.c_str()) = py::cast(predef);
}

void define_properties(py::module &m)
{
    py::module t = m.def_submodule("property");

    py::class_<logger::Property, AbstractProperty, std::shared_ptr<logger::Property>>(t, "AbstractProperty",
        "Base class for all Properties")
        .def(py::init<std::string>())
        // Bind overloaded call_to_string
        .def("call_to_string",
            static_cast<std::string (logger::Property::*)(const logger::Info<double> &, const std::string &) const>(
                &logger::Property::call_to_string),
            py::arg("log_info"), py::arg("nan") = "")
       .def("call_to_string",
            static_cast<std::string (logger::Property::*)(const logger::Info<std::vector<double>> &, const std::string &) const>(
                &logger::Property::call_to_string),
            py::arg("log_info"), py::arg("nan") = "")
        .def("__call__",
            py::overload_cast<const logger::Info<double> &>(&logger::Property::operator(), py::const_),
            py::arg("info"))
        .def("__call__",
            py::overload_cast<const logger::Info<std::vector<double>> &>(&logger::Property::operator(), py::const_),
            py::arg("info"))
        .def_property("name", &logger::Property::name, &logger::Property::set_name);


    py::class_<PyProperty, logger::Property, std::shared_ptr<PyProperty>>(t, "Property")
        .def(py::init<py::object, std::string>(), py::arg("container"), py::arg("attribute"),
             R"pbdoc(
                Wrapper for properties with a Python container
    
                Parameters
                ----------
                container: object
                    The object which contains the property to be logged
                attribute: str
                    The name of the property to be logged, must be an attribute on container          
            )pbdoc")
        // Bind overloaded call_to_string
        .def("__call__",
             py::overload_cast<const logger::Info<double> &>(&PyProperty::operator(), py::const_),
             py::arg("info"))
        .def("__call__",
             py::overload_cast<const logger::Info<std::vector<double>> &>(&PyProperty::operator(), py::const_),
             py::arg("info"))
        .def(py::pickle([](const PyProperty &t) { return py::make_tuple(t.container(), t.name()); },
                        [](py::tuple t) {
                            return PyProperty{t[0].cast<py::object>(), t[1].cast<std::string>()};
                        }));

    define_property<watch::Evaluations>(t, "Evaluations", watch::evaluations);

    define_property<watch::RawY>(t, "RawY", watch::raw_y);
    define_property<watch::RawYBest>(t, "RawYBest", watch::raw_y_best);

    define_property<watch::TransformedY>(t, "TransformedY", watch::transformed_y);
    define_property<watch::TransformedYBest>(t, "TransformedYBest", watch::transformed_y_best);

    define_property<watch::CurrentY>(t, "CurrentY", watch::current_y);
    define_property<watch::CurrentBestY>(t, "CurrentBestY", watch::current_y_best);

    define_property<watch::Violation>(t, "Violation", watch::violation);
    define_property<watch::Penalty>(t, "Penalty", watch::penalty);
}