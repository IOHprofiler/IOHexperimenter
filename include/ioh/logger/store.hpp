#pragma once

#include <map>
#include <optional>

#include <ioh/common/log.hpp>
#include "loggers.hpp"

namespace ioh::logger {
    
    /** A logger that stores all the possible information in-memory, in nested maps.
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
     *
     * @ingroup Loggers
     */
    class Store : public Watcher {
        public:
            /** @name Data structure types
             * Convenience naming for the underlying nested data structure.
             *
             * @{ */

            //! value
            using Value      = std::optional<double>;
            //!  name        => value
            using Attributes = std::map<std::string,Value     >; 
            //!  evaluations => Attributes
            using Run        = std::map<size_t     ,Attributes>; 
            //!  run id      => Run
            using Runs       = std::map<size_t     ,Run       >; 
            //!  instance id => runs
            using Instances  = std::map<int        ,Runs      >; 
            //! nb of dim   => instances
            using Dimensions = std::map<int        ,Instances >; 
            //! pb id       => dimension
            using Problems   = std::map<int        ,Dimensions>; 
            //! suite name  => problems
            using Suites     = std::map<std::string,Problems  >; 
            /** @} */

            /** When attached directly to a Problem (out of a Suites), use the following key for the Suites map. */
            inline static const std::string default_suite = "None";

            /** Direct accessor to the data structure. */
            Suites data() {return _data;}

            /** A set of keys leading to a set of property values within the data structure. */
            struct Cursor {
                //! Suite name
                std::string suite;
                //! problem id
                int pb;
                //! Dimension
                int dim;
                //! Problem instance
                int instance;
                //! run id
                size_t run;
                //! evaluation number
                size_t evaluation;
                /**
                 * @brief Construct a new Cursor object
                 * 
                 * @param suite_name Suite name
                 * @param problem_id problem id
                 * @param dimension Dimension
                 * @param instance_id Problem instance
                 * @param run_id run id
                 * @param evaluation_id evaluation number
                 */
                Cursor(const std::string& suite_name = default_suite,
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

            /** Access a map of property values with a Cursor. */
            Attributes data(const Cursor& current)
            {
                return _data.at(current.suite).at(current.pb).at(current.dim).at(current.instance).at(current.run).at(current.evaluation);
            }

            /** Access a property value with a Cursor and the property name. */
            Value at(const Cursor& current, const std::string& property_name)
            {
                return data(current).at(property_name);
            }

            /** Access a property value with a Cursor and the Property itself. */
            Value at(const Cursor& current, const Property& property)
            {
                return data(current).at(property.name());
            }
            
        protected:
            /** The main data structure in which log events are stored. */
            Suites _data;

            /** The current Cursor. */
            Cursor _current;

            /** Accessor to the current attributes map. */
            Attributes& current_attributes()
            {
                return _data[_current.suite][_current.pb][_current.dim][_current.instance][_current.run][_current.evaluation];
            }
            
        public:
            /** The logger::Store should at least track one logger::Property, or else it makes no sense to use it. */
            Store(std::vector<std::reference_wrapper<logger::Trigger >> triggers,
                  std::vector<std::reference_wrapper<logger::Property>> Attributes)
            : Watcher(triggers, Attributes)
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
            virtual void call(const logger::Info& log_info) override
            {
                // Get the Attributes list at the current cursor.
                Attributes& att = current_attributes();
                // Save the corresponding values.
                for(const auto& rwp : this->properties_) {
                    att[rwp.first] = rwp.second.get()(log_info);
                }
                // Jump to next cursor.
                _current.evaluation++;
            }
    };

    /** Default logger is the in-memory one.
     *
     * @ingroup Logging
     */
    using Default = Store;

} // ioh::logger
