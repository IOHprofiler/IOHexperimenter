#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "ioh.hpp"

namespace py = pybind11;
using namespace ioh;


template <typename LoggerType>
class PyLogger : public LoggerType
{
public:
    using LoggerType::LoggerType;

    void watch(logger::Property &property) override
    {
        std::cout << "cannot call this yet" << property.name() << std::endl;
    }

    void log(const logger::Info &log_info) override { PYBIND11_OVERRIDE(void, LoggerType, log, log_info); }

    void attach_problem(const problem::MetaData &problem) override
    {
        PYBIND11_OVERRIDE(void, LoggerType, attach_problem, problem);
    }

    void attach_suite(const std::string &suite_name) override
    {
        PYBIND11_OVERRIDE(void, LoggerType, attach_suite, suite_name);
    }

    void call(const logger::Info &log_info) override { PYBIND11_OVERRIDE(void, LoggerType, call, log_info); }
};

void define_triggers(py::module &m)
{
    py::module t = m.def_submodule("trigger");

    py::class_<logger::Trigger, std::shared_ptr<logger::Trigger>>(t, "Trigger")
        .def("__call__", &logger::Trigger::operator())
        .def("reset", &logger::Trigger::reset);

    py::class_<trigger::Always, logger::Trigger, std::shared_ptr<trigger::Always>>(t, "Always");
    t.attr("ALWAYS") = py::cast(trigger::always);

    py::class_<trigger::OnImprovement, logger::Trigger, std::shared_ptr<trigger::OnImprovement>>(t, "OnImprovement");
    t.attr("ON_IMPROVEMENT") = py::cast(trigger::on_improvement);

    py::class_<trigger::At, logger::Trigger, std::shared_ptr<trigger::At>>(t, "At");
    py::class_<trigger::Each, logger::Trigger, std::shared_ptr<trigger::Each>>(t, "Each");
    py::class_<trigger::During, logger::Trigger, std::shared_ptr<trigger::During>>(t, "During");
}

void define_properties(py::module &m)
{
    py::module t = m.def_submodule("property");

    py::class_<logger::Property, std::shared_ptr<logger::Property>>(t, "Property")

        .def("__call__", &logger::Property::operator())
        .def("name", &logger::Property::name)
        .def("call_to_string", &logger::Property::call_to_string);

    py::class_<watch::Evaluations, logger::Property, std::shared_ptr<watch::Evaluations>>(t, "Evaluations")
        .def(py::init<std::string, std::string>());
    t.attr("EVALUATIONS") = py::cast(watch::evaluations);

    py::class_<watch::RawYBest, logger::Property, std::shared_ptr<watch::RawYBest>>(t, "RawYBest")
        .def(py::init<std::string, std::string>());
    t.attr("RAW_Y_BEST") = py::cast(watch::raw_y_best);

    py::class_<watch::CurrentY, logger::Property, std::shared_ptr<watch::CurrentY>>(t, "CurrentY")
        .def(py::init<std::string, std::string>());
    t.attr("CURRENT_Y_BEST") = py::cast(watch::current_y);

    py::class_<watch::TransformedY, logger::Property, std::shared_ptr<watch::TransformedY>>(t, "TransformedY")
        .def(py::init<std::string, std::string>());
    t.attr("TRANSFORMED_Y") = py::cast(watch::transformed_y);

    py::class_<watch::TransformedYBest, logger::Property, std::shared_ptr<watch::TransformedYBest>>(t,
                                                                                                    "TransformedYBest")
        .def(py::init<std::string, std::string>());
    t.attr("TRANSFORMED_Y_BEST") = py::cast(watch::transformed_y_best);
}

void define_bases(py::module &m)
{
    py::class_<Logger, std::shared_ptr<Logger>>(m, "Logger")
        .def("add_trigger", &Logger::trigger)
        .def("log", &Logger::log)
        .def("attach_problem", &Logger::attach_problem)
        .def("attach_suite", &Logger::attach_suite)
        .def("call", &Logger::call)
        .def("reset", &Logger::reset)
        .def_property_readonly("problem", &Logger::problem);

    using namespace logger;

    py::class_<Watcher, Logger, std::shared_ptr<Watcher>>(m, "Watcher").def("watch", &Watcher::watch);
}

