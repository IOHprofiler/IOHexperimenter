
template<class InputType>
IOHprofiler_ExternPythonHelper<InputType>::IOHprofiler_ExternPythonHelper(
        std::string module_name,
        std::string instance_name
        )
    :
        _api_module(module_name),
        _api_instance(instance_name)
{
    Py_Initialize();
    PyObject* py_name = PyUnicode_DecodeFSDefault(_api_module.c_str());
    module = PyImport_Import(py_name);
    Py_DECREF(py_name);

    if(not module) {
        PyErr_Print();
        std::ostringstream msg;
        msg << "Failed to import the Python module `" << _api_module << "`";
        IOH_error(msg.str());
        return;

    } else {
#ifndef NDEBUG
        std::ostringstream log_msg;
        log_msg << "Python module `" << _api_module << "` loaded succesfully";
        IOH_log(log_msg.str());
#endif
    }
}

template<class InputType>
IOHprofiler_ExternPythonHelper<InputType>::~IOHprofiler_ExternPythonHelper()
{
    Py_DECREF(module);
}

template<class InputType>
PyObject* IOHprofiler_ExternPythonHelper<InputType>::pycall(PyObject* py_callable, PyObject* py_args)
{
    PyObject* py_result = PyObject_CallObject(py_callable, py_args);
    Py_DECREF(py_args);
    if(not py_result) {
        PyErr_Print();
        std::ostringstream msg;
        msg << "Call to Python callable failed";
        IOH_error(msg.str());
        return nullptr;
    }
    return py_result;
}

template<class InputType>
PyObject* IOHprofiler_ExternPythonHelper<InputType>::get_callable(std::string func_name)
{
    PyObject* py_inst = PyObject_GetAttrString(module, _api_instance.c_str());

    if(not py_inst) {
        if(PyErr_Occurred()) {
            PyErr_Print();
        }
        Py_XDECREF(py_inst);
        std::ostringstream err_msg;
        err_msg << "Python problem instance not found: `"
            << _api_instance << "`";
        IOH_error(err_msg.str()); // FIXME Maybe a local exception would be better.
        return nullptr; // Useless if IOH_error raise an exception.
    }

    PyObject* py_func = PyObject_GetAttrString(py_inst, func_name.c_str());

    if(not py_func) {
        if(PyErr_Occurred()) {
            PyErr_Print();
        }
        Py_XDECREF(py_inst);
        Py_XDECREF(py_func);
        std::ostringstream err_msg;
        err_msg << "Python method not found: `"
            << _api_instance << "." << func_name << "`";
        IOH_error(err_msg.str()); // FIXME Maybe a local exception would be better.
        return nullptr; // Useless if IOH_error raise an exception.
    }

    if(not PyCallable_Check(py_func)) {
        if(PyErr_Occurred()) {
            PyErr_Print();
        }
        Py_XDECREF(py_inst);
        Py_XDECREF(py_func);
        std::ostringstream err_msg;
        err_msg << "Python method not callable: `"
            << _api_instance << "." << func_name << "`";
        IOH_error(err_msg.str()); // FIXME Maybe a local exception would be better.
        return nullptr; // Useless if IOH_error raise an exception.
    }

#ifndef NDEBUG
    if(py_func and PyCallable_Check(py_func)) {
        std::ostringstream log_msg;
        log_msg << "Python object is callable: `"
                << _api_instance << "." << func_name << "`";
            ;
        IOH_log(log_msg.str());
    }
#endif

    Py_XDECREF(py_inst);
    return py_func;
}

template<class InputType>
template<class T>
PyObject* IOHprofiler_ExternPythonHelper<InputType>::as_tuple( const T& x )
{
    PyObject* py_args = PyTuple_New( x.size() );
    for(size_t i = 0; i < x.size(); ++i) {
        PyObject* py_elem = this->from_num(x[i]);
        if(not py_elem) {
            Py_DECREF(py_args);
            std::ostringstream msg;
            msg << "Cannot convert argument's element to a Python type `" << x[i] << "`";
            IOH_error(msg.str());
            return nullptr;
        }
        PyTuple_SetItem(py_args, i, py_elem);
    }
    return py_args;
}

template<class InputType>
template<class R>
R IOHprofiler_ExternPythonHelper<InputType>::float_as_num(PyObject* py_value)
{
    return static_cast<R>(PyFloat_AsDouble(py_value));
}

template<class InputType>
template<class R>
R IOHprofiler_ExternPythonHelper<InputType>::long_as_num(PyObject* py_value)
{
    return static_cast<R>(PyLong_AsLong(py_value));
}

template<class InputType>
template<typename T>
PyObject* IOHprofiler_ExternPythonHelper<InputType>::from_num( T x )
{
    return PyFloat_FromDouble( x );
        }

template<class InputType>
template<class R>
R IOHprofiler_ExternPythonHelper<InputType>::as_num(PyObject* py_value)
{
    R result;
    if(PyLong_Check(py_value)) {
        result = long_as_num<R>(py_value);
    } else
    if(PyFloat_Check(py_value)) {
        result = float_as_num<R>(py_value);
    } else {
        std::ostringstream msg;
        msg << "Cannot convert this Python object type";
        IOH_error(msg.str());
        return std::numeric_limits<R>::max();
    }
    if(PyErr_Occurred()) {
        PyErr_Print();
        std::ostringstream msg;
        msg << "Python numeric conversion failed";
        IOH_error(msg.str());
        return std::numeric_limits<R>::max();
    }
    return result;
}

template<class InputType>
template<class R>
R IOHprofiler_ExternPythonHelper<InputType>::call(PyObject* py_function)
{
    PyObject* py_args = PyTuple_New(0);
    PyObject* py_res = pycall(py_function, py_args);
    Py_DECREF(py_args);
    R res = as_num<R>(py_res);
    Py_DECREF(py_res);
    return res;
}

