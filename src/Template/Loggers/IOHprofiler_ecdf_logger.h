/// \file IOHprofiler_ecdf_logger.h
/// \brief Header file for classes: IOHprofiler_ecdf_logger, IOHprofiler_Range*, IOHprofiler_ecdf_*.
///
/// \author Johann Dreo
#ifndef _IOHPROFILER_ECDF_LOGGER_H_
#define _IOHPROFILER_ECDF_LOGGER_H_

#include <iostream>
#include <sstream>

#include "IOHprofiler_observer.h"
#include "IOHprofiler_common.h"
#include "IOHprofiler_problem.h"

/** Interface for classes that computes indices from ranges.
 *
 * The idea is to input [min,max] and the discretization size
 * and to compute the position in [0,size] of any value in [min,max].
 *
 * Used in IOHprofiler_ecdf_logger.
 */
template<class R>
class IOHprofiler_Range
{
    public:
        IOHprofiler_Range(R amin, R amax, size_t asize);
        R min() const;
        R max() const;
        size_t size() const;
        R length() const;
        R step() const;

        //! Main interface.
        virtual size_t index( double x ) = 0;

    protected:
        R _min;
        R _max;
        size_t _size;
};

/** Linear range.
 */
template<class R>
class IOHprofiler_RangeLinear : public IOHprofiler_Range<R>
{
    public:
        IOHprofiler_RangeLinear(R min, R max, size_t size);

        virtual size_t index( double x );
};

/** Logarithmic (base 10) range.
 */
template<class R>
class IOHprofiler_RangeLog : public IOHprofiler_Range<R>
{
    public:
        IOHprofiler_RangeLog(R min, R max, size_t size);

        virtual size_t index( double x );
};

/** Type used to store a single run attainment bi-dimensional function.
 *
 * First dimension is error targets,
 * second dimension function evaluations targets.
 *
 * @note One expect to have a lot of those matrix in a real-life setting,
 * and the more general case is to have objective-function bounded computation times.
 * It is thus chosen to reduce the memory footprint, using the infamous bool vector specialization
 * instead of the (supposedly) faster vector of char.
 */
using IOHprofiler_AttainMat =
    std::vector<       // error targets
        std::vector<   // function evaluations
            bool >>;   // occurence count

/** Print an attainment matrix on an output stream.
 */
std::ostream& operator<<(std::ostream& out, const IOHprofiler_AttainMat& mat);

/** Type used to store all bi-dimensional attainment functions.
 *
 * First dimension is the problem id,
 * second dimension is the dimension id,
 * last dimension is the instance id.
 * Every item is an IOHprofiler_AttainMat.
 */
using IOHprofiler_AttainSuite =
    std::map< size_t,         // problem
        std::map< size_t,     // dim
            std::map< size_t, // instance
                 IOHprofiler_AttainMat >>>; // ECDF

/** An observer which stores bi-dimensional error/evaluations discretized attainment matrices.
 *
 * A matrix is stored for each triplet (problem,dimension,instance),
 * everything being identified by its index.
 * Attainment matrices are boolean 2D tables, in which a 1 indicates that
 * an error target (row index) have been attained at an evaluation target (column index).
 *
 * The whole set of matrices can be accessed by the `get` accessor.
 * A single one can be accessed by the `at` accessor.
 * Ranges can be get back by `*_range` accessors,
 * which give access to the corresponding `min` and `max` functions (@see IOHprofiler_Range).
 *
 * Example:
 * @code
    using Logger = IOHprofiler_ecdf_logger<BBOB_suite::Input_type>;
    BBOB_suite bench({1,2},{1,2},{2,10});
    Logger logger(0,4e7,0, 0,100,20);

    BBOB_suite::Problem_ptr pb;
    while(pb = bench.get_next_problem()) {
        logger.target_problem(*pb);
        // Make a `sol`
        for(size_t i=0; i<100; ++i) { // 100 evaluations
            double f = pb->evaluate(sol);
            logger.write_line(pb->loggerInfo());
        }
    }
    std::clog << logger.at(1,2,2) << std::endl;
 * @endcode
 */
template<class T>
class IOHprofiler_ecdf_logger : public IOHprofiler_observer<T>
{
    protected:
        /** Internal types  @{ */
        /** Keep essential metadata about the problem. */
        struct Problem {
            int pb;
            int dim;
            int ins;
            bool has_opt;
            std::vector<double> opt;
            IOH_optimization_type maxmin;
        };
        /** @} */

