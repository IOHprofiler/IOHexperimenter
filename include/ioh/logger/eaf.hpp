#pragma once

#include <vector>
#include <map>
#include <string>
#include <numeric>
#include <memory>

#include "loggers.hpp"

namespace ioh {
namespace logger {

    /** @defgroup EAF Empirical Attainment Function
     * Features related to the EAF logger.
     * 
     * @ingroup Loggers
     */

    /** @defgroup EAF_API EAF API
     * API related to the EAF logger.
     * 
     * @ingroup Logging
     */

    namespace eaf {
        /** A point in the quality/time 2D space.
         * 
         * That is, the values associated with an objective function call,
         * as logged by an EAF logger.
         *
         * "time" means "number of calls to the objective function".
         * 
         * @ingroup EAF_API
         */
        struct Point {
            /** The quality of the point, i.e. its objective function value. */
            double qual;

            /** The time of the point, i.e. the number of the objective function calls. */
            size_t time;

            /** Empty constructor, building up an invalid Point. */
            Point() : qual(std::numeric_limits<double>::signaling_NaN()),
                      time(std::numeric_limits<size_t>::signaling_NaN())
            { }

            /** Constructor with quality, then time. */
            Point(double quality, size_t nb_evals) : qual(quality), time(nb_evals) { }

            /** Constructor with time, then quality. */
            Point(size_t nb_evals, double quality) : qual(quality), time(nb_evals) { }

            friend std::ostream& operator<< (std::ostream& out, const Point& p )
            {
                out << "{\"qual\":" << p.qual << ",\"time\":" << p.time << "}";
                return out;
            }
        };

        /** A point and its corresponding run index.
         *
         * Used in stat::Levels.
         * 
         * @ingroup EAF_API
         */
        struct RunPoint : public Point {
            /** Index of the run among all the runs hosted in the logger. */
            size_t run;

            /** Empty constructor, building up an invalid Point. */
            RunPoint() : Point(), run(std::numeric_limits<size_t>::signaling_NaN()) { }

            /** Constructor with quality, then time. */
            RunPoint(double q, size_t t, size_t r) : Point(q,t), run(r) { }

            /** Constructor with time, then quality. */
            RunPoint(size_t t, double q, size_t r) : Point(q,t), run(r) { }

            friend std::ostream& operator<< (std::ostream& out, const RunPoint& p )
            {
                out << "{\"run\":" << p.run << ", \"point\":";
                out << static_cast<const Point&>(p);
                out << "}";
                return out;
            }
        };

        /** A set of non-dominated points. */
        using Front = std::vector<eaf::RunPoint>; // Should be ordered already, no need to use a slower container.
        
        /** Comparator for sorting a vector of eaf::Point with descending qualities.
         * 
         * @returns true if the quality of `a` is larger than the quality of `b`.
         * 
         * @ingroup EAF_API
         */
        inline bool descending_qual(const eaf::Point& a, const eaf::Point& b) { // FIXME double check if it's the same for maximization
            return a.qual > b.qual;
        }

        /** Comparator for sorting a vector of eaf::Point with descending time.
         * 
         * @returns true if the time of `a` is larger than the quality of `b`.
         * 
         * @ingroup EAF_API
         */
        inline bool descending_time(const eaf::Point& a, const eaf::Point& b) { // FIXME double check if it's the same for maximization
            return a.time > b.time;
        }

        /** Comparator for sorting a vector of eaf::Point with ascending times.
         * 
         * @returns true if the time of `a` is smaller than the time of `b`.
         * 
         * @ingroup EAF_API
         */
        inline bool ascending_time(const eaf::Point& a, const eaf::Point& b) {
            return a.time < b.time;
        }

        /** Comparator for sorting a vector of eaf::Point with ascending qualities.
         * 
         * @returns true if the quality of `a` is smaller than the time of `b`.
         * 
         * @ingroup EAF_API
         */
        inline bool ascending_qual(const eaf::Point& a, const eaf::Point& b) {
            return a.qual < b.qual;
        }

    } // eaf

