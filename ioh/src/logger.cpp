#include "pyproperty.hpp"


// Trampoline
template<typename R>
struct AbstractWatcher : logger::Watcher<R>
{
    using logger::Watcher<R>::Watcher;

    void attach_problem(const problem::MetaData &problem) override
    {
        PYBIND11_OVERRIDE(void, logger::Watcher<R>, attach_problem, problem);
    }
    void attach_suite(const std::string &suite_name) override
    {
        PYBIND11_OVERRIDE_PURE(void, logger::Watcher<R>, attach_suite, suite_name);
    }
    void call(const logger::Info<R> &log_info) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(void, logger::Watcher<R>, "__call__", call, log_info);
    }
};

// Python spec. implementation
template <typename WatcherType>
class PyWatcher : public WatcherType
{
protected:
    std::vector<std::unique_ptr<PyProperty>> property_ptrs_;

public:
    using WatcherType::WatcherType;

    void watch(logger::Property &property) override { PYBIND11_OVERRIDE(void, WatcherType, watch, property); }

    void watch(const py::object &container, const std::string &attribute)
    {
        auto p = std::make_unique<PyProperty>(container, attribute);
        watch(*p);
        property_ptrs_.push_back(std::move(p));
    }

    void watch(const py::object &container, const std::vector<std::string> &attributes)
    {
        for (const auto &attr : attributes)
            watch(container, attr);
    }

    void attach_problem(const problem::MetaData &problem) override
    {
        PYBIND11_OVERRIDE(void, WatcherType, attach_problem, problem);
    }

    void attach_suite(const std::string &suite_name) override
    {
        PYBIND11_OVERRIDE(void, WatcherType, attach_suite, suite_name);
    }
    
    void call(const logger::Info<double> &log_info) override 
    {
        PYBIND11_OVERRIDE(void, WatcherType, call, log_info);
    }

};

template <typename WatcherType>
class PyMultiWatcher : public WatcherType
{
protected:
    std::vector<std::unique_ptr<PyProperty>> property_ptrs_;

public:
    using WatcherType::WatcherType;

    void watch(logger::Property &property) override { PYBIND11_OVERRIDE(void, WatcherType, watch, property); }

    void watch(const py::object &container, const std::string &attribute)
    {
        auto p = std::make_unique<PyProperty>(container, attribute);
        watch(*p);
        property_ptrs_.push_back(std::move(p));
    }

    void watch(const py::object &container, const std::vector<std::string> &attributes)
    {
        for (const auto &attr : attributes)
            watch(container, attr);
    }

    void attach_problem(const problem::MetaData &problem) override
    {
        PYBIND11_OVERRIDE(void, WatcherType, attach_problem, problem);
    }

    void attach_suite(const std::string &suite_name) override
    {
        PYBIND11_OVERRIDE(void, WatcherType, attach_suite, suite_name);
    }

    
    void call(const logger::Info<std::vector<double>> &log_info) {
        PYBIND11_OVERRIDE(void, WatcherType, call, log_info);
    }

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

    void add_run_attribute_python(const py::object &container, const std::string &name)
    {
        auto p = std::make_unique<PyProperty>(container, name);
        add_run_attribute_python(name, (*p)(logger::Info<double>{}).value());
        prop_ptrs_.push_back(std::move(p));
    }

    void add_run_attributes_python(const py::object &container, const std::vector<std::string> &attributes)
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
            set_run_attribute_python(ptr->name(), (*ptr)(logger::Info<double>{}).value());
        AnalyzerType::handle_last_eval();
    }
};



// Python spec. implementation
template <typename A>
class PyMultiAnalyzer : public PyMultiWatcher<A>
{
    std::unordered_map<std::string, std::unique_ptr<double>> double_ptrs_;
    std::vector<std::unique_ptr<PyProperty>> prop_ptrs_;

public:
    using AnalyzerType = PyMultiWatcher<A>;
    using AnalyzerType::AnalyzerType;

    virtual void close() override { AnalyzerType::close(); }

    virtual ~PyMultiAnalyzer() { close(); }

    void add_run_attribute_python(const py::object &container, const std::string &name)
    {
        auto p = std::make_unique<PyProperty>(container, name);
        add_run_attribute_python(name, (*p)(logger::Info<std::vector<double>>{}).value());
        prop_ptrs_.push_back(std::move(p));
    }

