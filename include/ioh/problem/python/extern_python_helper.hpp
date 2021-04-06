#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "ioh/common.hpp"


namespace ioh {
    namespace problem {
        namespace python {
            template <class InputType>
            class ExternPythonHelper {
            protected:
                std::string _api_module;
                std::string _api_instance;

            public:
                PyObject *module;

                ExternPythonHelper(
                    const std::string& module_name,
                    const std::string& instance_name
                    )
                    : _api_module(module_name),
                      _api_instance(instance_name) {

                    Py_Initialize();
                    PyObject *py_name = PyUnicode_DecodeFSDefault(
                        _api_module.c_str());
                    module = PyImport_Import(py_name);
                    Py_DECREF(py_name);

                    if (!module) {
                        PyErr_Print();
                        std::ostringstream msg;
                        msg << "Failed to import the Python module `" <<
                            _api_module << "`";
                        common::log::error(msg.str());
                        return;
                    }
#ifndef NDEBUG
                    std::ostringstream log_msg;
                    log_msg << "Python module `" << _api_module <<
                        "` loaded succesfully";
                    common::log::info(log_msg.str());
#endif
                }

                ~ExternPythonHelper() {
                    Py_DECREF(module);
                }

                PyObject *get_callable(std::string func_name) {
                    PyObject *py_inst = PyObject_GetAttrString(
                        module, _api_instance.c_str());

                    if (!py_inst) {
                        if (PyErr_Occurred()) {
                            PyErr_Print();
                        }
                        Py_XDECREF(py_inst);
                        std::ostringstream err_msg;
                        err_msg << "Python problem instance not found: `"
                            << _api_instance << "`";
                        common::log::error(err_msg.str());
                        // FIXME Maybe a local exception would be better.
                        return nullptr;
                        // Useless if IOH_error raise an exception.
                    }

                    PyObject *py_func = PyObject_GetAttrString(
                        py_inst, func_name.c_str());

                    if (!py_func) {
                        if (PyErr_Occurred()) {
                            PyErr_Print();
                        }
                        Py_XDECREF(py_inst);
                        Py_XDECREF(py_func);
                        std::ostringstream err_msg;
                        err_msg << "Python method not found: `"
                            << _api_instance << "." << func_name << "`";
                        common::log::error(err_msg.str());
                        // FIXME Maybe a local exception would be better.
                        return nullptr;
                        // Useless if IOH_error raise an exception.
                    }

                    if (!PyCallable_Check(py_func)) {
                        if (PyErr_Occurred()) {
                            PyErr_Print();
                        }
                        Py_XDECREF(py_inst);
                        Py_XDECREF(py_func);
                        std::ostringstream err_msg;
                        err_msg << "Python method not callable: `"
                            << _api_instance << "." << func_name << "`";
                        common::log::error(err_msg.str());
                        // FIXME Maybe a local exception would be better.
                        return nullptr;
                        // Useless if IOH_error raise an exception.
                    }

#ifndef NDEBUG
                    if (py_func && PyCallable_Check(py_func)) {
                        std::ostringstream log_msg;
                        log_msg << "Python object is callable: `"
                            << _api_instance << "." << func_name << "`";
                        common::log::info(log_msg.str());
                    }
#endif

                    Py_XDECREF(py_inst);
                    return py_func;
                }

                template <class R>
                R call(PyObject *py_function) {
                    PyObject *py_args = PyTuple_New(0);
                    PyObject *py_res = pycall(py_function, py_args);
                    Py_DECREF(py_args);
                    R res = as_num<R>(py_res);
                    Py_DECREF(py_res);
                    return res;
                }

            protected:

                PyObject *pycall(PyObject *py_callable, PyObject *py_args) {
                    PyObject *py_result = PyObject_CallObject(
                        py_callable, py_args);
                    Py_DECREF(py_args);
                    if (!py_result) {
                        PyErr_Print();
                        std::ostringstream msg;
                        msg << "Call to Python callable failed";
                        common::log::error(msg.str());
                        return nullptr;
                    }
                    return py_result;
                }

                template <class T>
                PyObject *as_tuple(const T &x) {
                    PyObject *py_args = PyTuple_New(x.size());
                    for (size_t i = 0; i < x.size(); ++i) {
                        PyObject *py_elem = this->from_num(x[i]);
                        if (!py_elem) {
                            Py_DECREF(py_args);
                            std::ostringstream msg;
                            msg <<
                                "Cannot convert argument's element to a Python type `"
                                << x[i] <<
                                "`";
                            common::log::error(msg.str());
                            return nullptr;
                        }
                        PyTuple_SetItem(py_args, i, py_elem);
                    }
                    return py_args;
                }


