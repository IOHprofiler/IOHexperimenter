/// \file IOHprofiler_ecdf_logger.h
/// \brief Header file for class IOHprofiler_ecdf_logger.
///
///
/// \author Johann Dreo
/// \date 2020-02-04

/***** IOHprofiler_Range ****/

template<class R>
IOHprofiler_Range<R>::IOHprofiler_Range(R amin, R amax, size_t asize) :
    _min(amin),
    _max(amax),
    _size(asize)
{ }

template<class R>
R IOHprofiler_Range<R>::min() const {return _min;}

template<class R>
R IOHprofiler_Range<R>::max() const {return _max;}

template<class R>
size_t IOHprofiler_Range<R>::size() const {return _size;}

template<class R>
R IOHprofiler_Range<R>::length() const {return _max - _min;}

template<class R>
R IOHprofiler_Range<R>::step() const {return length()/_size;}


/***** IOHprofiler_RangeLinear ****/

template<class R>
IOHprofiler_RangeLinear<R>::IOHprofiler_RangeLinear(R min, R max, size_t size) :
    IOHprofiler_Range<R>(min,max,size)
{ }

template<class R>
size_t IOHprofiler_RangeLinear<R>::index( double x )
{
    return std::floor( (x - this->min()) / (this->length()) * this->size() );
}


/***** IOHprofiler_RangeLog ****/

template<class R>
IOHprofiler_RangeLog<R>::IOHprofiler_RangeLog(R min, R max, size_t size) :
    IOHprofiler_Range<R>(min,max,size)
{ }

template<class R>
size_t IOHprofiler_RangeLog<R>::index( double x )
{
    return std::floor( std::log10(1+(x-this->min())) / std::log10(1+this->length()) * this->size() );
}


/***** IOHprofiler_AttainMat *****/

std::ostream& operator<<(std::ostream& out, const IOHprofiler_AttainMat& mat)
{
    // Just in case one store anything else than 0/1 some day.
    size_t ndigits = 1;
    if(mat.back().back() > 0) {
        ndigits = 1+std::floor(std::log10(mat.back().back()));
    }
    out.precision(ndigits);
    for(size_t i=0; i < mat.size(); ++i) {
        for(size_t j = 0; j < mat[i].size(); ++j) {
            out << std::fixed << mat[i][j] << " ";
        }
        out << std::endl;
    }
    return out;
}


/***** IOHprofiler_ecdf_logger *****/

template<class T>
IOHprofiler_ecdf_logger<T>::IOHprofiler_ecdf_logger(
        const double error_min, const double error_max, const size_t error_buckets,
        const size_t evals_min, const size_t evals_max, const size_t evals_buckets
    ) :
        _default_range_error(error_min, error_max, error_buckets),
        _default_range_evals(evals_min, evals_max, evals_buckets),
        _range_error(_default_range_error),
        _range_evals(_default_range_evals),
        _empty( error_buckets,
            std::vector<bool>( evals_buckets,
                0))
{ }

template<class T>
IOHprofiler_ecdf_logger<T>::IOHprofiler_ecdf_logger(
        IOHprofiler_Range<double>& error_buckets,
        IOHprofiler_Range<size_t>& evals_buckets
    ) :
        _default_range_error(0,1,1),
        _default_range_evals(0,1,1),
        _range_error(error_buckets),
        _range_evals(evals_buckets),
        _empty( error_buckets.size(),
            std::vector<bool>( evals_buckets.size(),
                0))
{ }

template<class T>
void IOHprofiler_ecdf_logger<T>::activate_logger()
{
    // pass
}

template<class T>
void IOHprofiler_ecdf_logger<T>::track_suite(const IOHprofiler_suite<T>&)
{
    reset();
}

template<class T>
void IOHprofiler_ecdf_logger<T>::track_problem(const IOHprofiler_problem<T> & pb)
{
    _current.pb     = pb.IOHprofiler_get_problem_id();
    _current.dim    = pb.IOHprofiler_get_number_of_variables();
    _current.ins    = pb.IOHprofiler_get_instance_id();
    _current.maxmin = pb.IOHprofiler_get_optimization_type();
    _current.opt    = pb.IOHprofiler_get_optimal();
    // mono-objective only
    assert(_current.opt.size() == 1);
    init_ecdf(_current);
}

