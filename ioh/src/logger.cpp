#include "pyproperty.hpp"

#include <map>
#include <memory>
#include <unordered_map>

namespace ioh_python::logger_cpp
{
    //! The method `name` of the Python object of ptr, if it has one: for attach_suite, which has no binding, so that
    //! nanobind's trampoline cannot look it up (it fails for a name that the Python type does not have)
    template <typename T>
    nb::object python_method(const T *ptr, const char *name)
    {
        nb::object self = nb::find(ptr);
        if (!self.is_valid())
            return {};
        return nb::getattr(self, name, nb::handle());
    }

    // Trampoline: lets Python subclass AbstractLogger
    struct AbstractWatcher : ioh::logger::Watcher
    {
        NB_TRAMPOLINE(ioh::logger::Watcher);

        void attach_problem(const ioh::problem::MetaData &problem) override { NB_OVERRIDE(attach_problem, problem); }
        void attach_suite(const std::string &suite_name) override
        {
            nb::gil_scoped_acquire gil;
            if (nb::object f = python_method(static_cast<const ioh::logger::Watcher *>(this), "attach_suite");
                f.is_valid())
                f(suite_name);
            else
                throw std::runtime_error("Tried to call pure virtual function \"AbstractLogger::attach_suite\"");
        }
        void call(const ioh::logger::Info &log_info) override { NB_OVERRIDE_PURE_NAME("__call__", call, log_info); }
    };

    // Python spec. implementation: a watcher that can also watch attributes of Python objects, and whose virtual
    // methods can be overridden by Python subclasses
    template <typename WatcherType>
    class PyWatcher : public WatcherType
    {
    protected:
        std::vector<std::unique_ptr<PyProperty>> property_ptrs_;

    public:
        NB_TRAMPOLINE(WatcherType);

        PyWatcher(const PyWatcher &) = delete;
        PyWatcher &operator=(const PyWatcher &) = delete;
        PyWatcher(PyWatcher &&) = delete;
        PyWatcher &operator=(PyWatcher &&) = delete;

        void watch(ioh::logger::Property &property) override { NB_OVERRIDE(watch, property); }

        void watch(const nb::object &container, const std::string &attribute)
        {
            auto p = std::make_unique<PyProperty>(container, attribute);
            watch(*p);
            property_ptrs_.push_back(std::move(p));
        }

        void watch(const nb::object &container, const std::vector<std::string> &attributes)
        {
            for (const auto &attr : attributes)
                watch(container, attr);
        }

        void attach_problem(const ioh::problem::MetaData &problem) override { NB_OVERRIDE(attach_problem, problem); }

        void attach_suite(const std::string &suite_name) override
        {
            {
                nb::gil_scoped_acquire gil;
                if (nb::object f = python_method(static_cast<const PyWatcher *>(this), "attach_suite"); f.is_valid())
                {
                    f(suite_name);
                    return;
                }
            }
            WatcherType::attach_suite(suite_name);
        }

        void call(const ioh::logger::Info &log_info) override { NB_OVERRIDE(call, log_info); }
    };


    // Python spec. implementation
    template <typename A>
    class PyAnalyzer : public PyWatcher<A>
    {
        std::unordered_map<std::string, std::unique_ptr<double>> double_ptrs_;
        std::vector<std::unique_ptr<PyProperty>> prop_ptrs_;

    public:
        using AnalyzerType = PyWatcher<A>;
        using AnalyzerType::AnalyzerType;

        virtual void close() override { AnalyzerType::close(); }

        virtual ~PyAnalyzer() { close(); }

        void add_run_attribute_python(const nb::object &container, const std::string &name)
        {
            auto p = std::make_unique<PyProperty>(container, name);
            add_run_attribute_python(name, (*p)(ioh::logger::Info{}).value());
            prop_ptrs_.push_back(std::move(p));
        }

        void add_run_attributes_python(const nb::object &container, const std::vector<std::string> &attributes)
        {
            for (const auto &attr : attributes)
                add_run_attribute_python(container, attr);
        }