    /** A logger that store the 2D quality/time attainment fronts for each runs.
     * 
     * An attainment front is the set of Pareto-optimal quality/time targets attained during a given run.
     * That is, a set of points that are non-dominated: either the quality or the time is better than the other points in set.
     * 
     * The set of attainment fronts samples a 2D distribution of probability,
     * which is a generalization of performance distribution of the algorithm(s) observed by the logger(s).
     * Taking the convex subset of the projections of the non-dominated points for a given quality target would recover
     * the sample of the expected runtime empirical cumulative density function.
     * 
     * The underlying empirical attainment function levelsets can be computed with stat::Levels.
     *
     * @ingroup EAF
     * @ingroup Loggers
     * 
     * More information in the following publication:
     * @code
        @incollection{LopPaqStu09emaa,
          editor = { Thomas Bartz-Beielstein  and  Marco Chiarandini  and  Lu{\'\i}s Paquete  and  Mike Preuss },
          year = 2010,
          address = {Berlin, Germany},
          publisher = {Springer},
          booktitle = {Experimental Methods for the Analysis of Optimization Algorithms},
          author = { Manuel L{\'o}pez-Ib{\'a}{\~n}ez  and  Lu{\'\i}s Paquete  and  Thomas St{\"u}tzle },
          title = {Exploratory Analysis of Stochastic Local Search Algorithms in Biobjective Optimization},
          pages = {209--222},
          doi = {10.1007/978-3-642-02538-9_9},
          abstract = {This chapter introduces two Perl programs that
                      implement graphical tools for exploring the performance of stochastic local search algorithms
                      for biobjective optimization problems. These tools are based on the concept of the empirical attainment
                      function (EAF), which describes the probabilistic distribution of the outcomes obtained by a
                      stochastic algorithm in the objective space. In particular, we consider the visualization of
                      attainment surfaces and differences between the first-order EAFs of the outcomes of two
                      algorithms. This visualization allows us to identify certain algorithmic behaviors in a graphical way.
                      We explain the use of these visualization tools and illustrate them with examples arising from
                      practice.}
        }
     */
    class EAF : public Logger {
    public:
        /** @name Data structure types
         * Convenience naming for the underlying nested data structure.
         *
         * @{ */
        using Runs       = std::map<size_t     , eaf::Front>;
        using Instances  = std::map<int        , Runs      >;
        using Dimensions = std::map<int        , Instances >;
        using Problems   = std::map<int        , Dimensions>;
        using Suites     = std::map<std::string, Problems  >;
        /** @} */

        /** When attached directly to a Problem (out of a Suites), use the following key for the Suites map. */
        inline static const std::string default_suite = "None";

        /** Direct accessor to the data structure. */
        [[nodiscard]] const Suites& data() const {return _data;}

        /** A set of keys leading to a set of property values within the data structure. */
        struct Cursor {
            std::string suite;
            int pb;
            int dim;
            int ins;
            size_t run;
            Cursor(std::string suite_name = default_suite,
                    int problem_id = 0,
                    int dimension = 0,
                    int instance_id = 0,
                    size_t run_id = 0)
            : suite(suite_name),
              pb(problem_id),
              dim(dimension),
              ins(instance_id),
              run(run_id)
            { }
        };

        /** Return the front at the given cursor. */
        [[nodiscard]] const eaf::Front& data(const Cursor& cur) const
        {
#ifndef NDEBUG
            assert(_data.count(cur.suite) > 0);
            assert(_data.at(cur.suite).count(cur.pb) > 0);
            assert(_data.at(cur.suite).at(cur.pb).count(cur.dim) > 0);
            assert(_data.at(cur.suite).at(cur.pb).at(cur.dim).count(cur.ins) > 0);
            assert(_data.at(cur.suite).at(cur.pb).at(cur.dim).at(cur.ins).count(cur.run) > 0);
#endif
            return _data.at(cur.suite).at(cur.pb).at(cur.dim).at(cur.ins).at(cur.run);

        }

    public:
        /** Constructor.
         * 
         * @warning A classical error in C++ is to instantiate a constructor with empty parentheses.
         *          Here, you should take care of instantiating without any parenthese at all:
         *          @code
         *              ioh::logger::EAF my_logger;
         *          @endcode
         */
        EAF() : Logger(), _has_problem_type(false)
        {
            // Do not use the Logger's constructor, to avoid passing references to uninitialized members.
            // /!\ needed by the algorithm, do not change unless you know what you're doing.
            triggers_.insert(std::ref(_on_improvement));
            // /!\ needed by the related eaf::stat::* classes.
            properties_.insert_or_assign(_y_best.name(), _y_best);
            properties_.insert_or_assign(_evaluations.name(), _evaluations);
        }

        /** Set the current suite name.
         * 
         * @note If this is never called, the suite name defaults to Store::default_suite ("None").
         */
        virtual void attach_suite(const std::string& suite_name) override
        {
            _current.suite = suite_name;
        }

        /** Attach to a problem. */
        void attach_problem(const problem::MetaData& problem) override
        {
            Logger::attach_problem(problem);

            _current.pb  = problem.problem_id;
            _current.dim = problem.n_variables;
            _current.ins = problem.instance;
            
            const Runs& runs  = _data[_current.suite][_current.pb][_current.dim][_current.ins];
            _current.run      = runs.size(); // De facto next run id.


            IOH_DBG(note, "Attach to: pb=" << _current.pb << ", dim=" << _current.dim << ", ins=" << _current.ins << ", run=" << _current.run)
#ifndef NDEBUG
            if(_has_problem_type and _current_problem_type.type() != problem.optimization_type.type()) {
                IOH_DBG(warning, "different types of problems are mixed, you will not be able to compute levels")
            }
#endif
            _current_problem_type = problem.optimization_type;
            _has_problem_type = true;
#ifndef NDEBUG
            // Reset _current_best (double check of trigger::on_improvement behaviour).
            if(_current_problem_type == common::OptimizationType::MIN) {
                _current_best = std::numeric_limits<double>::infinity();
            } else {
                _current_best = -std::numeric_limits<double>::infinity();
            }
#endif
        }