template<class T>
void IOHprofiler_ecdf_logger<T>::do_log(const std::vector<double>& infos)
{
    /* loggerInfo:
     *   evaluations,
     *   raw_objectives,
     *   best_so_far_raw_objectives
     *   transformed_objective
     *   best_so_far_transformed_objectives
     */

    // TODO make a struct for loggerInfo?
    double evals = infos[0];
    double err = std::abs(_current.opt[0] - infos[4]);

    // If this target is worst than the domain.
    if( evals > _range_evals.max() or err > _range_error.max() ) {
        // Discard it.
        std::clog << "WARNING: target out of domain. Error:" << err << ", evaluations:" << evals << std::endl;
        return;

    } else {
        size_t i_error;
        size_t j_evals;

        // If the target is in domain
        if(_range_error.min() <= err and err <= _range_error.max()) {
            i_error = _range_error.index(err);

        // If the target is better than the domain
        } else if( err < _range_error.min() ) {
            i_error = 0;
        }

        if(_range_evals.min() <= evals and evals <= _range_evals.max()) {
            j_evals = _range_evals.index(evals);

        } else if( evals < _range_evals.min() ) {
            j_evals = 0;
        }

        // Fill up the upper/upper quadrant of the attainment matrix with ones.
        fill_up(i_error, j_evals);
    }
}

template<class T>
const IOHprofiler_AttainSuite& IOHprofiler_ecdf_logger<T>::data()
{
    return _ecdf_suite;
}

template<class T>
const IOHprofiler_AttainMat& IOHprofiler_ecdf_logger<T>::at(size_t problem_id, size_t instance_id, size_t dim_id) const
{
    assert(_ecdf_suite.count(problem_id) != 0);
    assert(_ecdf_suite.at(problem_id).count(dim_id) != 0);
    assert(_ecdf_suite.at(problem_id).at(dim_id).count(instance_id) != 0);
    return _ecdf_suite.at(problem_id).at(dim_id).at(instance_id);
}

template<class T>
std::tuple<size_t, size_t, size_t> IOHprofiler_ecdf_logger<T>::size()
{
    return std::make_tuple(
        _ecdf_suite.size(), // problems
        _ecdf_suite.begin()->second.size(), // dimensions
        _ecdf_suite.begin()->second.begin()->second.size() // instances
        );
}

template<class T>
IOHprofiler_Range<double> IOHprofiler_ecdf_logger<T>::error_range()
{
    return _range_error;
}

template<class T>
IOHprofiler_Range<size_t> IOHprofiler_ecdf_logger<T>::eval_range() {
    return _range_evals;
}

template<class T>
void IOHprofiler_ecdf_logger<T>::reset()
{
    _ecdf_suite.clear();
}

template<class T>
void IOHprofiler_ecdf_logger<T>::init_ecdf( const Problem& cur )
{
    if(_ecdf_suite.count(cur.pb) == 0) {
        _ecdf_suite[cur.pb] = std::map< size_t, std::map< size_t, IOHprofiler_AttainMat >>();
    }
    if(_ecdf_suite[cur.pb].count(cur.dim) == 0) {
        _ecdf_suite[cur.pb][cur.dim] = std::map< size_t, IOHprofiler_AttainMat >();
    }
    if(_ecdf_suite[cur.pb][cur.dim].count(cur.ins) == 0) {
        _ecdf_suite[cur.pb][cur.dim][cur.ins] = _empty;
    }
    assert(_ecdf_suite.at(cur.pb).at(cur.dim).at(cur.ins).at(0).at(0) == 0);
}

template<class T>
IOHprofiler_AttainMat& IOHprofiler_ecdf_logger<T>::current_ecdf()
{
    assert(_ecdf_suite.count(_current.pb) != 0);
    assert(_ecdf_suite[_current.pb].count(_current.dim) != 0);
    assert(_ecdf_suite[_current.pb][_current.dim].count(_current.ins) != 0);
    return _ecdf_suite[_current.pb][_current.dim][_current.ins];
}

template<class T>
void IOHprofiler_ecdf_logger<T>::fill_up( size_t i_error, size_t j_evals)
{
    IOHprofiler_AttainMat& mat = current_ecdf();
    size_t imax = _range_error.size();
    size_t jmax = _range_evals.size();
    for(size_t i = i_error; i < imax; i++) {
        // If we reach a 1 on first col of this row, no need to continue.
        if(mat[i][j_evals] == 1) {
            break;
        } else {
            for(size_t j = j_evals; j < jmax; j++) {
                // If we reach a 1 on this col, no need to fill
                // the remaining columns for the next rows..
                if(mat[i][j] == 1) {
                    jmax = j;
                    break;
                } else {
                    mat[i][j] = 1;
                }
            } // for j
        }
    } // for i
}


/***** IOHprofiler_ecdf_sum *****/

size_t IOHprofiler_ecdf_sum::operator()(const IOHprofiler_AttainSuite& attainment)
{
    unsigned long int sum = 0;
    for(const auto& pbid_dimmap : attainment ) {
        for(const auto& dimid_insmap : pbid_dimmap.second ) {
            for(const auto& insid_mat : dimid_insmap.second) {
                const IOHprofiler_AttainMat& mat = insid_mat.second;
                for(const auto& row : mat) {
                    for(const auto& item : row ) {
                        sum += item;
                    }
                }
            }
        }
    }
    return sum;
}