void define_loggers(py::module &m)
{
    using namespace logger;
    using Trigs = std::vector<std::reference_wrapper<Trigger>>;
    using Props = std::vector<std::reference_wrapper<Property>>;

    py::class_<Combine, Logger, std::shared_ptr<Combine>>(m, "Combine")
        .def(py::init<std::vector<std::reference_wrapper<Logger>>>(), py::arg("loggers"))
        .def(py::init<std::reference_wrapper<Logger>>(), py::arg("logger"))
        .def("append", &Combine::append);

    const std::vector<std::string> common_headers = {
        "suite_name", "problem_name", "problem_id", "problem_instance", "optimization_type", "dimension", "run"};
    
    py::class_<PyLogger<FlatFile>, Watcher, std::shared_ptr<PyLogger<FlatFile>>>(m, "FlatFile")
        .def(py::init<Trigs, Props, std::string, fs::path, std::string, std::string, std::string, std::string, bool,
                      bool, std::vector<std::string>>(),
             py::arg("triggers"), py::arg("properties"), py::arg("filename") = "IOH.dat",
             py::arg("output_directory") = "./", py::arg("separator") = "\t", py::arg("comment") = "#",
             py::arg("no_value") = "None", py::arg("end_of_line") = "\n", py::arg("repeat_header") = false,
             py::arg("store_positions") = false, py::arg("common_header_titles") = common_headers)
        .def_property_readonly("filename", &FlatFile::filename)
        .def_property_readonly(
            "output_directory",
            [](PyLogger<FlatFile> &f) { return std::filesystem::absolute(f.output_directory()).generic_string(); })
        .def("__repr__", [](const PyLogger<FlatFile> &f) {
            return fmt::format("<FlatFile {}>", (f.output_directory() / f.filename()).generic_string());
        });


    py::class_<Store::Cursor>(m, "Cursor").def(py::init<std::string, int, int, int, size_t, size_t>());

    using PyStore = PyLogger<Store>;

    py::class_<PyStore, Watcher, std::shared_ptr<PyStore>>(m, "Store")
        .def(py::init<Trigs, Props>())
        .def("data", py::overload_cast<>(&PyStore::data))
        .def("at", [](PyStore &f, std::string suite_name, int pb, int dim, int inst, size_t run, size_t evaluation) { 
                    const auto cursor = Store::Cursor(suite_name, pb, dim, inst, run, evaluation); 
                    return f.data(cursor);
                })
        .def("__repr__", [](PyStore &f) {
            return fmt::format("<Store (suites: ({}),)>", fmt::join(ioh::common::keys(f.data()), ","));
        });


    using PyAnalyzer = PyLogger<Analyzer>;
    Trigs def_trigs{trigger::on_improvement};
    Props def_props{};

    py::class_<PyAnalyzer, Watcher, std::shared_ptr<PyAnalyzer>>(m, "Analyzer")
          .def(
              py::init<Trigs, Props, fs::path, std::string, std::string, std::string, bool>(),
              py::arg("triggers") = def_trigs, 
              py::arg("additional_properties") = def_props, 
              py::arg("root") = fs::current_path(),
              py::arg("folder_name") = "ioh_data", 
              py::arg("algorithm_name") = "algorithm_name", 
              py::arg("algorithm_info") = "algorithm_info",
              py::arg("store_positions") = true
          )
          .def("add_experiment_attribute", &PyAnalyzer::add_experiment_attribute)
          .def("set_experiment_attributes", &PyAnalyzer::set_experiment_attributes)
          .def("add_run_attribute", &PyAnalyzer::add_run_attribute)
          .def("set_run_attributes", &PyAnalyzer::set_run_attributes)
          .def("set_run_attribute", &PyAnalyzer::set_run_attribute)
          .def_property_readonly("output_directory", &PyAnalyzer::output_directory)
          .def("__repr__", [](const PyAnalyzer &f) {return fmt::format("<Analyzer {}>", f.output_directory().generic_string());})
        ;

} 