                template <class R>
                R as_num(PyObject *py_value) {
                    R result;
                    if (PyLong_Check(py_value)) {
                        result = long_as_num<R>(py_value);
                    } else if (PyFloat_Check(py_value)) {
                        result = float_as_num<R>(py_value);
                    } else {
                        std::ostringstream msg;
                        msg << "Cannot convert this Python object type";
                        common::log::error(msg.str());
                        return std::numeric_limits<R>::max();
                    }
                    if (PyErr_Occurred()) {
                        PyErr_Print();
                        std::ostringstream msg;
                        msg << "Python numeric conversion failed";
                        common::log::error(msg.str());
                        return std::numeric_limits<R>::max();
                    }
                    return result;
                }

                template <class R>
                R float_as_num(PyObject *py_value) {
                    return static_cast<R>(PyFloat_AsDouble(py_value));
                }

                template <class R>
                R long_as_num(PyObject *py_value) {
                    return static_cast<R>(PyLong_AsLong(py_value));
                }

                template <typename T>
                PyObject *from_num(T x);

                /***************************************************************
                 * Below are implementation of templates specializations.
                 *
                 * Because they are difficult to forward-declare
                 * without dummy helper classes,
                 * the implementation are not in the `*.hpp`.
                 *
                 * You should not need to read them
                 * if you just look for the interface.
                 **************************************************************/

            public:
                //! Template specializations of call @{

                template <>
                void call(PyObject *py_function) {
                    PyObject *py_args = PyTuple_New(0);
                    pycall(py_function, py_args);
                    Py_DECREF(py_args);
                }

                template <>
                std::vector<InputType> call(PyObject *py_function) {
                    PyObject *py_args = PyTuple_New(0);
                    PyObject *py_res = pycall(py_function, py_args);
                    Py_DECREF(py_args);
                    assert(PyList_Check(py_res));
                    size_t res_len = PyList_Size(py_res);
                    std::vector<InputType> vec;
                    vec.reserve(res_len);
                    for (size_t i = 0; i < res_len; ++i) {
                        PyObject *py_item = PyList_GetItem(py_res, i);
                        InputType x = as_num<InputType>(py_item);
                        vec.push_back(x);
                    }
                    Py_DECREF(py_res);
                    return vec;
                }

                template <>
                std::string call(PyObject *py_function) {
                    PyObject *py_args = PyTuple_New(0);
                    PyObject *py_res = pycall(py_function, py_args);
                    Py_DECREF(py_args);
                    assert(PyUnicode_Check(py_res));
                    std::string res(PyUnicode_AsUTF8(py_res));
                    Py_DECREF(py_res);
                    return res;
                }

                template <class R, class T>
                R call(PyObject *py_function, T args) {
                    PyObject *py_args = as_tuple(args);
                    PyObject *py_res = pycall(py_function, py_args);
                    Py_DECREF(py_args);
                    R res = as_num<R>(py_res);
                    Py_DECREF(py_res);
                    return res;
                }

                template <class T>
                void call(PyObject *py_function, T args) {
                    PyObject *py_args = as_tuple(args);
                    pycall(py_function, py_args);
                    Py_DECREF(py_args);
                }

                //! @}

            protected:

                //! Template specializations of from_num for integer InputType(s). @{

                template <>
                PyObject *from_num(int x) {
                    return PyLong_FromLong(x);
                }

                template <>
                PyObject *from_num(long x) {
                    return PyLong_FromLong(x);
                }

                template <>
                PyObject *from_num(long long x) {
                    return PyLong_FromLongLong(x);
                }

                template <>
                PyObject *from_num(unsigned long x) {
                    return PyLong_FromUnsignedLong(x);
                }

                template <>
                PyObject *from_num(unsigned long long x) {
                    return PyLong_FromUnsignedLongLong(x);
                }

                //! @}


                //! Template specialization of as_num @{

                template <>
                int long_as_num(PyObject *py_value) {
                    return static_cast<int>(PyLong_AsLong(py_value));
                }

                template <>
                double long_as_num(PyObject *py_value) {
                    return static_cast<double>(PyLong_AsDouble(py_value));
                }

                template <>
                long long long_as_num(PyObject *py_value) {
                    return static_cast<long long>(PyLong_AsLongLong(py_value));
                }

                template <>
                unsigned long long_as_num(PyObject *py_value) {
                    return static_cast<unsigned long>(PyLong_AsUnsignedLong(
                        py_value));
                }

                template <>
                unsigned long long long_as_num(PyObject *py_value) {
                    return static_cast<unsigned long long>(
                        PyLong_AsUnsignedLongLong(py_value));
                }
            };
        }
    }
}
