#pragma once

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#if defined(__GNUC__) || defined(__GNUG__)
#include <cxxabi.h>
#endif

#include "container_utils.hpp"
#include "file.hpp"


namespace ioh::problem
{
    //! Definition interface of problems that are defined in static files
    struct InstanceBasedProblem
    {
        /**
         * @brief A problem class with multiple instances defined, where each
         * instance is a single unique problem, with its own dimension etc. Every fid has only
         * 1 instance, and only one dimension.
         *
         */
        //! A constructor function
        template <typename T, typename... Args>
        using Constructor = std::function<T(Args &&...)>;

        //! Shorthand for tuple used in constructor list
        template <typename T, typename... Args>
        using ConstructorTuple = std::tuple<Constructor<T, Args...>, int, std::optional<std::string>>;


        //! A vector of constructors-id pairs
        template <typename T, typename... Args>
        using Constructors = std::vector<ConstructorTuple<T, Args...>>;

        /**
         * @brief Method to load instances
         *
         * @tparam T The type of the problem
         * @tparam Args the arguments of the constructor
         * @param definitions the file with instance defintions
         * @return Constructors<T, Args...> a vector of constrcutor functions
         */
        template <typename T, typename... Args>
        static Constructors<T, Args...> load_instances(const std::optional<fs::path> &definitions = std::nullopt)
        {
            return {}; // Return empty vector by default
        }
    };

    // The types whose factories have built-in entries (see add_builtins below)
    template <typename T>
    class SingleObjectiveProblem;
    class BBOB;
    struct SBOX;
    class PBO;
    class CEC2013;
    class CEC2022;
    class DynamicBinVal;
    namespace submodular
    {
        struct GraphProblem;
    }
    namespace star_discrepancy::real
    {
        class StarDiscrepancy;
    }
    namespace star_discrepancy::integer
    {
        class StarDiscrepancy;
    }
} // namespace ioh::problem

namespace ioh::suite
{
    template <typename ProblemType>
    class Suite;
}

namespace ioh::common
{
    //! Function to get the next non zero value in an array of integers
    inline int get_next_id(const std::vector<int> &ids)
    {
        return ids.empty() ? 1 : (*std::max_element(ids.begin(), ids.end())) + 1;
    }

