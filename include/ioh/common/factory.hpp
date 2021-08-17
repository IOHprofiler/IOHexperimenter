#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include <cstdlib>

#if defined(__GNUC__) || defined(__GNUG__)
#include <cxxabi.h>
#endif

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


    template <class AbstractType, typename... Args>
    struct Factory
    {
        using Type = std::shared_ptr<AbstractType>;
        using Creator = std::function<Type(Args &&...)>;

        static Factory &instance()
        {
            static Factory f; // NOLINT
            return f;
        }

        void include(const std::string &name, const int id, Creator creator)
        {
            const auto already_defined = name_map.find(name) != std::end(name_map);
            assert(!already_defined);
            name_map[name] = std::move(creator);
            if (!already_defined)
                id_map[check_or_get_next_available(id)] = name;
        }

        [[nodiscard]] int check_or_get_next_available(const int id) const
        {
            const auto known_ids = ids();
            const auto it = std::find(known_ids.begin(), known_ids.end(), id);
            return it == known_ids.end() ? id : get_next_id(known_ids);
        }

        [[nodiscard]] std::vector<std::string> names() const
        {
            std::vector<std::string> keys;
            for (const auto &[fst, snd] : name_map)
                keys.push_back(fst);
            return keys;
        }

        [[nodiscard]] std::vector<int> ids() const
        {
            std::vector<int> keys;
            for (const auto &[fst, snd] : id_map)
                keys.push_back(fst);
            return keys;
        }

        [[nodiscard]] std::unordered_map<int, std::string> map() const { return id_map; }

        [[nodiscard]] Type create(const std::string &name, Args... params) const
        {
            const auto entry = name_map.find(name);
            assert(entry != std::end(name_map));
            return entry->second(std::forward<Args>(params)...);
        }

        [[nodiscard]] Type create(const int id, Args... params) const
        {
            const auto entry = id_map.find(id);
            assert(entry != std::end(id_map));
            return create(entry->second, std::forward<Args>(params)...);
        }

    private:
        Factory() = default;
        Factory(const Factory &) = delete;
        std::unordered_map<std::string, Creator> name_map;
        std::unordered_map<int, std::string> id_map;
    };

    template <bool IsProblem>
    struct IdGetter
    {
    };

    template <>
    struct IdGetter<true>
    {
        template <typename T>
        static int get_id(const std::vector<int> &)
        {
            return T(1, 1).meta_data().problem_id;
        }
    };

    template <>
    struct IdGetter<false>
    {
        template <typename T>
        static int get_id(const std::vector<int> &ids)
        {
            return get_next_id(ids);
        }
    };


    template <typename Parent, typename... Args>
    struct RegisterWithFactory
    {
        template <class T>
        static void include()
        {
            auto &factory = Factory<Parent, Args...>::instance();
            const auto is_problem_type = std::conjunction<std::is_same<int, Args>...>::value;
            const int id = IdGetter<is_problem_type>::template get_id<T>(factory.ids());

            factory.include(class_name<T>(), id,
                            [](Args &&...params) { return std::make_unique<T>(std::forward<Args>(params)...); });
        }

        static Factory<Parent, Args...> &instance() { return Factory<Parent, Args...>::instance(); }
    };


    template <class Type, class Factory>
    struct InvokeApplyOnConstruction
    {
        InvokeApplyOnConstruction() { Factory::template include<Type>(); }
    };

    template <class Type, class Factory>
    struct RegistrationInvoker
    {
        static inline InvokeApplyOnConstruction<Type, Factory> registration_invoker =
            InvokeApplyOnConstruction<Type, Factory>();
    };


    template <class Type, class Factory>
    struct AutomaticTypeRegistration : RegistrationInvoker<Type, Factory>
    {
        InvokeApplyOnConstruction<Type, Factory> &invoker = RegistrationInvoker<Type, Factory>::registration_invoker;
    };
} // namespace ioh::common