        /** Process a log event. */
        void call(const logger::Info& log_info) override
        {
            IOH_DBG(debug, "EAF called after improvement")
            // Access the properties that were instantiated in the constructor.
            const std::optional<double> y_best      = properties_.at(_y_best.name()).get()(log_info);
            const std::optional<double> evaluations = properties_.at(_evaluations.name()).get()(log_info);
#ifndef NDEBUG
            assert(y_best); // Assert that the optional holds a value, which should be the case here.
            assert(evaluations);

            // If trigger::on_improvement does its job, we always have an improvement here.
            IOH_DBG(debug, "y_best=" << y_best.value()
                <<  (_current_problem_type == common::OptimizationType::MIN ? " <" : " >")
                << " current_best=" << _current_best << " ?");
            
            assert(_current_problem_type(y_best.value(), _current_best));
            _current_best = y_best.value();
#endif
            eaf::Front& f = current_front();
            f.push_back( eaf::RunPoint(
                    y_best.value(),
                    static_cast<size_t>(evaluations.value()),
                    _current.run
                ));
        }

        /** Reset the state. */
        void reset() override
        {
            IOH_DBG(debug, "EAF reset")
            Logger::reset();
#ifndef NDEBUG
            if(_current_problem_type == common::OptimizationType::MIN) {
                _current_best =  std::numeric_limits<double>::infinity();
            } else {
                _current_best = -std::numeric_limits<double>::infinity();
            }
#endif
        }

        common::OptimizationType optimization_type() const
        {
            assert(_has_problem_type);
            return _current_problem_type.type();
        }

    protected:
#ifndef NDEBUG
        // Use to double check that trigger::on_improvement does its job.
        // It should not be necessary otherwise.
        double _current_best;
#endif
        bool _has_problem_type;
        common::FOptimizationType _current_problem_type;

        /** The main data structure in which log events are stored. */
        Suites _data;

        /** The current Cursor. */
        Cursor _current;

        /** Accessor to the current front. */
        [[nodiscard]] eaf::Front& current_front()
        {
            // This creates maps if they do not exists.
            return _data[_current.suite][_current.pb][_current.dim][_current.ins][_current.run];
        }

        /** Default trigger is on every improvement.
        *
        * Because it fits the algorithmics.
        */
        trigger::OnImprovement _on_improvement;

        //! Property watching the number of evaluations.
        watch::Evaluations _evaluations;

        //! Property watching the objective function value.
        watch::CurrentBestY _y_best;


    }; // class EAF

    /** Everything related to the EAF logger. */
    namespace eaf {

        /** Computes attainment levels of the Empirical Attainment Function.
         * 
         * This computes the levelsets of the 2D (quality/time) function of the data logged by the EAF logger.
         * Calling it returns a (sub)set of Pareto fronts which can be interpreted as a 2D generalization of
         * quantiles.
         * Each levelset is a set a points defined by a quality and a number of calls to the objective function (the "time").
         * 
         * There is as much levelsets as there are runs that have been monitored by the logger.
         * Several of those levelsets are expected to be identical, as several runs may have attained the same level.
         * 
         * You can ask for only a subset of the levels at construction.
         * All levels are computed by default.
         * 
         * Example:
         * @code
            suite::BBOB suite({1, 2}, {1, 2}, {10, 30});
            logger::EAF logger;
            suite.attach_logger(logger);
            // [call the problem... the logger will store the quality/time Pareto fronts for each run.
            logger::eaf::stat::Levels levels_at(common::OptimizationType::MIN, {0, nb_runs/2, nb_runs-1}); // Will computes 3 level sets.
            auto levels = levels_at(logger); // Actually compute the levels.
         * @endcode
         *
         * From code: https://eden.dei.uc.pt/~cmfonsec/aft.html
         * Related publication:
         * @code
            @InProceedings{inp_Fonseca2011,
              author     = {Fonseca, Carlos M. and Guerreiro, Andreia P. and López-Ibáñez, Manuel and Paquete, Luís},
              booktitle  = {{I}nternational {C}onference on {E}volutionary {Multi}-{Criterion} {Optimization} ({EMO'11}},
              title      = {On the {Computation} of the {Empirical} {Attainment} {Function}},
              year       = {2011},
              address    = {Berlin, Heidelberg},
              editor     = {Takahashi, Ricardo H. C. and Deb, Kalyanmoy and Wanner, Elizabeth F. and Greco, Salvatore},
              pages      = {106--120},
              publisher  = {Springer},
              series     = {Lecture {Notes} in {Computer} {Science}},
              abstract   = {The attainment function provides a description of the location of the distribution of a random non-dominated point set. This function can be estimated from experimental data via its empirical counterpart, the empirical attainment function (EAF). However, computation of the EAF in more than two dimensions is a non-trivial task. In this article, the problem of computing the empirical attainment function is formalised, and upper and lower bounds on the corresponding number of output points are presented. In addition, efficient algorithms for the two and three-dimensional cases are proposed, and their time complexities are related to lower bounds derived for each case.},
              doi        = {10.1007/978-3-642-19893-9_8},
              isbn       = {9783642198939},
              language   = {en},
            }
         * @endcode
         *
         * From code: https://github.com/MLopez-Ibanez/eaf
         * Related publication:
         * @code
            @incollection{LopPaqStu09emaa,
              editor = { Thomas Bartz-Beielstein  and  Marco Chiarandini  and  Lu{\'\i}s Paquete  and  Mike Preuss },
              year = 2010,
              address = {Berlin, Germany},
              publisher = {Springer},
              booktitle = {Experimental Methods for the Analysis of Optimization Algorithms},
              author = { Manuel L{\'o}pez-Ib{\'a}{\~n}ez  and  Lu{\'\i}s Paquete  and  Thomas St{\"u}tzle },
              title = {Exploratory Analysis of Stochastic Local Search Algorithms in Biobjective Optimization},
              pages = {209--222},
              doi = {10.1007/978-3-642-02538-9_9},
              abstract = {This chapter introduces two Perl programs that
                          implement graphical tools for exploring the performance of stochastic local search algorithms
                          for biobjective optimization problems. These tools are based on the concept of the empirical attainment
                          function (EAF), which describes the probabilistic distribution of the outcomes obtained by a
                          stochastic algorithm in the objective space. In particular, we consider the visualization of
                          attainment surfaces and differences between the first-order EAFs of the outcomes of two
                          algorithms. This visualization allows us to identify certain algorithmic behaviors in a graphical way.
                          We explain the use of these visualization tools and illustrate them with examples arising from
                          practice.}
            }
         * @endcode
         *
         * @ingroup EAF
         */
        class Levels {
            protected:
                /** Asked attainment levels indices. */
                std::vector<size_t> _attlevels;