    /**
     * \brief Get the type name of a given template T
     * \tparam T a type
     * \return the name of T
     */
    template <typename T>
    std::string type_name()
    {
        std::string name = typeid(T).name();
#if defined(__GNUC__) || defined(__GNUG__)
        int status;
        auto demangled_name = abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status);
        if (status == 0)
        {
            name = demangled_name;
            std::free(demangled_name);
        }
#endif
        return name;
    }

   
    /**
     * \brief Get the type name of the problem
     * \tparam T the template of the Problem
     * \return the name of T
     */
    template <typename T>
    std::string class_name()
    {
        auto name = type_name<T>();
        name = name.substr(name.find_last_of(' ') + 1);
        name = name.substr(name.find_last_of("::") + 1);
        name = name.substr(0, name.find_first_of(">"));
        return name;
    }

    template <class AbstractType, typename... Args>
    struct Factory;

    //! Adds the built-in entries of a factory; factories of other types start empty
    template <class AbstractType, typename... Args>
    void add_builtins(Factory<AbstractType, Args...> &)
    {
    }

    /**
     * @name Built-in entries
     * Defined in the compiled library (src/registry), so that only the registry translation units
     * instantiate the problem classes. Without the library these are link errors, never empty factories.
     */
    ///@{
    void add_builtins(Factory<problem::SingleObjectiveProblem<double>, int, int> &);
    void add_builtins(Factory<problem::SingleObjectiveProblem<int>, int, int> &);
    void add_builtins(Factory<problem::BBOB, int, int> &);
    void add_builtins(Factory<problem::SBOX, int, int> &);
    void add_builtins(Factory<problem::PBO, int, int> &);
    void add_builtins(Factory<problem::CEC2013, int, int> &);
    void add_builtins(Factory<problem::CEC2022, int, int> &);
    void add_builtins(Factory<problem::DynamicBinVal, int, int> &);
    void add_builtins(Factory<problem::submodular::GraphProblem, int, int> &);
    void add_builtins(Factory<problem::star_discrepancy::real::StarDiscrepancy, int, int> &);
    void add_builtins(Factory<problem::star_discrepancy::integer::StarDiscrepancy, int, int> &);
    void add_builtins(Factory<suite::Suite<problem::SingleObjectiveProblem<double>>, std::vector<int>,
                              std::vector<int>, std::vector<int>> &);
    void add_builtins(Factory<suite::Suite<problem::SingleObjectiveProblem<int>>, std::vector<int>,
                              std::vector<int>, std::vector<int>> &);
    ///@}

    /**
     * @brief Class for generating classes for a given abtract type
     *
     * @tparam AbstractType the parent type of the classes to be generator
     * @tparam Args the arguments of the common constructor for children of AbtractType
     */
    template <class AbstractType, typename... Args>
    struct Factory
    {
        //! A shared ptr to AbstractType
        using Type = std::shared_ptr<AbstractType>;

        //! Creator method for instances of Type
        using Creator = std::function<Type(Args &&...)>;

        //! Accessor to static instance; the first call adds the built-in entries
        static Factory &instance()
        {
            static Factory f{populate_tag{}}; // NOLINT
            return f;
        }

        /**
         * @brief Include a new element to the factory
         *
         * A new name gets `id`, or the next free id when `id` is taken. An existing name keeps its id and
         * gets the new creator. Built-in entries must be unique: a duplicate name or id among them throws.
         */
        void include(const std::string &name, const int id, Creator creator)
        {
            const std::lock_guard<std::recursive_mutex> lock(mutex_);
            const auto already_defined = name_map.find(name) != std::end(name_map);
            if (adding_builtins_ && (already_defined || id_map.find(id) != std::end(id_map)))
                throw std::logic_error("Duplicate built-in factory entry: " + name + " (id " + std::to_string(id) +
                                       ")");
            name_map[name] = std::move(creator);
            if (!already_defined)
                id_map[check_or_get_next_available(id)] = name;
        }

        //! Get the next available id
        [[nodiscard]] int check_or_get_next_available(const int id) const
        {
            const auto known_ids = ids();
            const auto it = std::find(known_ids.begin(), known_ids.end(), id);
            return it == known_ids.end() ? id : get_next_id(known_ids);
        }

        //! Get the next available id
        [[nodiscard]] int check_or_get_next_available(const int id, const std::string &name) const
        {
            const std::lock_guard<std::recursive_mutex> lock(mutex_);
            const auto already_defined = name_map.find(name) != std::end(name_map);
            if (already_defined)
                for (const auto &kv : id_map)
                    if (kv.second == name)
                        return kv.first;
            return check_or_get_next_available(id);
        }

        //! Accessor for the list of registered names
        [[nodiscard]] std::vector<std::string> names() const
        {
            const std::lock_guard<std::recursive_mutex> lock(mutex_);
            return common::keys(name_map);
        }

        //! Accessor for the list of registered ids
        [[nodiscard]] std::vector<int> ids() const
        {
            const std::lock_guard<std::recursive_mutex> lock(mutex_);
            return common::keys(id_map);
        }

        //! Accessor for the map from id to name
        [[nodiscard]] std::map<int, std::string> map() const
        {
            const std::lock_guard<std::recursive_mutex> lock(mutex_);
            return id_map;
        }

        //! Create a new instance of Type using the Creator registered; throws std::out_of_range for unknown names
        [[nodiscard]] Type create(const std::string &name, Args... params) const
        {
            Creator creator;
            {
                const std::lock_guard<std::recursive_mutex> lock(mutex_);
                const auto entry = name_map.find(name);
                if (entry == std::end(name_map))
                    throw std::out_of_range("No factory entry named " + name);
                creator = entry->second;
            }
            // outside the lock: a creator may use other factories
            return creator(std::forward<Args>(params)...);
        }

        //! Create a new instance of Type using the Creator registered; throws std::out_of_range for unknown ids
        [[nodiscard]] Type create(const int id, Args... params) const
        {
            std::string name;
            {
                const std::lock_guard<std::recursive_mutex> lock(mutex_);
                const auto entry = id_map.find(id);
                if (entry == std::end(id_map))
                    throw std::out_of_range("No factory entry with id " + std::to_string(id));
                name = entry->second;
            }
            return create(name, std::forward<Args>(params)...);
        }

    private:
        struct populate_tag
        {
        };

        //! The built-in entries are added before anything else can register, so their ids are deterministic.
        //! add_builtins must not call instance() of the factory it populates.
        explicit Factory(populate_tag)
        {
            adding_builtins_ = true;
            add_builtins(*this);
            adding_builtins_ = false;
        }
        Factory(const Factory &) = delete;
        Factory &operator=(const Factory &) = delete;

        std::map<std::string, Creator> name_map;
        std::map<int, std::string> id_map;
        bool adding_builtins_ = false;
        mutable std::recursive_mutex mutex_;
    };

    struct FactoryID {
        int id;
        std::string name;
        
    };

    //! Helper to get a new ID for a given class
    template <bool IsProblem>
    struct IdGetter
    {
    };

    //! Helper to get a new ID for a problem class
    template <>
    struct IdGetter<true>
    {
        //! Gets an from the class meta_data
        template <typename T>
        static FactoryID get_id(const std::vector<int> &)
        {
            const auto meta = T(1, 1).meta_data();
            return {meta.problem_id, meta.name};
        }
    };

    //! Helper to get a new ID for a non problem class
    template <>
    struct IdGetter<false>
    {
        //! Gets an id
        template <typename T>
        static FactoryID get_id(const std::vector<int> &ids)
        {
            return {get_next_id(ids), class_name<T>()};
        }
    };    

    /**
     * @brief Base factory registry
     *
     * @tparam Parent Abstract type
     * @tparam Args Parent constructor args
     */
    template <typename Parent, typename... Args>
    struct RegisterWithFactory
    {
        //! Typedef for instance based problems
        using InstanceBasedProblem = ioh::problem::InstanceBasedProblem;

        //! Include T in the factory
        template <class T>
        static void include()
        {
            include<T>(instance());
        }

        //! Include T in the given factory (the built-in entries use this while their factory is created)
        template <class T>
        static typename std::enable_if<!std::is_base_of<InstanceBasedProblem, T>::value, void>::type include(
            Factory<Parent, Args...> &factory)
        {
            const auto is_problem_type = std::conjunction<std::is_same<int, Args>...>::value;
            const auto meta = IdGetter<is_problem_type>::template get_id<T>(factory.ids());

            factory.include(meta.name, meta.id,
                            [](Args &&...params) { return std::make_unique<T>(std::forward<Args>(params)...); });
        }

        //! Include specific for instance based problems
        template <class T>
        static typename std::enable_if<std::is_base_of<InstanceBasedProblem, T>::value, void>::type include(
            Factory<Parent, Args...> &factory)
        {
            auto constructors = InstanceBasedProblem::load_instances<T, Args...>();

            for (auto &ci : constructors)
            {
                const auto id = std::get<1>(ci);
                const auto name = std::get<2>(ci).value_or(fmt::format("{}{}", class_name<T>(), id));

                factory.include(name, id, [c = std::get<0>(ci)](Args &&...params) {
                    return std::make_unique<T>(c(std::forward<Args>(params)...));
                });
            }
        }

        //! Accessor to static instance
        static Factory<Parent, Args...> &instance() { return Factory<Parent, Args...>::instance(); }
    };


    // Automatic registration at static initialization. The library's own problems and suites no longer use
    // it (every translation unit paid for registering all of them); it remains for user-defined types, see
    // problem::RegisteredProblem.

    //! Helper for factory system
    template <class Type, class Factory>
    struct InvokeApplyOnConstruction
    {
        InvokeApplyOnConstruction() { Factory::template include<Type>(); }
    };

    //! Helper for factory system
    template <class Type, class Factory>
    struct RegistrationInvoker
    {
        //! Forces creation of `registration_invoker`
        static inline InvokeApplyOnConstruction<Type, Factory> registration_invoker =
            InvokeApplyOnConstruction<Type, Factory>();
    };

    //! Helper for factory system
    template <class Type, class Factory>
    struct AutomaticTypeRegistration : RegistrationInvoker<Type, Factory>
    {
        //! Forces creation of `invoker`
        InvokeApplyOnConstruction<Type, Factory> &invoker = RegistrationInvoker<Type, Factory>::registration_invoker;
    };
} // namespace ioh::common