    void add_run_attributes_python(const py::object &container, const std::vector<std::string> &attributes)
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
            set_run_attribute_python(ptr->name(), (*ptr)(logger::Info<std::vector<double>>{}).value());
        AnalyzerType::handle_last_eval();
    }
};


void define_bases(py::module &m)
{   
    using SingleObjectiveLogger = Logger<double>;
    using MultiObjectiveLogger = Logger<std::vector<double>>;

    py::class_<SingleObjectiveLogger, std::shared_ptr<SingleObjectiveLogger>>(m, "Logger", "Base class for all loggers")
        .def("add_trigger", &SingleObjectiveLogger::trigger, "Add a trigger to the logger")
        .def("call", &SingleObjectiveLogger::call, "Performs logging behaviour")
        .def("reset", &SingleObjectiveLogger::reset, "Reset the state of the logger")
        .def_property_readonly("problem", &SingleObjectiveLogger::problem, "Reference to the currently attached problem");


    py::class_<MultiObjectiveLogger, std::shared_ptr<MultiObjectiveLogger>>(m, "MultiLogger", "Base class for all loggers")
        .def("add_trigger", &MultiObjectiveLogger::trigger, "Add a trigger to the logger")
        .def("call", &MultiObjectiveLogger::call, "Performs logging behaviour")
        .def("reset", &MultiObjectiveLogger::reset, "Reset the state of the logger")
        .def_property_readonly("problem", &MultiObjectiveLogger::problem, "Reference to the currently attached problem");
    
    using namespace logger;
    using SingleObjectiveWatcher = Watcher<double>;
    using MultiObjectiveWatcher = Watcher<std::vector<double>>;
    py::class_<SingleObjectiveWatcher, AbstractWatcher<double>, Logger<double>, std::shared_ptr<SingleObjectiveWatcher>>(
        m, "AbstractLogger", "Base class for loggers which track properties")
        .def(py::init<Triggers, Properties>(), py::arg("triggers") = Triggers{}, py::arg("properties") = Properties{})
        .def("watch", &SingleObjectiveWatcher::watch)
        .def("attach_problem", &SingleObjectiveWatcher::attach_problem, py::arg("problem"), "attach a problem (MetaData) to a logger");
    
    py::class_<MultiObjectiveWatcher, AbstractWatcher<std::vector<double>>, Logger<std::vector<double>>, std::shared_ptr<MultiObjectiveWatcher>>(
        m, "AbstractMultiLogger", "Base class for loggers which track properties")
        .def(py::init<Triggers, Properties>(), py::arg("triggers") = Triggers{}, py::arg("properties") = Properties{})
        .def("watch", &MultiObjectiveWatcher::watch)
        .def("attach_problem", &MultiObjectiveWatcher::attach_problem, py::arg("problem"), "attach a problem (MetaData) to a logger");
}