                /** Whether we minimize or maximize. */
                const common::FOptimizationType _f_optim;

                /** Compare two objective function values.
                 * 
                 * Handle maximization or minimizaton problems transparently.
                 * 
                 * @param q1 the considered objective value.
                 * @param q2 the objective function value checked against.
                 * @returns true if q1 is better than q2.
                 */
                bool is_better(const double q1, const double q2)
                {
                    return _f_optim(q1, q2);
                }
                
                /** Compare two objective function values.
                 * 
                 * Handle maximization or minimizaton problems transparently.
                 * 
                 * @param q1 the considered objective value.
                 * @param q2 the objective function value checked against.
                 * @returns true if q1 is better than or equal to q2.
                 */
                bool is_better_or_eq(const double q1, const double q2)
                {
                    return is_better(q1, q2) or q1 == q2;
                }
                
            public:
                //! The type returned by the call interface.
                using Type = std::map<size_t,eaf::Front>;

                /** Constructor.
                 * 
                 * Take the levelsets indices that you want to compute.
                 * There is as much attainment levelsets as there is runs in the logger,
                 * probably with duplicates.
                 * Indices should be in `[0,nb_runs[`.
                 * 
                 * If you pass an empty vector `{}` (the default), it will compute all the levelsets.
                 * 
                 * @param optim_type whether the problem is minimizing or maximizing.
                 * @param attainment_levels Levelsets to be computed (empty=all, the default).
                 */
                Levels(const common::OptimizationType optim_type, std::vector<size_t> attainment_levels = {})
                : _attlevels(attainment_levels)
                , _f_optim{optim_type}
                { }

