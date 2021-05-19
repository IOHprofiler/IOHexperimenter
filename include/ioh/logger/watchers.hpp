#pragma once

#include <map>
#include <optional>

#include "api.hpp"

namespace ioh::logger {
    
    /** A Logger/Watcher that stores all the possible information in-memory, in nested maps.
     * 
     * The order of the entry keys is:
     *  suite_name > problem_id > n_variables > instance > run_id > evaluation > property_name > property_value
     * In JSON-like format:
     * {suite_name: {problem_id: {n_variables: {instance: {run_id: {evaluation: {property_name : property_value}}}}}}}
     * 
     * You would access a given property value like:
     * @code
            logger::Store store({log::TransformedY()});
            // [Attach to a problem and run...]

            auto prop = store.data["None"][1][10][2][0][1234]["transformed_y"];
            // Or alternatively:
            logger::Store::Cursor cur("None",1,10,2,0,1234);
            const std::string prop_name = TransformedY().name();
            logger::Store::Value prop = store.data(cur)[prop_name];

            // Then test if the property has a value:
            if(prop) {
                std::cout << prop_name << ": " << prop << std::endl;
            } else {
                std::cout << prop_name << " does not exists in this context" << std::endl;
            }
     * @endcode
     * 
     * @note If track_suite has never been called, the default suite name is Store::default_suite ("None");
     */
    class Store : public Watcher {
        public:
            using Value      = std::optional<double>;
            using Properties = std::map<std::string,Value     >; // name        => value
            using Run        = std::map<size_t     ,Properties>; // evaluations => properties
            using Runs       = std::map<size_t     ,Run       >; // run id      => Run
            using Instances  = std::map<int        ,Runs      >; // instance id => runs
            using Dimension  = std::map<int        ,Instances >; // nb of dim   => instances
            using Problems   = std::map<int        ,Dimension >; // pb id       => dimension
            using Suite      = std::map<std::string,Problems  >; // suite name  => problems

            inline static const std::string default_suite = "None";

            Suite data() {return _data;}
            
            struct Cursor {
                std::string suite;
                int pb;
                int dim;
                int instance;
                size_t run;
                size_t evaluation;
                Cursor(std::string suite_name = default_suite,
                        int problem_id = 0,
                        int dimension = 0,
                        int instance_id = 0,
                        size_t run_id = 0,
                        size_t evaluation_id = 0)
                : suite(suite_name),
                  pb(problem_id),
                  dim(dimension),
                  instance(instance_id),
                  run(run_id),
                  evaluation(evaluation_id)
                { }
            };

            Properties data(const Cursor current)
            {
                return _data.at(current.suite).at(current.pb).at(current.dim).at(current.instance).at(current.run).at(current.evaluation);
            }

            Value at(const Cursor current, const std::string property_name)
            {
                return data(current).at(property_name);
            }

            Value at(const Cursor current, const Property& property)
            {
                return data(current).at(property.name());
            }
            
        protected:
            Suite _data;
            Cursor _current;

            Properties& current_properties()
            {
                return _data[_current.suite][_current.pb][_current.dim][_current.instance][_current.run][_current.evaluation];
            }
            
        public:
            /** The logger::Store should at least track one logger::Property, or else it makes no sense to use it. */
            Store(std::initializer_list<std::reference_wrapper<logger::Trigger >> triggers,
                  std::initializer_list<std::reference_wrapper<logger::Property>> properties)
            : Watcher(triggers, properties)
            { }

            /** Track a problem/instance/dimension and/or create a new run.
             * 
             * Problem, instance and dimension are given in the argument.
             * Run and evaluations number are managed internally.
             * 
             * @note A new run is created everytime this is called.
             */
            virtual void attach_problem(const problem::MetaData& problem) override
            {
                Logger::attach_problem(problem);
                
                _current.pb         = problem.problem_id;
                _current.dim        = problem.n_variables;
                _current.instance   = problem.instance;
                
                _current.evaluation = 0;
                const Runs& runs    = _data[_current.suite][_current.pb][_current.dim][_current.instance];
                _current.run        = runs.size(); // De facto next run id.
            }

            /** Set the current suite name.
             * 
             * @note If this is never called, the suite name defaults to Store::default_suite ("None").
             */
            virtual void attach_suite(const std::string& suite_name) override
            {
                _current.suite = suite_name;
            }

            /** Atomic log action. */
            virtual void call(const log::Info& log_info) override
            {
                Properties& att = current_properties();
                for(const auto& rwp : this->_properties) {
                    att[rwp.get().name()] = rwp.get()(log_info);
                }
                _current.evaluation++;
            }
    };

    /** Default logger is the in-memory one. */
    using Default = Store;

} // ioh::logger
