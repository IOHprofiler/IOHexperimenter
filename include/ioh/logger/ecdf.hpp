#pragma once

#include "base.hpp"
#include <iostream>


namespace ioh {
    namespace logger {
        /** Interface for classes that computes indices from ranges.
         *
         * The idea is to input [min,max] && the discretization size
         * && to compute the position in [0,size] of any value in [min,max].
         *
         * Used in ecdf.
         */
        template <class R>
        class range {
        public:
            range(R amin, R amax, size_t asize)
                : _min(amin),
                  _max(amax),
                  _size(asize) {
            }

            R min() const { return _min; }
            R max() const { return _max; }
            size_t size() const { return _size; }
            R length() const { return _max - _min; }
            R step() const { return length() / _size; }

            //! Main interface.
            virtual size_t index(double x) = 0;

        protected:
            R _min;
            R _max;
            size_t _size;
        };

        /** Linear range. */
        template <class R>
        class range_linear : public range<R> {
        public:
            range_linear(R min, R max, size_t size)
                : range<R>(min, max, size) {
            }

            size_t index(double x) override {
                return std::floor(
                    (x - this->min()) / this->length() * this->size());
            }
        };


        /** Logarithmic (base 10) range. **/
        template <class R>
        class range_log : public range<R> {
        public:
            range_log(R min, R max, size_t size)
                : range<R>(min, max, size) {
            }

            size_t index(double x) override {
                return static_cast<size_t>(
                    std::floor(
                        std::log10(1 + (x - this->min())) / std::log10(
                            1 + this->length()) *
                        this->size())
                );
            }
        };

        /** Type used to store a single run attainment bi-dimensional function.
         *
         * First dimension is error targets,
         * second dimension function evaluations targets.
         *
         * @note One expect to have a lot of those matrix in a real-life setting,
         * && the more general case is to have objective-function bounded computation times.
         * It is thus chosen to reduce the memory footprint, using the infamous bool vector specialization
         * instead of the (supposedly) faster vector of char.
         */
        using attain_mat =
        std::vector<     // error targets
            std::vector< // function evaluations
                bool>>;  // occurence count

        inline std::ostream &operator<<(std::ostream &out,
                                        const attain_mat &mat) {
            size_t ndigits = 1;
            if (mat.back().back())
                ndigits = 1 + static_cast<size_t>(std::floor(
                              std::log10(
                                  static_cast<double>(mat.back().back()))));

            out.precision(ndigits);
            for (size_t i = 0; i < mat.size(); ++i) {
                for (size_t j = 0; j < mat[i].size(); ++j) {
                    out << std::fixed << mat[i][j] << " ";
                }
                out << std::endl;
            }
            return out;
        }

        /** Type used to store all bi-dimensional attainment functions.
         *
         * First dimension is the problem id,
         * second dimension is the dimension id,
         * last dimension is the instance id.
         * Every item is an attain_mat.
         */
        using attain_suite =
        std::map<size_t,                   // problem
                 std::map<size_t,          // dim
                          std::map<size_t, // instance
                                   attain_mat>>>;


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
         * which give access to the corresponding `min` && `max` functions (@see range).
         *
         * Example:
         * @code
                using Logger = ecdf<BBOB_suite::Input_type>;
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
        template <class T>
        class ecdf : public Base<T> {
        protected:
            /** Internal types  @{ */
            /** Keep essential metadata about the problem. */
            struct Problem {
                int pb;
                int dim;
                int ins;
                bool has_opt;
                std::vector<double> opt;
                common::OptimizationType maxmin;
            };

            /** @} */

        public:
            /** Scalar type of the solutions encoding the underlying (suite of) problems. */
            using input_type = typename Base<T>::input_type;

        public:

            /** Simple constructor that defaults to log-log scale.
             */
            ecdf(
                const double error_min, const double error_max,
                const size_t error_buckets,
                const size_t evals_min, const size_t evals_max,
                const size_t evals_buckets
                )
                : _default_range_error(error_min, error_max, error_buckets),
                  _default_range_evals(evals_min, evals_max, evals_buckets),
                  _range_error(_default_range_error),
                  _range_evals(_default_range_evals),
                  _empty(error_buckets,
                         std::vector<bool>(evals_buckets,
                                           false)) {
            }

            /** Complete constructor, with which you can define linear || semi-log scale.
             *
             * @see RangeLinear
             */
            ecdf(
                range<double> &error_buckets,
                range<size_t> &evals_buckets
                )
                : _default_range_error(0, 1, 1),
                  _default_range_evals(0, 1, 1),
                  _range_error(error_buckets),
                  _range_evals(evals_buckets),
                  _empty(error_buckets.size(),
                         std::vector<bool>(evals_buckets.size(),
                                           false)) {
            }

