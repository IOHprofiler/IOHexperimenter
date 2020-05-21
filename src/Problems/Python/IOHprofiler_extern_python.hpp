
template<class InputType>
IOHprofiler_ExternPythonProblem<InputType>::IOHprofiler_ExternPythonProblem(
        std::string module_name,
        std::string instance_name,
        std::string objective_function_api,
        std::string instance_api_get,
        std::string dimension_api_get,
        std::string problem_id_api_get,
        std::string lowerbound_api,
        std::string upperbound_api,
        std::string minimization_api,
        std::string name_api,
        std::string type_api,
        std::string number_of_objectives_api
) :
    IOHprofiler_problem<InputType>(), // Default instance and dimension

    py(module_name, instance_name),

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

    _return_on_error(std::numeric_limits<InputType>::infinity())
{

    // Get and check the necessary interfaces.
    _py_objfunc          = py.get_callable(_api_objfunc);
    _py_instance_get     = py.get_callable(_api_instance_get);
    _py_dimension_get    = py.get_callable(_api_dimension_get);
    _py_problem_id_get   = py.get_callable(_api_problem_id_get);
    _py_lowerbound_get   = py.get_callable(_api_lowerbound_get);
    _py_upperbound_get   = py.get_callable(_api_upperbound_get);
    _py_minimization_get = py.get_callable(_api_minimization_get);
    _py_name_get         = py.get_callable(_api_name_get);
    _py_type_get         = py.get_callable(_api_type_get);
    _py_number_of_objectives_get
        = py.get_callable(_api_number_of_objectives_get);

    // Call the Python function to initialize the problem internals.
    this->IOHprofiler_set_problem_id(
            py.template call<int>(_py_problem_id_get) );

    this->IOHprofiler_set_instance_id(
            py.template call<int>(_py_instance_get) );

    this->IOHprofiler_set_number_of_variables(
            py.template call<int>(_py_dimension_get) );

    if(py.template call<bool>(_py_minimization_get)) {
        this->IOHprofiler_set_as_minimization();
    } else {
        this->IOHprofiler_set_as_maximization();
    }

    this->IOHprofiler_set_number_of_objectives(
            py.template call<int>(_py_number_of_objectives_get) );

    this->IOHprofiler_set_lowerbound(
            py.template call<std::vector<InputType>>(_py_lowerbound_get) );

    this->IOHprofiler_set_upperbound(
            py.template call<std::vector<InputType>>(_py_upperbound_get) );

    this->IOHprofiler_set_problem_name(
            py.template call<std::string>(_py_name_get) );

    this->IOHprofiler_set_problem_type(
            py.template call<std::string>(_py_type_get) );

}

template<class InputType>
double IOHprofiler_ExternPythonProblem<InputType>::internal_evaluate(const std::vector<InputType> &x)
{
    double fitness = py.template call<double>(_py_objfunc, x);
#ifndef NDEBUG
        std::ostringstream log_msg;
        log_msg << "Python objective function returned: " << fitness;
        IOH_log(log_msg.str());
#endif
    return fitness;
}

template<class InputType>
IOHprofiler_ExternPythonProblem<InputType>::~IOHprofiler_ExternPythonProblem()
{
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

