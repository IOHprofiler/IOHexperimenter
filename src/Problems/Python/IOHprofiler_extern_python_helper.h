/// \brief Classes for calling Python with templates.
///
/// ...
///
/// \author Johann Dreo

#ifndef _IOHPROFILER_EXTERN_PYTHON_HELPER_H
#define _IOHPROFILER_EXTERN_PYTHON_HELPER_H

#include <string>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

/** Helper class which call Python's API without having to bother with types.
 *
 * The public interface expose `get_callable` to get
 * a handle on an instance member function,
 * and `call` to actually call it with some arguments.
 *
 * The class details will take care of converting arguments and return types
 * from/to Python's data structures.
 */
template<class InputType>
class IOHprofiler_ExternPythonHelper
{
    protected:
        std::string _api_module;
        std::string _api_instance;

    public:
        PyObject* module;

        IOHprofiler_ExternPythonHelper(
                std::string module_name,
                std::string instance_name
        );

        ~IOHprofiler_ExternPythonHelper();

        PyObject* get_callable(std::string func_name);

        template<class R>
        R call(PyObject* py_function);

    protected:

        PyObject* pycall(PyObject* py_callable, PyObject* py_args);

        template<class T>
        PyObject* as_tuple( const T& x );

        template<class R>
        R as_num(PyObject* py_value);

        template<class R>
        R float_as_num(PyObject* py_value);

        template<class R>
        R long_as_num(PyObject* py_value);

        template<typename T>
        PyObject* from_num( T x );

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

        template<>
        void call(PyObject* py_function)
        {
            PyObject* py_args = PyTuple_New(0);
            pycall(py_function, py_args);
            Py_DECREF(py_args);
        }

        template<>
        std::vector<InputType> call(PyObject* py_function)
        {
            PyObject* py_args = PyTuple_New(0);
            PyObject* py_res = pycall(py_function, py_args);
            Py_DECREF(py_args);
            assert(PyList_Check(py_res));
            size_t res_len = PyList_Size(py_res);
            std::vector<InputType> vec;
            vec.reserve(res_len);
            for(size_t i=0; i < res_len; ++i) {
                PyObject* py_item = PyList_GetItem(py_res, i);
                InputType x = as_num<InputType>(py_item);
                vec.push_back(x);
            }
            Py_DECREF(py_res);
            return vec;
        }

        template<>
        std::string call(PyObject* py_function)
        {
            PyObject* py_args = PyTuple_New(0);
            PyObject* py_res = pycall(py_function, py_args);
            Py_DECREF(py_args);
            assert(PyUnicode_Check(py_res));
            std::string res(PyUnicode_AsUTF8(py_res));
            Py_DECREF(py_res);
            return res;
        }

        template<class R, class T>
        R call(PyObject* py_function, T args)
        {
            PyObject* py_args = as_tuple(args);
            PyObject* py_res = pycall(py_function, py_args);
            Py_DECREF(py_args);
            R res = as_num<R>(py_res);
            Py_DECREF(py_res);
            return res;
        }

        template<class T>
        void call(PyObject* py_function, T args)
        {
            PyObject* py_args = as_tuple(args);
            pycall(py_function, py_args);
            Py_DECREF(py_args);
        }

        //! @}

    protected:

        //! Template specializations of from_num for integer InputType(s). @{

        template<>
        PyObject* from_num( int x ) {
            return PyLong_FromLong( x );
        }

        template<>
        PyObject* from_num( long x ) {
            return PyLong_FromLong( x );
        }

        template<>
        PyObject* from_num( long long x ) {
            return PyLong_FromLongLong( x );
        }

        template<>
        PyObject* from_num( unsigned long x ) {
            return PyLong_FromUnsignedLong( x );
        }

        template<>
        PyObject* from_num( unsigned long long x ) {
            return PyLong_FromUnsignedLongLong( x );
        }

        //! @}


        //! Template specialization of as_num @{

        template<>
        int long_as_num(PyObject* py_value) {
            return static_cast<int>(PyLong_AsLong(py_value));
        }

        template<>
        double long_as_num(PyObject* py_value) {
            return static_cast<double>(PyLong_AsDouble(py_value));
        }

        template<>
        long long long_as_num(PyObject* py_value) {
            return static_cast<long long>(PyLong_AsLongLong(py_value));
        }

        template<>
        unsigned long long_as_num(PyObject* py_value) {
            return static_cast<unsigned long>(PyLong_AsUnsignedLong(py_value));
        }

        template<>
        unsigned long long long_as_num(PyObject* py_value) {
            return static_cast<unsigned long long>(PyLong_AsUnsignedLongLong(py_value));
        }

        //! @}
};

#include "IOHprofiler_extern_python_helper.hpp"

#endif // _IOHPROFILER_EXTERN_PYTHON_HELPER_H