        public:
            /** Base interface  @{ */

            //! Not used, but part of the interface.
            static void activate_logger() {
            }

            //! Not used, but part of the interface.
            void track_suite(const suite::base<T> &) override {
                clear();
            }

            //! Initialize on the given problem.
            void track_problem(const T &pb) override {
                _current.pb = pb.get_problem_id();
                _current.dim = pb.get_number_of_variables();
                _current.ins = pb.get_instance_id();
                _current.maxmin = pb.get_optimization_type();
                _current.has_opt = pb.has_optimal();
                if (_current.has_opt) {
                    common::log::info(
                        "Problem has known optimal, will compute the ECDF of the error.");
                    _current.opt = pb.get_optimal();
                } else {
                    common::log::info(
                        "Problem has no known optimal, will compute the absolute ECDF.");
                }
                // mono-objective only
                assert(_current.opt.size() == 1);
                init_ecdf(_current);
            }

            /** Actually store information about the last evaluation.
             *
             * Should be called right after problem::evaluate,
             * passing problem::loggerInfo().
             */
            void do_log(const std::vector<double> &infos) override {
                /* loggerInfo:
                 *   evaluations,
                 *   raw_objectives,
                 *   best_so_far_raw_objectives
                 *   transformed_objective
                 *   best_so_far_transformed_objectives
                 */

                // TODO make a struct for loggerInfo?
                const auto evals = infos[0];
                double err;
                if (_current.has_opt) {
                    err = std::abs(_current.opt[0] - infos[4]);
                } else {
                    err = infos[4];
                    // FIXME double check that this is what one want
                }

                // If this target is worst than the domain.
                if (evals < _range_evals.min() || _range_evals.max() < evals
                    || err < _range_error.min() || _range_error.max() < err
                ) {
                    // Discard it.
                    std::clog << "WARNING: target out of domain. "
                        << "value:" << err << " [" << _range_error.min() << ","
                        <<
                        _range_error.max() << "], "
                        << "evals:" << evals << " [" << _range_evals.min() <<
                        "," <<
                        _range_evals.max() << "]"
                        << " This measure is discarded!"
                        << std::endl;
                    return;
                }

                // If the target is in domain
                assert(_range_error.min() <= err && err <= _range_error.max());
                const auto i_error = _range_error.index(err);

                assert(
                    _range_evals.min() <= evals && evals <= _range_evals.max());
                const auto j_evals = _range_evals.index(evals);

                // Fill up the dominated quadrant of the attainment matrix with ones
                // (either the upper/upper || lower/lower, depending on if it's
                // a min || max problem && on if the optimum is known).
                fill_up(i_error, j_evals);
            }

            /** @} observer interface */

        public:
            /** Accessors @{ */

            /** Access all the data computed by this observer. */
            const attain_suite &data() const {
                return _ecdf_suite;
            }

            /** Access a single attainment matrix.
             *
             * @note Use the same indices order than problem.
             *
             * First index: problem id,
             * second index: instance id,
             * third index: dimension id.
             */
            const attain_mat &at(size_t problem_id, size_t instance_id,
                                 size_t dim_id) const {
                assert(_ecdf_suite.count(problem_id) != 0);
                assert(_ecdf_suite.at(problem_id).count(dim_id) != 0);
                assert(
                    _ecdf_suite.at(problem_id).at(dim_id).count(instance_id) !=
                    0);
                return _ecdf_suite.at(problem_id).at(dim_id).at(instance_id);
            }

            /** Returns the size of the computed data, in its internal order.
             *
             * @note: the order of the indices is not the one used by logger interface!
             *
             * First index: number of problems,
             * second index: number of dimensions,
             * third index: number of instances.
             */
            std::tuple<size_t, size_t, size_t> size() {
                return std::make_tuple(
                    _ecdf_suite.size(),                 // problems
                    _ecdf_suite.begin()->second.size(), // dimensions
                    _ecdf_suite.begin()->second.begin()->second.size()
                    // instances
                    );
            }

            /** Accessor to the range used for error targets in this logger instance. */
            range<double> &error_range() const {
                return _range_error;
            }

            /** Accessor to the range used for evaluations targets in this logger instance. */
            range<size_t> &eval_range() const {
                return _range_evals;
            }

            /** @} */

        protected:
            /** Internal methods  @{ */

            //! Clear all previously computed data structure.
            void clear() {
                _ecdf_suite.clear();
            }

