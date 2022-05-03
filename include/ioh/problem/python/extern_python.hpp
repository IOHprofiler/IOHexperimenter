#pragma once

#include "extern_python_helper.hpp"
#include <ioh/common/log.hpp>
#include <ioh/problem.hpp>

namespace ioh {
    namespace problem {
        namespace python {
            template <class InputType>
            class ExternPythonProblem : public base<InputType> {
            public:
                /** The only mandatory arguments are the module and the instance.
                 *
                 * All other arguments defines the API which will be used
                 * to reach the Python code.
                 *
                 * This constructor initialize the Python API
                 * and check if all the necessary member functions
                 * can actually be called.
                 */
                ExternPythonProblem(
                    const std::string& module_name,
                    const std::string& instance_name,
                    const std::string& objective_function_api = "call",
                    const std::string& instance_api_get = "get_instance",
                    const std::string& dimension_api_get = "get_number_of_variables",
                    const std::string& problem_id_api_get = "get_problem_id",
                    const std::string& lowerbound_api = "get_lowerbound",
                    const std::string& upperbound_api = "get_upperbound",
                    const std::string& minimization_api = "get_minimization",
                    const std::string& name_api = "get_name",
                    const std::string& type_api = "get_type",
                    const std::string& number_of_objectives_api =
                        "get_number_of_objectives"
                    )
                    : base<InputType>(), // Default instance and dimension

                      _api_objfunc(objective_function_api),

                      _py_objfunc(nullptr),
                      _api_instance_get(instance_api_get),

                      _py_instance_get(nullptr),
                      _api_dimension_get(dimension_api_get),

                      _py_dimension_get(nullptr),
                      _api_problem_id_get(problem_id_api_get),

                      _py_problem_id_get(nullptr),
                      _api_lowerbound_get(lowerbound_api),

                      _py_lowerbound_get(nullptr),
                      _api_upperbound_get(upperbound_api),

                      _py_upperbound_get(nullptr),
                      _api_minimization_get(minimization_api),

                      _py_minimization_get(nullptr),
                      _api_name_get(name_api),

                      _py_name_get(nullptr),
                      _api_type_get(type_api),

                      _py_type_get(nullptr),
                      _api_number_of_objectives_get(number_of_objectives_api),

                      _py_number_of_objectives_get(nullptr),
                      _return_on_error(
                          std::numeric_limits<InputType>::infinity()),

                      py(module_name, instance_name) {
                    // Get and check the necessary interfaces.
                    _py_objfunc = py.get_callable(_api_objfunc);
                    _py_instance_get = py.get_callable(_api_instance_get);
                    _py_dimension_get = py.get_callable(_api_dimension_get);
                    _py_problem_id_get = py.get_callable(_api_problem_id_get);
                    _py_lowerbound_get = py.get_callable(_api_lowerbound_get);
                    _py_upperbound_get = py.get_callable(_api_upperbound_get);
                    _py_minimization_get = py.get_callable(
                        _api_minimization_get);
                    _py_name_get = py.get_callable(_api_name_get);
                    _py_type_get = py.get_callable(_api_type_get);
                    _py_number_of_objectives_get
                        = py.get_callable(_api_number_of_objectives_get);

                    // Call the Python function to initialize the problem internals.
                    this->set_problem_id(
                        py.template call<int>(_py_problem_id_get));

                    this->set_instance_id(
                        py.template call<int>(_py_instance_get));

                    this->set_number_of_variables(
                        py.template call<int>(_py_dimension_get));

                    if (py.template call<bool>(_py_minimization_get)) {
                        this->set_as_minimization();
                    } else {
                        this->set_as_maximization();
                    }

                    this->set_number_of_objectives(
                        py.template call<int>(_py_number_of_objectives_get));

                    this->set_lowerbound(
                        py.template call<std::vector<InputType>>(
                            _py_lowerbound_get));

                    this->set_upperbound(
                        py.template call<std::vector<InputType>>(
                            _py_upperbound_get));

                    this->set_problem_name(
                        py.template call<std::string>(_py_name_get));

                    this->set_problem_type(
                        py.template call<std::string>(_py_type_get));

                    std::cout << "Doest this work?: ";
                    std::cout << internal_evaluate(std::vector<int>(4, 6)) <<
                        std::endl;

                }

                // TODO Unused API:
                // virtual void prepare_problem() { }
                // virtual void customize_optimal() { }

                /** Actually call the objective function. */
                double internal_evaluate(
                    const std::vector<InputType> &x) override {
                    double fitness = py.template call<double>(_py_objfunc, x);
#ifndef NDEBUG
                    std::ostringstream log_msg;
                    log_msg << "Python objective function returned: " <<
                        fitness;
                    common::log::info(log_msg.str());
#endif
                    return fitness;
                }

                /** Deallocate Python variables. */
                virtual ~ExternPythonProblem() {
                    Py_DECREF(_py_objfunc);
                    Py_DECREF(_py_objfunc);
                    Py_DECREF(_py_instance_get);
                    Py_DECREF(_py_dimension_get);
                    Py_DECREF(_py_problem_id_get);
                    Py_DECREF(_py_lowerbound_get);
                    Py_DECREF(_py_upperbound_get);
                    Py_DECREF(_py_minimization_get);
                    Py_DECREF(_py_name_get);
                    Py_DECREF(_py_type_get);
                    Py_DECREF(_py_number_of_objectives_get);
                }


            protected:
                // This is a lot of boiler plate, but it's better to be eplicit.

                std::string _api_objfunc;
                PyObject *_py_objfunc;

                std::string _api_instance_get;
                PyObject *_py_instance_get;

                std::string _api_dimension_get;
                PyObject *_py_dimension_get;

                std::string _api_problem_id_get;
                PyObject *_py_problem_id_get;

                std::string _api_lowerbound_get;
                PyObject *_py_lowerbound_get;

                std::string _api_upperbound_get;
                PyObject *_py_upperbound_get;

                std::string _api_minimization_get;
                PyObject *_py_minimization_get;

                std::string _api_name_get;
                PyObject *_py_name_get;

                std::string _api_type_get;
                PyObject *_py_type_get;

                std::string _api_number_of_objectives_get;
                PyObject *_py_number_of_objectives_get;

                const InputType _return_on_error;

                ExternPythonHelper<InputType> py;
            };
        }
    }
}