                /** Extract the runs from the logger and computes the attainment levelsets. */
                Type operator()(const EAF& logger)
                {
                    // Not a perfect guard, because this only tests the last attached problem.
                    // But, there is no way to ensure that all problems seen by the loggers
                    // have been of the same type.
                    // This is at least a check.
                    assert(_f_optim == logger.problem().value().optimization_type.type());
                    
                    // Input:
                    std::vector<eaf::Front> fronts;
                    // Flatten the front in the runs.
                    for(const auto& suite_pb : logger.data()) {
                        for(const auto& pb_dim : suite_pb.second) {
                            for(const auto& dim_ins : pb_dim.second) {
                                for(const auto& ins_runs : dim_ins.second) {
                                    for(const auto& run_front : ins_runs.second) {
                                       fronts.push_back(run_front.second);
                    } } } } }
                    
                    const size_t nb_runs = fronts.size();
                    IOH_DBG(note, nb_runs << " runs in the EAF logger")
                    assert(nb_runs > 0);

                    // Copy and sort data.
                    std::vector<eaf::RunPoint> data_time, data_qual;
                    size_t total_nb_points = 0;
                    for(size_t run = 0; run < fronts.size(); ++run) {
                        assert(fronts[run].size() > 0);
                        // Cumulative size.
                        total_nb_points += fronts[run].size();

                        // Queues: time = x in papers, qual = y.
                        for(const eaf::Point& p : fronts[run]) {
                            // time, qual, id of the related run
                            data_time.push_back(eaf::RunPoint(p.time, p.qual, run));
                            data_qual.push_back(eaf::RunPoint(p.time, p.qual, run));
                        } 
                    }
                    IOH_DBG(note, total_nb_points << " front points in the EAF logger")

                    // Sort the two sets of data.
                    std::sort(std::begin(data_time),std::end(data_time),  ascending_time); // Time always ascend.

                    if(_f_optim == common::OptimizationType::MIN ) {
                        std::sort(std::begin(data_qual),std::end(data_qual), descending_qual); 
                    } else {
                        std::sort(std::begin(data_qual),std::end(data_qual),  ascending_qual); 
                    }

                    // Defaults to all levels asked.
                    if(_attlevels.empty()) {
                        for(size_t r=0; r<nb_runs; ++r) {
                            _attlevels.push_back(r);
                        }
                    }
                    // Output:
                    Type levels;

                    // The algorithm.
                    for(const size_t level : _attlevels) {
                        IOH_DBG(debug, "Parse level " << level)
                        eaf::Front eaf;
                        size_t it = 0; // current time index
                        size_t iq = 0; // current qual index
                        size_t nb_attained = 0;

                        // Can hold negative values.
                        std::vector<long> attained(nb_runs, 0);
                        std::vector<char> attained_runs(nb_runs, 0);

                        // Start at upper-left corner.
                        size_t run = data_time[it].run;
                        assert(run < nb_runs);
                        attained[run]++;
                        nb_attained++;

                        // Result for this level.
                        eaf::Front level_front;

                        do { // while it < total and iq < total
                            IOH_DBG(debug, "> Parse ascending time from " << it << " (time=" << data_time[it].time << ")" )

                            // Until the desired attainment level is reached.
                            while(it < total_nb_points-1 and
                                  (nb_attained < level or data_time[it].time == data_time[it+1].time)) {
                                it++;
                                assert(it < total_nb_points);
                                if(is_better_or_eq(data_time[it].qual, data_qual[iq].qual)) {
                                    run = data_time[it].run;
                                    assert(run < nb_runs);
                                    if(attained[run] == 0) {
                                        nb_attained++; }
                                    attained[run]++;
                                } // it.qual <= iq.qual
                            } // while it < total_nb_points
                            IOH_DBG(debug, "> Reached level at " << it)
                            // IOH_DBG_DUMP(xdebug, attained, "attained_right_{n}.dat");

                            if(nb_attained >= level) {
                                IOH_DBG(debug, ">> Level is to be saved")
                                IOH_DBG(debug, ">> Parse descending qual from " << iq << " (qual=" << data_qual[iq].qual << ")")
                                // Until the desired attainment level is no longer reached.
                                do { // while nb_attained >= level and iq < total

                                    for(size_t r=0; r < nb_runs; ++r) {
                                        attained_runs[r] = attained[r] >= 1 ? 1 : 0;
                                    }
                                    
                                    do { // while iq < total and iq.qual == iq-1.qual
                                        assert(it < total_nb_points);
                                        assert(iq < total_nb_points);
                                        if( data_qual[iq].time <= data_time[it].time) {
                                            run = data_qual[iq].run;
                                            assert(run < nb_runs);
                                            attained[run]--; // Can be negative.
                                            if(attained[run] == 0) {
                                                assert(nb_attained > 0);
                                                nb_attained--; }
                                        } // if iq.time <= it.time
                                        // IOH_DBG_DUMP(xdebug, attained, "attained_down_{n}.dat");
                                        iq++;
                                        assert(iq-1 < total_nb_points);
                                    } while(iq < total_nb_points and data_qual[iq].qual == data_qual[iq-1].qual);
                                } while(nb_attained >= level and iq < total_nb_points);
                                IOH_DBG(debug, ">> Reached level at " << iq)
                                // assert(nb_attained < level);
                                assert(it < total_nb_points);
                                assert(iq-1 < total_nb_points);
                                IOH_DBG(debug, ">> Front level point: (time=" << data_time[it].time << ",qual=" << data_qual[iq-1].qual << ")")
                                level_front.push_back( eaf::RunPoint(
                                        data_time[it].time,
                                        data_qual[iq-1].qual,
                                        std::accumulate( std::begin(attained_runs), std::end(attained_runs), 0)
                                    ));
                            } // if nb_attained

                        } while(it < total_nb_points-1 and iq < total_nb_points);

                        // Save this level.
                        IOH_DBG(note, "> Level " << level << " has " << level_front.size() << " points")
                        assert(level_front.size() > 0);
                        assert(levels.find(level) == std::end(levels));
                        levels[level] = level_front;
#ifndef NDEBUG
                            IOH_DBG(xdebug,">> Involved runs:")
                            IOH_DBG(xdebug,">>> " << level_front[0].run)
                            for(size_t i = 1; i < level_front.size(); ++i) {
                                if(level_front[i].run != level_front[i-1].run) {
                                    IOH_DBG(xdebug,">>> " << level_front[i].run)
                                }
                                assert(level_front[i].run == level_front[i-1].run);
                            }
#endif
                    } // l in levels

                    IOH_DBG(note, "Ended with " << levels.size() << " levels")
                    assert(levels.size() > 0);
                    assert(levels.size() <= nb_runs);
                    return levels;
                }

        }; // class Levels