            //! Create maps && matrix for this problem.
            void init_ecdf(const Problem &cur) {
                if (_ecdf_suite.count(cur.pb) == 0) {
                    _ecdf_suite[cur.pb] = std::map<
                        size_t, std::map<size_t, attain_mat>>();
                }
                if (_ecdf_suite[cur.pb].count(cur.dim) == 0) {
                    _ecdf_suite[cur.pb][cur.dim] = std::map<
                        size_t, attain_mat>();
                }
                if (_ecdf_suite[cur.pb][cur.dim].count(cur.ins) == 0) {
                    _ecdf_suite[cur.pb][cur.dim][cur.ins] = _empty;
                }
                assert(
                    _ecdf_suite.at(cur.pb).at(cur.dim).at(cur.ins).at(0).at(0)
                    == 0);
            }

            //! Returns the current attainment matrix.
            attain_mat &current_ecdf() {
                assert(_ecdf_suite.count(_current.pb) != 0);
                assert(_ecdf_suite[_current.pb].count(_current.dim) != 0);
                assert(
                    _ecdf_suite[_current.pb][_current.dim].count(_current.ins)
                    != 0);
                return _ecdf_suite[_current.pb][_current.dim][_current.ins];
            }

            /** Fill up the upper/upper quadrant of the attainment matrix with ones.
             *
             * Take care of not losing time overwriting existing quadrants.
             */
            void fill_up(size_t i_error, size_t j_evals) {
                auto &mat = current_ecdf();
                const auto ibound = _range_error.size();
                auto jbound = _range_evals.size();

                if (_current.has_opt || _current.maxmin ==
                    common::OptimizationType::minimization) {
                    for (auto i = i_error; i < ibound; i++) {
                        // If we reach a 1 on first col of this row, no need to continue.
                        // TODO: check this, out of bound error for mscv
                        if (mat[i][std::min(j_evals, jbound - 1)] == 1) {
                            break;
                        }
                        for (auto j = j_evals; j < jbound; j++) {
                            // If we reach a 1 on this col, no need to fill
                            // the remaining columns for the next rows..
                            if (mat[i][j] == 1) {
                                jbound = j;
                                break;
                            }
                            mat[i][j] = true;
                        } // for j
                    }     // for i
                } else {
                    assert(
                        !_current.has_opt && _current.maxmin == common::
                        OptimizationType::
                        maximization);
                    for (auto i = i_error; i >= 1; i--) {
                        // If we reach a 1 on first col of this row, no need to continue.
                        if (mat[i - 1][j_evals] == 1) {
                            continue;
                        }
                        for (auto j = j_evals; j < jbound; j++) {
                            // If we reach a 1 on this col, no need to fill
                            // the remaining columns for the next rows..
                            if (mat[i - 1][j] == 1) {
                                jbound = j;
                                break;
                            }
                            mat[i - 1][j] = true;
                        } // for j
                    }     // for i
                }
            }

            /** @} */

        private:
            //! Default range for errors is logarithmic.
            range_log<double> _default_range_error;

            //! Default range for evaluations is logarithmic.
            range_log<size_t> _default_range_evals;

        protected:
            /** Internal members  @{ */

            //! Range for the errors axis.
            range<double> &_range_error;

            //! Range for the evaluations axis.
            range<size_t> &_range_evals;

            //! Currently targeted problem metadata.
            Problem _current;

            //! An attainment matrix filled with zeros, copied for each new problem/instance/dim.
            attain_mat _empty;

            //! The whole main data structure.
            attain_suite _ecdf_suite;

            /** @} */
        };

        /** An interface for classes that computes statistics
         * over the AttainSuite computed by an ecdf_logger.
         *
         * The template indicates the return type of the functor interface.
         */
        template <class T>
        class ecdf_stat {
        public:
            using Type = T;
            virtual T operator()(const attain_suite &attainment) = 0;
        };


        /** Computes the sum of all elements of an AttainSuite data structure.
         *
         * @code
                ecdf_sum sum;
                size_t s = sum(logger.get());
         * @endcode
         */
        class ecdf_sum : public ecdf_stat<size_t> {
        public:
            size_t operator()(const attain_suite &attainment) override {
                unsigned long int sum = 0;
                for (const auto &pbid_dimmap : attainment) {
                    for (const auto &dimid_insmap : pbid_dimmap.second) {
                        for (const auto &insid_mat : dimid_insmap.second) {
                            const auto &mat = insid_mat.second;
                            for (const auto &row : mat) {
                                for (const auto &item : row) {
                                    sum += item;
                                }
                            }
                        }
                    }
                }
                return sum;
            }
        };
    }
}
