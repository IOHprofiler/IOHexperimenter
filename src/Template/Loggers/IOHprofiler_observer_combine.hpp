
template<class T>
IOHprofiler_observer_combine<T>::IOHprofiler_observer_combine( IOHprofiler_observer<T>& observer ) :
    _observers(1,&observer)
{ }

template<class T>
IOHprofiler_observer_combine<T>::IOHprofiler_observer_combine( std::vector<IOHprofiler_observer<T>*> observers ) :
    _observers(observers)
{
    assert(_observers.size() > 0);
}

template<class T>
void IOHprofiler_observer_combine<T>::add( IOHprofiler_observer<T>& observer )
{
    _observers.push_back(&observer);
}

template<class T>
void IOHprofiler_observer_combine<T>::track_suite(IOHprofiler_suite<T>& suite)
{
    for(auto& p : _observers) {
        p->track_suite(suite);
    }
}

template<class T>
void IOHprofiler_observer_combine<T>::track_problem(IOHprofiler_problem<T> & pb)
{
    for(auto& p : _observers) {
        p->track_problem(pb);
    }
}

template<class T>
void IOHprofiler_observer_combine<T>::do_log(const std::vector<double> &logger_info)
{
    for(auto& p : _observers) {
        p->do_log(logger_info);
    }
}

