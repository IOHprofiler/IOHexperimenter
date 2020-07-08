/// \brief Classes for calling problems written in Python.
///
/// ...
///
/// \author Johann Dreo

#ifndef _IOHPROFILER_EXTERN_PYTHON_H
#define _IOHPROFILER_EXTERN_PYTHON_H

#include <sstream>

#include "IOHprofiler_problem.h"
#include "IOHprofiler_extern_python_helper.h"

/** A proxy for problems written in the Python programming language.
 *
 * Given a Python module in which exists an instance of a class
 * honoring the needed interface, this class be be use as a proxy
 * that will call the Python code of a problem.
 *
 * @code
    IOHprofiler_ExternPythonProblem<int> pypb("mymodule", "myinstance");
    size_t d = pypb.IOHprofiler_get_number_of_variables();
    std::vector<int> sol(d,6);
    double fit = pypb.evaluate(sol);
    @endcode
 *
 * @note Given that this class operates on a class instance
 * a given Python module may host several problems.
 * However, you still need to create one C++ class instance
 * per Python class instance.
 *
 * The interface is formed by several member functions,
 * which should be implemented by the targeted Python class.
 *
 * An abstract base class enforcing the default interface
 * is distributed with IOH in the `ioh.py` file.
 * Users may inherit from this class to ensure that they
 * correctly implemented the interface, albeit
 * this is not mandatory (given that Python does not
 * enforce type checking).
 *
 * Default names of the members can be changed
 * by modifying the values of the `*api*` parameters
 * passed to this class' constructor.
 *
 * All the `get_*` member functions of the Python interface
 * just return a parameter.
 * Only the `call` function takes arguments
 * (i.e. a candidate solution to the problem).
 * This class send a call tuple to the Python function,
 * so that you can either define a fixed number of
 * explicit variables, either capture the tuple
 * as a positional argument expansion:
 * @code
    # Example for variable dimensions:
    def call(self, *args ):
        return sum(*args)

    # Example for fixed dimensions:
    def call(self, x0, x1, x2):
        return x0+x1+x2
 * @endcode
 */
template<class InputType>
class IOHprofiler_ExternPythonProblem : public IOHprofiler_problem<InputType>
{
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
        IOHprofiler_ExternPythonProblem(
                std::string module_name,
                std::string instance_name,
                std::string objective_function_api = "call",
                std::string instance_api_get = "get_instance",
                std::string dimension_api_get = "get_number_of_variables",
                std::string problem_id_api_get = "get_problem_id",
                std::string lowerbound_api = "get_lowerbound",
                std::string upperbound_api = "get_upperbound",
                std::string minimization_api = "get_minimization",
                std::string name_api = "get_name",
                std::string type_api = "get_type",
                std::string number_of_objectives_api = "get_number_of_objectives"
        );

        // TODO Unused API:
        // virtual void prepare_problem() { }
        // virtual void customize_optimal() { }

        /** Actually call the objective function. */
        virtual double internal_evaluate(const std::vector<InputType> &x);

        /** Deallocate Python variables. */
        virtual ~IOHprofiler_ExternPythonProblem();

    protected:
        // This is a lot of boiler plate, but it's better to be eplicit.

        std::string _api_objfunc;
        PyObject* _py_objfunc;

        std::string _api_instance_get;
        PyObject* _py_instance_get;

        std::string _api_dimension_get;
        PyObject* _py_dimension_get;

        std::string _api_problem_id_get;
        PyObject* _py_problem_id_get;

        std::string _api_lowerbound_get;
        PyObject* _py_lowerbound_get;

        std::string _api_upperbound_get;
        PyObject* _py_upperbound_get;

        std::string _api_minimization_get;
        PyObject* _py_minimization_get;

        std::string _api_name_get;
        PyObject* _py_name_get;

        std::string _api_type_get;
        PyObject* _py_type_get;

        std::string _api_number_of_objectives_get;
        PyObject* _py_number_of_objectives_get;

        const InputType _return_on_error;

        IOHprofiler_ExternPythonHelper<InputType> py;
};

#include "IOHprofiler_extern_python.hpp"

#endif // _IOHPROFILER_EXTERN_PYTHON_H