        /** Computes attainment levels of the Empirical Attainment Function.
         * 
         * @param logger the logger holding the data.
         * @param levels The desired level indices (empty=all, the default).
         * @returns A map associating a level index to the corresponding set of non-dominated quality/time points.
         * 
         * @see Levels for details.
         * 
         * @ingroup EAF
         */
        inline Levels::Type levels(const EAF& logger, std::vector<size_t> levels = {} )
        {
            Levels levels_of(logger.optimization_type(), levels);
            return levels_of(logger);
        }

        /** Interface for classes that computes statistics over an eaf::Levels' data.
         * 
         * @ingroup EAF_API
         */
        template<class T>
        class Stat {
            public:
                //! Type of the statistic.
                using Type = T;

                //! Main call interface.
                virtual T operator()(const eaf::Levels::Type& levels) = 0;
        };

        /** Interface for statistics over an EAF, which depend on a nadir reference point.
         * 
         * Either the desired nadir point is given, either it is automatically computed as
         * the extremum of all the levels, at the beginning of the call.
         * 
         * @note Letting the class computes its nadir point will be more expensive than
         *       using an extremum that you would know from bounds.
         * 
         * @warning This should always compute a metric that should be maximized in order to improve the performance.
         *          (i.e. *under* the curve for maximization problems, *above* the curve for minimization problems)
         * 
         * @ingroup EAF_API
         */
        template<class T>
        class NadirStat : public Stat<T>
        {
            protected:
                //! Initialization check flag.
                bool _has_nadir;

                //! The worst reference point.
                eaf::Point _nadir;

                //! The problem type (i.e. min or max).
                const common::FOptimizationType _is_better;

                //! Shift of the nadir point regarding the extremum of data.
                static const size_t _shift_time = 1;
                const double _shift_qual;

                /** Computes the extremum point as the worst coordinates on both quality and time axis for all front points.
                 */
                eaf::Point extremum(const eaf::Front& front) const
                {
                    // size_t worst_time;
                    // double worst_qual;
                    // if(_is_better.type() == common::OptimizationType::MIN) {
                    //     worst_time =  std::numeric_limits<size_t>::lowest();
                    //     worst_qual = -std::numeric_limits<double>::infinity();
                    // } else {
                    //     worst_time =  std::numeric_limits<size_t>::infinity();
                    //     worst_qual =  std::numeric_limits<double>::infinity();
                    // }
                    // for(const auto& p : front) {
                    //     if(p.time > worst_time) { worst_time = p.time; }
                    //     if(_is_better(worst_qual, p.qual)) { worst_qual = p.qual; }
                    // }
                    // return eaf::Point(worst_qual, worst_time);
                    double nadir_qual;
                    size_t nadir_time = std::numeric_limits<size_t>::lowest();

                    if(_is_better.type() == common::OptimizationType::MIN) {
                        nadir_qual = -std::numeric_limits<double>::infinity();
                    } else {
                        nadir_qual =  std::numeric_limits<double>::infinity();
                    }
                    for(const auto& candidate_nadir : front) {
                        if(candidate_nadir.time > nadir_time) { nadir_time = candidate_nadir.time; }
                        if(_is_better(nadir_qual, candidate_nadir.qual)) { nadir_qual = candidate_nadir.qual; }
                    }
                    return eaf::Point(nadir_qual, nadir_time);
                }

                /** Main interface.
                 * 
                 * This is called by the upper Stat::operator() interface, after having updated the nadir point.
                 */
                virtual T call(const eaf::Levels::Type& levels) = 0;

            public:
                //! The type returned by the call interface.
                using Type = typename Stat<T>::Type;
                
                /** Constructor without a nadir reference point. */
                NadirStat(const common::OptimizationType optim_type, const double shift_qual = 1e-6) : _has_nadir(false), _nadir(), _is_better{optim_type}, _shift_qual(shift_qual) {
                    IOH_DBG(note, "NadirStat without a nadir");
                }

                /** Constructor with a nadir reference point.
                 *
                 * @warning Using this constructor, you are responsible of passing the correct nadir point.
                 * 
                 * @warning Note that you must shift it to ensure that the statistic is always non-zero.
                 */
                NadirStat(const common::OptimizationType optim_type, eaf::Point nadir) : _has_nadir(true), _nadir(nadir), _is_better{optim_type},
                    _shift_qual(std::numeric_limits<double>::signaling_NaN()) { }