        void add_run_attribute_python(const std::string &name, double value)
        {
            auto ptr = std::make_unique<double>(value);
            AnalyzerType::add_run_attribute(name, ptr.get());
            double_ptrs_[name] = std::move(ptr);
        }

        void set_run_attribute_python(const std::string &name, double value) { *(double_ptrs_.at(name)) = value; }

        void set_run_attributes_python(const std::map<std::string, double> &attributes)
        {
            for (auto &[key, value] : attributes)
                set_run_attribute_python(key, value);
        }
        virtual void handle_last_eval() override
        {
            for (auto &ptr : prop_ptrs_)
                set_run_attribute_python(ptr->name(), (*ptr)(ioh::logger::Info{}).value());
            AnalyzerType::handle_last_eval();
        }
    };


    void define_bases(nb::module_ &m)
    {
        using ioh::Logger;
        using ioh::logger::Watcher;
        using ioh::logger::Triggers;
        using ioh::logger::Properties;

        nb::class_<Logger>(m, "Logger", "Base class for all loggers")
            .def("add_trigger", &Logger::trigger, "Add a trigger to the logger")
            .def("call", &Logger::call, "Performs logging behaviour")
            .def("reset", &Logger::reset, "Reset the state of the logger")
            .def_prop_ro("problem", &Logger::problem, "Reference to the currently attached problem");

        nb::class_<Watcher, AbstractWatcher, Logger>(m, "AbstractLogger", "Base class for loggers which track properties")
            .def(nb::init<Triggers, Properties>(), nb::arg("triggers") = Triggers{},
                 nb::arg("properties") = Properties{})
            .def("watch", [](Watcher &w, ioh::logger::Property &p) { w.Watcher::watch(p); })
            .def(
                "attach_problem", [](Watcher &w, const ioh::problem::MetaData &m) { w.Watcher::attach_problem(m); },
                nb::arg("problem"), "attach a problem (MetaData) to a logger");
    }