    public:
        /** Scalar type of the solutions encoding the underlying (suite of) problems. */
        using InputType = typename IOHprofiler_observer<T>::InputType;

    public:

        /** Simple constructor that defaults to log-log scale.
         */
        IOHprofiler_ecdf_logger(
                const double error_min, const double error_max, const size_t error_buckets,
                const size_t evals_min, const size_t evals_max, const size_t evals_buckets
            );

        /** Complete constructor, with which you can define linear or semi-log scale.
         *
         * @see IOHprofiler_RangeLinear
         */
        IOHprofiler_ecdf_logger(
                IOHprofiler_Range<double>& error_buckets,
                IOHprofiler_Range<size_t>& evals_buckets
            );

    public:
        /** Observer interface  @{ */

        //! Not used, but part of the interface.
        void activate_logger();

        //! Not used, but part of the interface.
        void track_suite(const IOHprofiler_suite<T>&);

        //! Initialize on the given problem.
        void track_problem(const IOHprofiler_problem<T> & pb);

        /** Actually store information about the last evaluation.
         *
         * Should be called right after IOHprofiler_problem::evaluate,
         * passing IOHprofiler_problem::loggerInfo().
         */
        void do_log(const std::vector<double>& infos);

        /** @} observer interface */

    public:
        /** Accessors @{ */

        /** Access all the data computed by this observer. */
        const IOHprofiler_AttainSuite& data();

        /** Access a single attainment matrix.
         *
         * @note Use the same indices order than IOHprofiler_problem.
         *
         * First index: problem id,
         * second index: instance id,
         * third index: dimension id.
         */
        const IOHprofiler_AttainMat& at(size_t problem_id, size_t instance_id, size_t dim_id) const;

        /** Returns the size of the computed data, in its internal order.
         *
         * @note: the order of the indices is not the one used by IOHprofiler_logger interface!
         *
         * First index: number of problems,
         * second index: number of dimensions,
         * third index: number of instances.
         */
        std::tuple<size_t, size_t, size_t> size();

        /** Accessor to the range used for error targets in this logger instance. */
        IOHprofiler_Range<double> error_range();

        /** Accessor to the range used for evaluations targets in this logger instance. */
        IOHprofiler_Range<size_t> eval_range();

        /** @} */

    protected:
        /** Internal methods  @{ */

        //! Clear all previously computed data structure.
        void reset();

        //! Create maps and matrix for this problem.
        void init_ecdf( const Problem& cur );

        //! Returns the current attainment matrix.
        IOHprofiler_AttainMat& current_ecdf();

        /** Fill up the upper/upper quadrant of the attainment matrix with ones.
         *
         * Take care of not losing time overwriting existing quadrants.
         */
        void fill_up( size_t i_error, size_t j_evals);

        /** @} */

    protected:
        /** Internal members  @{ */

        //! Range for the errors axis.
        IOHprofiler_Range<double>& _range_error;

        //! Range for the evaluations axis.
        IOHprofiler_Range<size_t>& _range_evals;

        //! Currently targeted problem metadata.
        Problem _current;

        //! An attainment matrix filled with zeros, copied for each new problem/instance/dim.
        IOHprofiler_AttainMat _empty;

        //! The whole main data structure.
        IOHprofiler_AttainSuite _ecdf_suite;

    private:
        //! Default range for errors is logarithmic.
        IOHprofiler_RangeLog<double> _default_range_error;

        //! Default range for evaluations is logarithmic.
        IOHprofiler_RangeLog<size_t> _default_range_evals;

        /** @} */
};

/** An interface for classes that computes statistics
 * over the IOHprofiler_AttainSuite computed by an IOHprofiler_ecdf_logger.
 *
 * The template indicates the return type of the functor interface.
 */
template<class T>
class IOHprofiler_ecdf_stat
{
    public:
        using Type = T;
        virtual T operator()(const IOHprofiler_AttainSuite& attainment) = 0;
};

/** Computes the sum of all elements of an IOHprofiler_AttainSuite data structure.
 *
 * @code
    IOHprofiler_ecdf_sum sum;
    size_t s = sum(logger.get());
 * @endcode
 */
class IOHprofiler_ecdf_sum : public IOHprofiler_ecdf_stat<size_t>
{
    public:
        size_t operator()(const IOHprofiler_AttainSuite& attainment);
};

#include "IOHprofiler_ecdf_logger.hpp"

#endif // _IOHPROFILER_ECDF_LOGGER_H_