void define_flatfile(py::module &m)
{
    using namespace logger;
    const std::vector<std::string> common_headers = {
        "suite_name", "problem_name", "problem_id", "problem_instance", "optimization_type", "dimension", "run"};
    
    using SingleObjectiveFlatFile = FlatFile<double>;


    py::class_<PyWatcher<SingleObjectiveFlatFile>, Watcher<double>, std::shared_ptr<PyWatcher<SingleObjectiveFlatFile>>>(m, "FlatFile")
        .def(py::init<Triggers, Properties, std::string, fs::path, std::string, std::string, std::string, std::string,
                      bool, bool, std::vector<std::string>>(),
             py::arg("triggers"), py::arg("properties"), py::arg("filename") = "IOH.dat",
             py::arg("output_directory") = "./", py::arg("separator") = "\t", py::arg("comment") = "#",
             py::arg("no_value") = "None", py::arg("end_of_line") = "\n", py::arg("repeat_header") = false,
             py::arg("store_positions") = false, py::arg("common_header_titles") = common_headers,
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
        .def_property_readonly("filename", &SingleObjectiveFlatFile::filename)
        .def_property_readonly(
            "output_directory",
            [](PyWatcher<SingleObjectiveFlatFile> &f) { return fs::absolute(f.output_directory()).generic_string(); })
        .def("watch", py::overload_cast<Property &>(&PyWatcher<SingleObjectiveFlatFile>::watch))
        .def("watch", py::overload_cast<const py::object &, const std::string &>(&PyWatcher<SingleObjectiveFlatFile>::watch))
        .def("watch",
             py::overload_cast<const py::object &, const std::vector<std::string> &>(&PyWatcher<SingleObjectiveFlatFile>::watch))
        .def("__repr__", [](const PyWatcher<SingleObjectiveFlatFile> &f) {
            return fmt::format("<FlatFile {}>", (f.output_directory() / f.filename()).generic_string());
        });
    
    py::class_<PyMultiWatcher<logger::FlatFile<std::vector<double>>>, logger::Watcher<std::vector<double>>, std::shared_ptr<PyMultiWatcher<logger::FlatFile<std::vector<double>>>>>(m, "MultiFlatFile")
        .def(py::init<Triggers, Properties, std::string, fs::path, std::string, std::string, std::string, std::string,
                      bool, bool, std::vector<std::string>>(),
             py::arg("triggers"), py::arg("properties"), py::arg("filename") = "IOH.dat",
             py::arg("output_directory") = "./", py::arg("separator") = "\t", py::arg("comment") = "#",
             py::arg("no_value") = "None", py::arg("end_of_line") = "\n", py::arg("repeat_header") = false,
             py::arg("store_positions") = false, py::arg("common_header_titles") = common_headers,
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
        .def_property_readonly("filename", &logger::FlatFile<std::vector<double>>::filename)
        .def_property_readonly(
            "output_directory",
            [](PyMultiWatcher<logger::FlatFile<std::vector<double>>> &f) { return fs::absolute(f.output_directory()).generic_string(); })
        .def("watch", py::overload_cast<Property &>(&PyMultiWatcher<logger::FlatFile<std::vector<double>>>::watch))
        .def("watch", py::overload_cast<const py::object &, const std::string &>(&PyMultiWatcher<logger::FlatFile<std::vector<double>>>::watch))
        .def("watch",
             py::overload_cast<const py::object &, const std::vector<std::string> &>(&PyMultiWatcher<logger::FlatFile<std::vector<double>>>::watch))
        .def("__repr__", [](const PyMultiWatcher<logger::FlatFile<std::vector<double>>> &f) {
            return fmt::format("<MultiFlatFile {}>", (f.output_directory() / f.filename()).generic_string());
        });
}

template <typename A>
void define_analyzer(py::module &m)
{
    using namespace logger;
    Triggers def_trigs{trigger::on_delta_improvement};
    Properties def_props{};
    using PyAnalyzer = PyAnalyzer<A>;
    py::class_<PyAnalyzer, Watcher<double>, std::shared_ptr<PyAnalyzer>>(m, "Analyzer")
        .def(py::init<Triggers, Properties, fs::path, std::string, std::string, std::string, bool>(),
             py::arg("triggers") = def_trigs, py::arg("additional_properties") = def_props,
             py::arg("root") = fs::current_path(), py::arg("folder_name") = "ioh_data",
             py::arg("algorithm_name") = "algorithm_name", py::arg("algorithm_info") = "algorithm_info",
             py::arg("store_positions") = false,
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

        .def("add_run_attribute", py::overload_cast<const std::string &, double>(&PyAnalyzer::add_run_attribute_python))
        .def("add_run_attribute",
             py::overload_cast<const py::object &, const std::string &>(&PyAnalyzer::add_run_attribute_python))
        .def("add_run_attributes",
             py::overload_cast<const py::object &, const std::vector<std::string> &>(
                 &PyAnalyzer::add_run_attributes_python))

        .def("set_run_attributes", &PyAnalyzer::set_run_attributes_python) // takes a map<str, double>
        .def("set_run_attribute", &PyAnalyzer::set_run_attribute_python) // takes str, double>
        .def_property_readonly("output_directory",
                               [](const PyAnalyzer &self) { return self.output_directory().generic_string(); })
        .def("close", &PyAnalyzer::close)
        .def("watch", py::overload_cast<Property &>(&PyAnalyzer::watch))
        .def("watch", py::overload_cast<const py::object &, const std::string &>(&PyAnalyzer::watch))
        .def("watch", py::overload_cast<const py::object &, const std::vector<std::string> &>(&PyAnalyzer::watch))
        .def("__repr__",
             [](const PyAnalyzer &f) { return fmt::format("<Analyzer {}>", f.output_directory().generic_string()); });
}


template <typename A>
void define_multianalyzer(py::module &m)
{
    using namespace logger;
    Triggers def_trigs{trigger::on_delta_improvement};
    Properties def_props{};
    using PyMultiAnalyzer = PyMultiAnalyzer<A>;
    py::class_<PyMultiAnalyzer, Watcher<std::vector<double>>, std::shared_ptr<PyMultiAnalyzer>>(m, "MultiAnalyzer")
        .def(py::init<Triggers, Properties, fs::path, std::string, std::string, std::string, bool>(),
             py::arg("triggers") = def_trigs, py::arg("additional_properties") = def_props,
             py::arg("root") = fs::current_path(), py::arg("folder_name") = "ioh_data",
             py::arg("algorithm_name") = "algorithm_name", py::arg("algorithm_info") = "algorithm_info",
             py::arg("store_positions") = false,
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
        .def("add_experiment_attribute", &PyMultiAnalyzer::add_experiment_attribute)
        .def("set_experiment_attributes", &PyMultiAnalyzer::set_experiment_attributes)

        .def("add_run_attribute", py::overload_cast<const std::string &, double>(&PyMultiAnalyzer::add_run_attribute_python))
        .def("add_run_attribute",
             py::overload_cast<const py::object &, const std::string &>(&PyMultiAnalyzer::add_run_attribute_python))
        .def("add_run_attributes",
             py::overload_cast<const py::object &, const std::vector<std::string> &>(
                 &PyMultiAnalyzer::add_run_attributes_python))

        .def("set_run_attributes", &PyMultiAnalyzer::set_run_attributes_python) // takes a map<str, double>
        .def("set_run_attribute", &PyMultiAnalyzer::set_run_attribute_python) // takes str, double>
        .def_property_readonly("output_directory",
                               [](const PyMultiAnalyzer &self) { return self.output_directory().generic_string(); })
        .def("close", &PyMultiAnalyzer::close)
        .def("watch", py::overload_cast<Property &>(&PyMultiAnalyzer::watch))
        .def("watch", py::overload_cast<const py::object &, const std::string &>(&PyMultiAnalyzer::watch))
        .def("watch", py::overload_cast<const py::object &, const std::vector<std::string> &>(&PyMultiAnalyzer::watch))
        .def("__repr__",
             [](const PyMultiAnalyzer &f) { return fmt::format("<MultiAnalyzer {}>", f.output_directory().generic_string()); });
}


void define_store(py::module &m)
{
    using namespace logger;

    using PyStore = PyWatcher<Store<double>>;
    py::class_<PyStore, Watcher<double>, std::shared_ptr<PyStore>>(m, "Store")
        .def(py::init<Triggers, Properties>(), py::arg("triggers"), py::arg("properties"),
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
        .def("data", py::overload_cast<>(&PyStore::data), "Accessor to the internal data container")
        .def(
            "at",
            [](PyStore &f, std::string suite_name, int pb, int dim, int inst, size_t run, size_t evaluation) {
                const auto cursor = Store<double>::Cursor(suite_name, pb, dim, inst, run, evaluation);
                return f.data(cursor);
            },
            "Accessor for a specific stored record")
        .def("watch", py::overload_cast<Property &>(&PyStore::watch), "Add a property")
        .def("watch", py::overload_cast<const py::object &, const std::string &>(&PyStore::watch), "Add a property")
        .def("watch", py::overload_cast<const py::object &, const std::vector<std::string> &>(&PyStore::watch),
             "Add multliple properties")
        .def("__repr__", [](PyStore &f) {
            return fmt::format("<Store (suites: ({}),)>", fmt::join(ioh::common::keys(f.data()), ","));
        });
}


void define_eah(py::module &m);
void define_eaf(py::module &m);
void define_properties(py::module &m);
void define_triggers(py::module &m);

void define_loggers(py::module &m)
{
    using namespace logger;
    using SingleObjectiveLogger = Logger<double>;
    py::class_<Combine, SingleObjectiveLogger, std::shared_ptr<Combine>>(m, "Combine", "Utility class to combine multiple loggers")
        .def(py::init<std::vector<std::reference_wrapper<SingleObjectiveLogger>>>(), py::arg("loggers"))
        .def(py::init<std::reference_wrapper<SingleObjectiveLogger>>(), py::arg("logger"))
        .def("append", &Combine::append);

    define_flatfile(m);
   
    define_store(m);
    define_analyzer<logger::analyzer::Analyzer>(m);
    define_multianalyzer<logger::analyzer::MultiAnalyzer>(m);

    define_eah(m);
    define_eaf(m);
}


void define_logger(py::module &m)
{
    py::class_<fs::path>(m, "Path").def(py::init<std::string>());
    py::implicitly_convertible<std::string, fs::path>();

    define_triggers(m);
    define_properties(m);
    define_bases(m);
    define_loggers(m);
}
