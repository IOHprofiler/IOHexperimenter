#include "pch.hpp"
#include <nanobind/trampoline.h>


// Trampoline
struct AbstractWatcher : logger::Watcher
{
    NB_TRAMPOLINE(logger::Watcher, 3);
    
    // using logger::Watcher::Watcher;

    void attach_problem(const problem::MetaData &problem) override
    {
        NB_OVERRIDE(attach_problem, problem);
    }
    void attach_suite(const std::string &suite_name) override
    {
        NB_OVERRIDE(attach_suite, suite_name);
    }
    void call(const logger::Info &log_info) override
    {
        NB_OVERRIDE(call, log_info);
    }
};

// Python spec. implementation
template <typename WatcherType>
class PyWatcher : public WatcherType
{
protected:
    std::vector<std::unique_ptr<PyProperty>> property_ptrs_;

public:
    NB_TRAMPOLINE(WatcherType, 4);

    void watch(logger::Property &property) override { NB_OVERRIDE(watch, property); }

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

    void attach_problem(const problem::MetaData &problem) override
    {
        NB_OVERRIDE(attach_problem, problem);
    }
    void attach_suite(const std::string &suite_name) override
    {
        NB_OVERRIDE(attach_suite, suite_name);
    }

    void call(const logger::Info &log_info) override { NB_OVERRIDE(call, log_info); }
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
        add_run_attribute_python(name, (*p)(logger::Info{}).value());
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
            set_run_attribute_python(ptr->name(), (*ptr)(logger::Info{}).value());
        AnalyzerType::handle_last_eval();
    }
};


void define_bases(nb::module_ &m)
{
    nb::class_<Logger>(m, "Logger", "Base class for all loggers")
        .def("add_trigger", &Logger::trigger, "Add a trigger to the logger")
        .def("call", &Logger::call, "Performs logging behaviour")
        .def("reset", &Logger::reset, "Reset the state of the logger")
        .def_prop_ro("problem", &Logger::problem, "Reference to the currently attached problem");

    using namespace logger;
    nb::class_<Watcher, AbstractWatcher, Logger>(
        m, "AbstractLogger", "Base class for loggers which track properties")
        .def(nb::init<Triggers, Properties>(), nb::arg("triggers") = Triggers{}, nb::arg("properties") = Properties{})
        .def("watch", &Watcher::watch)
        .def("attach_problem", &Watcher::attach_problem, nb::arg("problem"), "attach a problem (MetaData) to a logger");
}

void define_flatfile(nb::module_ &m)
{
    using namespace logger;
    const std::vector<std::string> common_headers = {
        "suite_name", "problem_name", "problem_id", "problem_instance", "optimization_type", "dimension", "run"};

    nb::class_<PyWatcher<FlatFile>, Watcher, std::shared_ptr<PyWatcher<FlatFile>>>(m, "FlatFile")
        .def(nb::init<Triggers, Properties, std::string, fs::path, std::string, std::string, std::string, std::string,
                      bool, bool, std::vector<std::string>>(),
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
        .def_prop_ro(
            "output_directory",
            [](PyWatcher<FlatFile> &f) { return fs::absolute(f.output_directory()).generic_string(); })
        .def("watch", nb::overload_cast<Property &>(&PyWatcher<FlatFile>::watch))
        .def("watch", nb::overload_cast<const nb::object &, const std::string &>(&PyWatcher<FlatFile>::watch))
        .def("watch",
             nb::overload_cast<const nb::object &, const std::vector<std::string> &>(&PyWatcher<FlatFile>::watch))
        .def("__repr__", [](const PyWatcher<FlatFile> &f) {
            return fmt::format("<FlatFile {}>", (f.output_directory() / f.filename()).generic_string());
        });
}

template <typename A>
void define_analyzer(nb::module_ &m)
{
    using namespace logger;
    Triggers def_trigs{trigger::on_delta_improvement};
    Properties def_props{};
    using PyAnalyzer = PyAnalyzer<A>;
    nb::class_<PyAnalyzer, Watcher, std::shared_ptr<PyAnalyzer>>(m, "Analyzer")
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
        .def("add_experiment_attribute", &PyAnalyzer::add_experiment_attribute)
        .def("set_experiment_attributes", &PyAnalyzer::set_experiment_attributes)

        .def("add_run_attribute", nb::overload_cast<const std::string &, double>(&PyAnalyzer::add_run_attribute_python))
        .def("add_run_attribute",
             nb::overload_cast<const nb::object &, const std::string &>(&PyAnalyzer::add_run_attribute_python))
        .def("add_run_attributes",
             nb::overload_cast<const nb::object &, const std::vector<std::string> &>(
                 &PyAnalyzer::add_run_attributes_python))

        .def("set_run_attributes", &PyAnalyzer::set_run_attributes_python) // takes a map<str, double>
        .def("set_run_attribute", &PyAnalyzer::set_run_attribute_python) // takes str, double>
        .def_prop_ro("output_directory",
                               [](const PyAnalyzer &self) { return self.output_directory().generic_string(); })
        .def("close", &PyAnalyzer::close)
        .def("watch", nb::overload_cast<Property &>(&PyAnalyzer::watch))
        .def("watch", nb::overload_cast<const nb::object &, const std::string &>(&PyAnalyzer::watch))
        .def("watch", nb::overload_cast<const nb::object &, const std::vector<std::string> &>(&PyAnalyzer::watch))
        .def("__repr__",
             [](const PyAnalyzer &f) { return fmt::format("<Analyzer {}>", f.output_directory().generic_string()); });
}


void define_store(nb::module_ &m)
{
    using namespace logger;

    using PyStore = PyWatcher<Store>;
    nb::class_<PyStore, Watcher, std::shared_ptr<PyStore>>(m, "Store")
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
        .def("watch", nb::overload_cast<Property &>(&PyStore::watch), "Add a property")
        .def("watch", nb::overload_cast<const nb::object &, const std::string &>(&PyStore::watch), "Add a property")
        .def("watch", nb::overload_cast<const nb::object &, const std::vector<std::string> &>(&PyStore::watch),
             "Add multliple properties")
        .def("__repr__", [](PyStore &f) {
            return fmt::format("<Store (suites: ({}),)>", fmt::join(ioh::common::keys(f.data()), ","));
        });
}


void define_eah(nb::module_ &m);
void define_eaf(nb::module_ &m);
void define_properties(nb::module_ &m);
void define_triggers(nb::module_ &m);

void define_loggers(nb::module_ &m)
{
    using namespace logger;
    nb::class_<Combine, Logger, std::shared_ptr<Combine>>(m, "Combine", "Utility class to combine multiple loggers")
        .def(nb::init<std::vector<std::reference_wrapper<Logger>>>(), nb::arg("loggers"))
        .def(nb::init<std::reference_wrapper<Logger>>(), nb::arg("logger"))
        .def("append", &Combine::append);

    define_flatfile(m);
    define_store(m);
    auto old = m.def_submodule("old");
    define_analyzer<logger::analyzer::v1::Analyzer>(old);
    define_analyzer<logger::analyzer::v2::Analyzer>(m);

    define_eah(m);
    define_eaf(m);
}


void define_logger(nb::module_ &m)
{
    nb::class_<fs::path>(m, "Path").def(nb::init<std::string>());
    nb::implicitly_convertible<std::string, fs::path>();

    define_triggers(m);
    define_properties(m);
    define_bases(m);
    define_loggers(m);
}