                /** Update the nadir point (if not previously indicated) and then calls NadirStat::call. */
                T operator()(const eaf::Levels::Type& levels) override
                {
                    if(not _has_nadir) {
                        double nadir_qual;
                        double epsil_qual;
                        size_t nadir_time = 0;
                        const size_t epsil_time = 1;

                        if(_is_better.type() == common::OptimizationType::MIN) {
                            nadir_qual = -std::numeric_limits<double>::infinity();
                            // epsil_qual =  1;//std::numeric_limits<double>::epsilon();
                            epsil_qual =  _shift_qual;
                        } else {
                            nadir_qual =  std::numeric_limits<double>::infinity();
                            epsil_qual = -1;//std::numeric_limits<double>::epsilon();
                            epsil_qual = -_shift_qual;
                        }
                        for(const auto& [_,front] : levels) {
                            eaf::Point candidate_nadir = extremum(front);
                            if(candidate_nadir.time > nadir_time) { nadir_time = candidate_nadir.time; }
                            if(_is_better(nadir_qual, candidate_nadir.qual)) { nadir_qual = candidate_nadir.qual; }
                        }
                        IOH_DBG(debug,"Estimated nadir: (time=" << nadir_time+epsil_time << ",qual=" << nadir_qual+epsil_qual << ")" );
                        _nadir = eaf::Point(nadir_qual+epsil_qual, nadir_time+epsil_time);
                        _has_nadir = true;
                    }

                    return call(levels);
                };

        };

        /** Classes that computes statistics over the eaf::Levels::Type data. */
        namespace stat {

            /** Computes all the surfaces of the given attainment levelsets.
             * 
             * This computes the surface encompassed by each levelset, up to the extremum point,
             * and returns a map between the level index and of the corresponding surfaces.
             * 
             * @warning This will always compute a metric that should be maximized in order to improve the performance.
             *          (i.e. *under* the curve for maximization problems, *above* the curve for minimization problems)
             * 
             * @ingroup EAF
             */
            class Surfaces : public NadirStat<std::map<size_t,double>>
            {
                protected:
                    /** The NadirStat interface. */
                    Type call(const eaf::Levels::Type& levels) override
                    {
                        Type results;
                        for(auto [level,front] : levels) {
                            results[level] = surface(front);
                        }
                        IOH_DBG(debug,"Surfaces of " << levels.size() << " levels");
                        assert(results.size() == levels.size());
                        return results;
                    }
                    
                public:
                    /** Constructor without a nadir reference point. */
                    Surfaces(const common::OptimizationType optim_type) : NadirStat(optim_type) { }

                    /** Constructor with a nadir reference point. */
                    Surfaces(const common::OptimizationType optim_type, eaf::Point nadir) : NadirStat(optim_type, nadir) { }

                    // This is public so as to allow for single loop computations in Volume::call.
                    /** Computes the surface of the given non-dominated set of points.
                     * 
                     * @warning You need to have set a nadir point before calling this low-level function.
                     */
                    double surface(eaf::Front front) const
                    {
                        IOH_DBG(xdebug,"Front of size: " << front.size());
                        assert(_has_nadir);
                        double n_qual = _nadir.qual;
                        double surface = 0;

                        if(this->_is_better.type() == common::OptimizationType::MIN) {
                            std::sort(std::begin(front), std::end(front), eaf::descending_qual);
                        } else {
                            std::sort(std::begin(front), std::end(front), eaf::ascending_qual);
                        }
                        
                        for(const auto& p : front) {
                            surface +=   std::fabs(n_qual - p.qual) 
                                       * std::labs(static_cast<long>(_nadir.time) - static_cast<long>(p.time)); // cast because time are unsigned
                            n_qual = p.qual;
                            assert(surface > 0);
                        }
                        IOH_DBG(xdebug, "Surface of front = " << surface);
                        return surface;
                    }
            };

            /** Computes the "hypervolumes" of the given attainment levelsets.
             * 
             * This computes the surfaces encompassed by each levelset, up to the extremum point,
             * and returns a map between the level index and of the corresponding surfaces.
             * 
             * @warning The nadir reference point is computed automatically as the extremum of all the levels.
             *          Thus, if you call this function several times on different levels,
             *          the surfaces will (may) be computed with different nadir points.
             *          To indicate your own nadir point, use the stat::Surfaces class directly.
             * 
             * @warning This will always compute a metric that should be maximized in order to improve the performance.
             *          (i.e. *under* the curve for maximization problems, *above* the curve for minimization problems)
             * 
             * @ingroup EAF
             */
            inline Surfaces::Type surfaces(const common::OptimizationType optim_type, const eaf::Levels::Type& levels)
            {
                Surfaces surfaces_of(optim_type);
                return surfaces_of(levels);
            }
                                  
            /** Computes the volume of the whole empirical attainement function.
             * 
             * This computes the surface encompassed by each levelset of the given EAF (up to a reference point),
             * and then sum the volume between those surfaces.
             * 
             * @note If only a subset of the levelsets are given, the true volume will be underestimated.
             * 
             * @warning This will always compute a metric that should be maximized in order to improve the performance.
             *          (i.e. *under* the curve for maximization problems, *above* the curve for minimization problems)
             * 
             * @ingroup EAF
             */
            class Volume : public NadirStat<double>
            {
                protected:

                    double call(const eaf::Levels::Type& levels) override
                    {
                        IOH_DBG(debug, "Volume of " << levels.size() << " levels");
                        Surfaces s(this->_is_better.type(), _nadir);
                        // Do not compute surfaces right here to avoid two loops on levels instead of one.

                        double volume = 0;
                        auto prev_level = -1;
                        for(const auto& [level,front] : levels) {
                            const auto fsurf = s.surface(front);
#ifndef NDEBUG
                                if(fsurf == 0) {
                                    IOH_DBG(warning, "Level " << level << " has a null surface.");
                                }
#endif
                            volume += fsurf * (static_cast<double>(level) - prev_level);
                            prev_level = static_cast<int>(level);
                        }
                        IOH_DBG(debug, "Volume = " << volume);
#ifndef NDEBUG
                            if(volume == 0) {
                                IOH_DBG(warning, "Null volume, if some front have single points, set a nadir point.");
                            }
#endif
                        return volume;
                    }
                public:
                    /** Constructor without a nadir reference point. */
                    Volume(const common::OptimizationType optim_type) : NadirStat(optim_type) { }

                    /** Constructor with a nadir reference point. */
                    Volume(const common::OptimizationType optim_type, eaf::Point nadir) : NadirStat(optim_type, nadir) { }

            }; // class Volume

            /** Computes the absolute "hypervolume" of the whole empirical attainement function.
             *
             * @warning The nadir reference point is computed automatically as the extremum of all the levels.
             *          Thus, if you call this function several times on different levels,
             *          the surfaces will (may) be computed with different nadir points.
             *          To indicate your own nadir point, use the stat::Volume class directly.
             * 
             * @warning This will always compute a metric that should be maximized in order to improve the performance.
             *          (i.e. *under* the curve for maximization problems, *above* the curve for minimization problems)
             * 
             * @ingroup EAF
             */
            inline double volume(const common::OptimizationType optim_type, const eaf::Levels::Type& levels)
            {
                Volume volume_of(optim_type);
                return volume_of(levels);
            }

            /** Computes the normalized "hypervolume" of the whole empirical attainment function.
             * 
             * If you don't pass the number of runs, it will be guessed by looking at
             * the number of runs in the very first logs.
             * 
             * @warning This will compute a metric that should be maximized in order to improve the performance.
             *          (i.e. *under* the curve for maximization problems, *above* the curve for minimization problems)
             * 
             * @ingroup EAF
             */
            inline double volume(const EAF& logger, double error_min, double error_max, size_t evals_min, size_t evals_max, size_t nb_runs = 0)
            {
                assert(error_max > error_min);
                assert(evals_max > evals_min);

                // Prepare the instance with the Nadir point.
                std::unique_ptr<Volume> volume_of = nullptr;
                if(logger.optimization_type() == common::OptimizationType::MIN) {
                    volume_of = std::make_unique<Volume>(logger.optimization_type(), Point(error_max, evals_max));
                } else {
                    volume_of = std::make_unique<Volume>(logger.optimization_type(), Point(error_min, evals_max));
                }

                // Computes all levels.
                auto v = (*volume_of)(levels(logger, {}));

                // Get the number of runs from the first instance.
                // WARNING: this assume that all instances have the same number of runs,
                // an assumption that is not enforced in the logger.
                if(nb_runs == 0) {
                    const auto suite = logger.data().begin();
                    assert(suite != logger.data().end());
                    const auto pb = suite->second.begin();
                    assert(pb != suite->second.end());
                    const auto dim = pb->second.begin();
                    assert(dim != pb->second.end());
                    const auto ins = dim->second.begin();
                    assert(ins != dim->second.end());
                    nb_runs = ins->second.size();
#ifndef NDEBUG
                    // Double check that all instances have that many runs
                    for(const auto& suite_pb : logger.data()) {
                        for(const auto& pb_dim : suite_pb.second) {
                            for(const auto& dim_ins : pb_dim.second) {
                                for(const auto& ins_runs : dim_ins.second) {
                                    for(const auto& run_front : ins_runs.second) {
                                       assert(run_front.second.size() == nb_runs);
                    } } } } }
#endif

                }
                assert(nb_runs > 0);
                return v / (nb_runs * (error_max-error_min) * (evals_max-evals_min));
            }

            /** Computes the absolute "hypervolume" of the whole empirical attainement function.
             *
             * @warning The nadir reference point is computed automatically as the extremum of all the levels.
             *          Thus, if you call this function several times on different levels,
             *          the surfaces will (may) be computed with different nadir points.
             *          To indicate your own nadir point, use the stat::Volume class directly.
             * 
             * @warning This will always compute a metric that should be maximized in order to improve the performance.
             *          (i.e. *under* the curve for maximization problems, *above* the curve for minimization problems)
             * 
             * @ingroup EAF
             */
            inline double volume(const EAF& logger)
            {
                return volume(logger.optimization_type(), levels(logger));
            }
            
        } // stat
    } // eaf
} // logger
} // ioh
