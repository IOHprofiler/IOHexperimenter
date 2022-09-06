#pragma once

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <optional>

#if defined(__GNUC__) || defined(__GNUG__)
#include <cxxabi.h>
#endif

#include "container_utils.hpp"
#include "file.hpp"


namespace ioh::problem
{
    struct InstanceBasedProblem
    {
        /**
         * @brief A problem class with multiple instances defined, where each
         * instance is a single unique problem, with its own dimension etc. Every fid has only
         * 1 instance, and only one dimension.
         * 
         */
        
        template <typename T, typename... Args>
        using Constructor = std::function<T(Args &&...)>;

        template <typename T, typename... Args>
        using Constructors = std::vector<std::pair<Constructor<T, Args...>, int>>;


        template <typename T, typename... Args>
        static Constructors<T, Args...> load_instances(const std::optional<fs::path>& definitions = std::nullopt)
        {
            return {}; // Return empty vector by default
        }
    };
} // namespace ioh::problem

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
        return name = name.substr(0, name.find_first_of(">"));
    }

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

        //! Accessor to static instance
        static Factory &instance()
        {
            static Factory f; // NOLINT
            return f;
        }

        //! Include a new element to the factory
        void include(const std::string &name, const int id, Creator creator)
        {
            const auto already_defined = name_map.find(name) != std::end(name_map);
            assert(!already_defined && name.c_str());
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
            const auto already_defined = name_map.find(name) != std::end(name_map);
            if (already_defined)
                for (const auto& kv : id_map)
                    if (kv.second == name)
                        return kv.first;
            return check_or_get_next_available(id);
        }

        //! Accessor for the list of registered names
        [[nodiscard]] std::vector<std::string> names() const { return common::keys(name_map); }

        //! Accessor for the list of registered ids
        [[nodiscard]] std::vector<int> ids() const { return common::keys(id_map); }

        //! Accessor for the map from id to name
        [[nodiscard]] std::map<int, std::string> map() const { return id_map; }


        //! Create a new instance of Type using the Creator registered
        [[nodiscard]] Type create(const std::string &name, Args... params) const
        {
            const auto entry = name_map.find(name);
            assert(entry != std::end(name_map));
            return entry->second(std::forward<Args>(params)...);
        }

        //! Create a new instance of Type using the Creator registered
        [[nodiscard]] Type create(const int id, Args... params) const
        {
            const auto entry = id_map.find(id);
            assert(entry != std::end(id_map));
            return create(entry->second, std::forward<Args>(params)...);
        }

    private:
        Factory() = default;
        Factory(const Factory &) = delete;
        std::map<std::string, Creator> name_map;
        std::map<int, std::string> id_map;
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
        static int get_id(const std::vector<int> &)
        {
            return T(1, 1).meta_data().problem_id;
        }
    };

    //! Helper to get a new ID for a non problem class
    template <>
    struct IdGetter<false>
    {
        //! Gets an id
        template <typename T>
        static int get_id(const std::vector<int> &ids)
        {
            return get_next_id(ids);
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
        using InstanceBasedProblem = ioh::problem::InstanceBasedProblem;

        //! Include Parent in the factory
        template <class T>
        static typename std::enable_if<!std::is_base_of<InstanceBasedProblem, T>::value, void>::type include()
        {
            auto &factory = Factory<Parent, Args...>::instance();
            const auto is_problem_type = std::conjunction<std::is_same<int, Args>...>::value;
            const int id = IdGetter<is_problem_type>::template get_id<T>(factory.ids());
            
            factory.include(class_name<T>(), id,
                            [](Args &&...params) { return std::make_unique<T>(std::forward<Args>(params)...); });
        }

        template <class T>
        static typename std::enable_if<std::is_base_of<InstanceBasedProblem, T>::value, void>::type include()
        {
            auto &factory = Factory<Parent, Args...>::instance();
            auto constructors = InstanceBasedProblem::load_instances<T, Args...>();

            for(auto& ci: constructors){
                factory.include(fmt::format("{}{}", class_name<T>(), ci.second), ci.second,
                    [c=ci.first](Args &&...params) { 
                        return std::make_unique<T>(c(std::forward<Args>(params)...));
                    });

            }
        }

        //! Accessor to static instance
        static Factory<Parent, Args...> &instance() { return Factory<Parent, Args...>::instance(); }
    };


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