void define_logger(py::module &m)
{
    py::class_<fs::path>(m, "Path").def(py::init<std::string>());
    py::implicitly_convertible<std::string, fs::path>();

    define_triggers(m);
    define_properties(m);
    define_bases(m);
    define_loggers(m);


    // py::class_<PyLogger, Base, std::shared_ptr<PyLogger>>(m, "Default")
    //     .def(py::init<fs::path, std::string, std::string, std::string, ioh::common::OptimizationType, bool, bool,
    //     int,
    //                   int, bool, std::vector<int>, int, int>(),
    //          py::arg("output_directory") = "./", py::arg("folder_name") = "ioh_data",
    //          py::arg("algorithm_name") = "algorithm_name", py::arg("algorithm_info") = "algorithm_info",
    //          py::arg("optimization_type") = ioh::common::OptimizationType::Minimization,
    //          py::arg("store_positions") = false, py::arg("t_always") = false, py::arg("t_on_interval") = 0,
    //          py::arg("t_per_time_range") = 0, py::arg("t_on_improvement") = true,
    //          py::arg("t_at_time_points") = std::vector<int>{0}, py::arg("trigger_at_time_points_exp_base") = 10,
    //          py::arg("trigger_at_range_exp_base") = 10)
    //     .def("declare_experiment_attributes", &PyLogger::declare_experiment_attributes)
    //     .def("declare_run_attributes", &PyLogger::declare_run_attributes)
    //     .def("declare_logged_attributes", &PyLogger::declare_logged_attributes)
    //     .def("__enter__", [=](const PyLogger &self) {return &self;})
    //     .def("__exit__", [=](PyLogger& self, const py::object &, const py::object &, const py::object &){
    //         self.flush();
    //     })
    //     .def("__repr__", [=](PyLogger &p){
    //         return "<DefaultLogger: '" + p.experiment_folder().path().generic_string() + "'>";
    //     })
    //     ;
    // py::class_<ECDF, Base, std::shared_ptr<ECDF>>(m, "ECDF")
    //     .def(py::init<double, double, size_t, size_t, size_t,  size_t>())
    //     .def("data", &ECDF::data)
    //     .def("at", &ECDF::at)
    //     .def("error_range", &ECDF::error_range)
    //     .def("eval_range", &ECDF::eval_range)
    //     ;
}


// class PyLogger final: public Analyzer
// {
//     py::object attribute_container_{};
//     std::vector<std::string> logged_attribute_names_{};
//     std::vector<std::string> run_attribute_names_{};

//     [[nodiscard]]
//     std::vector<double> get_attributes(const std::vector<std::string>& names) const
//     {
//         std::vector<double> attributes;
//         for (const auto &n : names)
//             attributes.emplace_back(
//                 PyFloat_AsDouble(attribute_container_.attr(n.c_str()).ptr())
//             );
//         return attributes;
//     }

// public:
//     using Analyzer::Analyzer;

//     void declare_logged_attributes(py::object &obj, const std::vector<std::string> &names)
//     {
//         attribute_container_ = obj;
//         logged_attribute_names_ = names;
//         create_logged_attributes(logged_attribute_names_);
//     }

//     void declare_run_attributes(py::object &obj, const std::vector<std::string> &names)
//     {
//         attribute_container_ = obj;
//         run_attribute_names_ = names;
//         create_run_attributes(run_attribute_names_);
//     }

//     void declare_experiment_attributes(const std::vector<std::string> &names, const std::vector<double> &values)
//     {
//         for (size_t i = 0; i < names.size(); i++)
//             add_experiment_attribute(names.at(i), values.at(i));
//     }

//     void track_problem(const ioh::problem::MetaData &problem) override
//     {
//         set_run_attributes(run_attribute_names_, get_attributes(run_attribute_names_));
//         Default::track_problem(problem);
//     }

//     void log(const LogInfo& log_info) override
//     {
//         set_logged_attributes(logged_attribute_names_, get_attributes(logged_attribute_names_));
//         Default::log(log_info);
//     }
// };