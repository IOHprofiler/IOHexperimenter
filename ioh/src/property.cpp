#include "pch.hpp"

using namespace ioh::logger;

// Trampoline
struct AbstractProperty : logger::Property
{
    AbstractProperty(const std::string &name) : logger::Property(name) {}

    std::string call_to_string(const logger::Info &log_info, const std::string &nan = "") const override
    {
        PYBIND11_OVERRIDE(std::string, logger::Property, call_to_string, log_info, nan);
    }

    std::optional<double> operator()(const logger::Info &info) const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(std::optional<double>, logger::Property, "__call__", operator(), info);
    }
};



template <typename P>
void define_property(nb::module_ &m, std::string name, P predef)
{

    nb::class_<P, logger::Property, std::shared_ptr<P>>(m, name.c_str())
        .def(nb::init<std::string, std::string>(), nb::arg("name"), nb::arg("format"),
             ("Property which tracks the " + name).c_str())
        .def(nb::pickle([](const P &t) { return nb::make_tuple(t.name(), t.format()); },
                        [](nb::tuple t) {
                            return P{t[0].cast<std::string>(), t[1].cast<std::string>()};
                        }));

    std::transform(name.begin(), name.end(), name.begin(), ::toupper);
    m.attr(name.c_str()) = nb::cast(predef);
}

void define_properties(nb::module_ &m)
{
    nb::module_ t = m.def_submodule("property");

    nb::class_<logger::Property, AbstractProperty, std::shared_ptr<logger::Property>>(t, "AbstractProperty",
                                                                                      "Base class for all Properties")
        .def(nb::init<std::string>())
        .def("__call__", &logger::Property::operator())
        .def_prop_rw("name", &logger::Property::name, &logger::Property::set_name)
        .def("call_to_string", &logger::Property::call_to_string);


    nb::class_<PyProperty, logger::Property, std::shared_ptr<PyProperty>>(t, "Property")
        .def(nb::init<nb::object, std::string>(), nb::arg("container"), nb::arg("attribute"),
             R"pbdoc(
                Wrapper for properties with a Python container

                Parameters
                ----------
                container: object
                    The object which contains the property to be logged
                attribute: str
                    The name of the property to be logger, must be an attribute on container          
            )pbdoc"

             )
        .def("call_to_string", &PyProperty::call_to_string)
        .def(nb::pickle([](const PyProperty &t) { return nb::make_tuple(t.container(), t.name()); },
                        [](nb::tuple t) {
                            return PyProperty{t[0].cast<nb::object>(), t[1].cast<std::string>()};
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