    void define_flatfile(nb::module_ &m)
    {
        using ioh::logger::FlatFile;
        using ioh::logger::Property;
        using ioh::logger::Watcher;
        using ioh::logger::Triggers;
        using ioh::logger::Properties;
        using PyFlatFile = PyWatcher<FlatFile>;

        const std::vector<std::string> common_headers = {
            "suite_name", "problem_name", "problem_id", "problem_instance", "optimization_type", "dimension", "run"};

        nb::class_<PyFlatFile, Watcher>(m, "FlatFile")
            .def(nb::init<Triggers, Properties, std::string, fs::path, std::string, std::string, std::string,
                          std::string, bool, bool, std::vector<std::string>>(),
                 nb::arg("triggers"), nb::arg("properties"), nb::arg("filename") = "IOH.dat",
                 nb::arg("output_directory") = "./", nb::arg("separator") = "\t", nb::arg("comment") = "#",
                 nb::arg("no_value") = "None", nb::arg("end_of_line") = "\n", nb::arg("repeat_header") = false,
                 nb::arg("store_positions") = false, nb::arg("common_header_titles") = common_headers,
                 R"pbdoc(
                A logger which stores all tracked properties to a file.

                Parameters
                ----------
                triggers: list[Trigger]
                    List of triggers, i.e. when to trigger logging
                properties: list[Property]
                    The list of properties to keep track of
                filename: str = "IOH.dat"
                    The file to log to
                output_directory: str = "./"
                    The path to which to write the file
                separator: str = "\t"
                    The column seperator
                comment: str = "#"
                    The comment character
                no_value: str = "None"
                    The NaN character
                end_of_line: str = "\n"
                    The eol character
                repeat_header: bool = false
                    Boolean value which indicates whether to repeat the header for every new run.
                store_positions: bool = false
                    Boolean value which indicates whether to store the x-positions in the file
            )pbdoc"

                 )
            .def_prop_ro("filename", &FlatFile::filename)
            .def_prop_ro("output_directory",
                         [](PyFlatFile &f) { return fs::absolute(f.output_directory()).generic_string(); })
            // bound to FlatFile's own methods, so that super() in a Python override runs them once (a virtual call
            // would come back to the override)
            .def("call", [](PyFlatFile &f, const ioh::logger::Info &info) { f.FlatFile::call(info); },
                 "Performs logging behaviour")
            .def(
                "attach_problem",
                [](PyFlatFile &f, const ioh::problem::MetaData &m) { f.FlatFile::attach_problem(m); },
                nb::arg("problem"), "attach a problem (MetaData) to a logger")
            .def("watch", [](PyFlatFile &f, Property &p) { f.FlatFile::watch(p); })
            .def("watch", nb::overload_cast<const nb::object &, const std::string &>(&PyFlatFile::watch))
            .def("watch", nb::overload_cast<const nb::object &, const std::vector<std::string> &>(&PyFlatFile::watch))
            .def("__repr__", [](const PyFlatFile &f) {
                return fmt::format("<FlatFile {}>", (f.output_directory() / f.filename()).generic_string());
            });
    }

    template <typename A>
    void define_analyzer(nb::module_ &m)
    {
        using ioh::logger::Property;
        using ioh::logger::Watcher;
        using ioh::logger::Triggers;
        using ioh::logger::Properties;
        using PyAnalyzerType = PyAnalyzer<A>;

        Triggers def_trigs{ioh::trigger::on_delta_improvement};
        Properties def_props{};

        nb::class_<PyAnalyzerType, Watcher>(m, "Analyzer")
            .def(nb::init<Triggers, Properties, fs::path, std::string, std::string, std::string, bool>(),
                 nb::arg("triggers") = def_trigs, nb::arg("additional_properties") = def_props,
                 nb::arg("root") = fs::current_path(), nb::arg("folder_name") = "ioh_data",
                 nb::arg("algorithm_name") = "algorithm_name", nb::arg("algorithm_info") = "algorithm_info",
                 nb::arg("store_positions") = false,
                 R"pbdoc(
                A logger which stores all tracked properties to a file.

                Parameters
                ----------
                triggers: list[Trigger]
                    List of triggers, i.e. when to trigger logging
                additional_properties: list[Property]
                    The list of additional properties to keep track of (additional to the default properties for this logger type)
                root: str = "./"
                    The path to which to write the files
                folder_name: str = "./"
                    The name of the folder to which to write the files
                algorithm_name: str = "algorithm_name"
                    Optional name parameter for the algorithm to be added to the info files
                algorithm_info: str = "algorithm_info"
                    Optional info parameter for the algorithm to be added to the info files
                store_positions: bool = false
                    Boolean value which indicates whether to store the x-positions in the file
            )pbdoc")
            .def("add_experiment_attribute", &PyAnalyzerType::add_experiment_attribute)
            .def("set_experiment_attributes", &PyAnalyzerType::set_experiment_attributes)

            .def("add_run_attribute",
                 nb::overload_cast<const std::string &, double>(&PyAnalyzerType::add_run_attribute_python))
            .def("add_run_attribute",
                 nb::overload_cast<const nb::object &, const std::string &>(&PyAnalyzerType::add_run_attribute_python))
            .def("add_run_attributes",
                 nb::overload_cast<const nb::object &, const std::vector<std::string> &>(
                     &PyAnalyzerType::add_run_attributes_python))

            .def("set_run_attributes", &PyAnalyzerType::set_run_attributes_python) // takes a map<str, double>
            .def("set_run_attribute", &PyAnalyzerType::set_run_attribute_python) // takes str, double>
            .def_prop_ro("output_directory",
                         [](const PyAnalyzerType &self) { return self.output_directory().generic_string(); })
            .def("close", &PyAnalyzerType::close)
            // bound to A's own methods, so that super() in a Python override runs them once (a virtual call
            // would come back to the override)
            .def("call", [](PyAnalyzerType &a, const ioh::logger::Info &info) { a.A::call(info); },
                 "Performs logging behaviour")
            .def(
                "attach_problem",
                [](PyAnalyzerType &a, const ioh::problem::MetaData &m) { a.A::attach_problem(m); },
                nb::arg("problem"), "attach a problem (MetaData) to a logger")
            .def("watch", [](PyAnalyzerType &a, Property &p) { a.A::watch(p); })
            .def("watch", nb::overload_cast<const nb::object &, const std::string &>(&PyAnalyzerType::watch))
            .def("watch",
                 nb::overload_cast<const nb::object &, const std::vector<std::string> &>(&PyAnalyzerType::watch))
            .def("__repr__", [](const PyAnalyzerType &f) {
                return fmt::format("<Analyzer {}>", f.output_directory().generic_string());
            });
    }


    void define_store(nb::module_ &m)
    {
        using ioh::logger::Property;
        using ioh::logger::Store;
        using ioh::logger::Watcher;
        using ioh::logger::Triggers;
        using ioh::logger::Properties;
        using PyStore = PyWatcher<Store>;

        nb::class_<PyStore, Watcher>(m, "Store")
            .def(nb::init<Triggers, Properties>(), nb::arg("triggers"), nb::arg("properties"),
                 R"pbdoc(
                A logger which stores all tracked properties in memory.

                Parameters
                ----------
                triggers: list[Trigger]
                    List of triggers, i.e. when to trigger logging
                properties: list[Property]
                    The list of properties to keep track of
            )pbdoc"

                 )
            .def("data", nb::overload_cast<>(&PyStore::data), "Accessor to the internal data container")
            .def(
                "at",
                [](PyStore &f, std::string suite_name, int pb, int dim, int inst, size_t run, size_t evaluation) {
                    const auto cursor = Store::Cursor(suite_name, pb, dim, inst, run, evaluation);
                    return f.data(cursor);
                },
                "Accessor for a specific stored record")
            // bound to Store's own methods, so that super() in a Python override runs them once (a virtual call
            // would come back to the override)
            .def("call", [](PyStore &s, const ioh::logger::Info &info) { s.Store::call(info); },
                 "Performs logging behaviour")
            .def(
                "attach_problem",
                [](PyStore &s, const ioh::problem::MetaData &m) { s.Store::attach_problem(m); },
                nb::arg("problem"), "attach a problem (MetaData) to a logger")
            .def("watch", [](PyStore &s, Property &p) { s.Store::watch(p); }, "Add a property")
            .def("watch", nb::overload_cast<const nb::object &, const std::string &>(&PyStore::watch),
                 "Add a property")
            .def("watch", nb::overload_cast<const nb::object &, const std::vector<std::string> &>(&PyStore::watch),
                 "Add multliple properties")
            .def("__repr__", [](PyStore &f) {
                return fmt::format("<Store (suites: ({}),)>", fmt::join(ioh::common::keys(f.data()), ","));
            });
    }

    void define_loggers(nb::module_ &m)
    {
        using ioh::Logger;
        using ioh::logger::Combine;

        nb::class_<Combine, Logger>(m, "Combine", "Utility class to combine multiple loggers")
            .def(nb::init<std::vector<std::reference_wrapper<Logger>>>(), nb::arg("loggers"))
            .def(nb::init<std::reference_wrapper<Logger>>(), nb::arg("logger"))
            .def("append", &Combine::append);

        define_flatfile(m);
        define_store(m);
        auto old = m.def_submodule("old");
        define_analyzer<ioh::logger::analyzer::v1::Analyzer>(old);
        define_analyzer<ioh::logger::analyzer::v2::Analyzer>(m);

        define_eah(m);
        define_eaf(m);
    }
} // namespace ioh_python::logger_cpp


void define_logger(nb::module_ &m)
{
    // fs::path arguments and results go through nanobind's filesystem caster (str or os.PathLike in, pathlib.Path
    // out); the name Path stays for code that used the class this module had before
    m.attr("Path") = nb::module_::import_("pathlib").attr("Path");

    define_triggers(m);
    define_properties(m);
    ioh_python::logger_cpp::define_bases(m);
    ioh_python::logger_cpp::define_loggers(m);
}
