#pragma once
#include <cassert>

#include "utils.hpp"

namespace ioh::common
{
    template <class AbstractType, typename ...Args>
    struct Factory
    {
        using Type = std::shared_ptr<AbstractType>;
        using Creator = std::function<Type(Args &&...)>;

        static Factory &instance()
        {
            static Factory f; // NOLINT 
            return f;
        }

        void include(const std::string name, const int id, Creator creator)
        {
            assert(map.find(name) == std::end(map));
            map[name] = std::move(creator);
            assert(id_map.find(id) == std::end(id_map));
            id_map[id] = name;
        }

        [[nodiscard]] std::vector<std::string> names() const
        {
            std::vector<std::string> keys;
            for (const auto &[fst, snd] : map)
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

        [[nodiscard]]
        std::unordered_map<int, std::string> name_to_id() const
        {
            return id_map;
        }

        [[nodiscard]] std::unordered_map<int, std::string> numeric_id_map() const
        {
            return id_map;
        }

        [[nodiscard]]
        Type create(const std::string id, Args ... params) const
        {
            const auto entry = map.find(id);
            assert(entry != std::end(map));
            return entry->second(std::forward<Args>(params)...);
        }

        [[nodiscard]]
        Type create(const int id, Args ... params) const
        {
            const auto entry = id_map.find(id);
            assert(entry != std::end(id_map));
            return create(entry->second, std::forward<Args>(params)...);
        }

    private:
        Factory() = default;
        Factory(const Factory &) = delete;
        std::unordered_map<std::string, Creator> map;
        std::unordered_map<int, std::string> id_map;
    };


    static int get_next_id(const std::vector<int> &ids)
    {
        return ids.empty() ? 1 : (*std::max_element(ids.begin(), ids.end())) + 1;
    }

    template <bool IsProblem>
    struct IdGetter
    {
    };

    template <>
    struct IdGetter<true>
    {
        template <typename T>
        static int get_id(const std::vector<int> &ids)
        {
            const auto problem_id = T(1, 1).meta_data().problem_id;
            const auto it = std::find(ids.begin(), ids.end(), problem_id);
            return it == ids.end() ? problem_id : get_next_id(ids);
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


    template <typename Parent, typename ...Args>
    struct RegisterWithFactory
    {
        template <class T>
        static void include()
        {
            auto &factory = Factory<Parent, Args...>::instance();
            const auto is_problem_type = std::conjunction<std::is_same<int, Args>...>::value;
            const int id = IdGetter<is_problem_type>::template get_id<T>(factory.ids());

            factory.include(class_name<T>(), id, [](Args &&...params)
            {
                return std::make_unique<T>(std::forward<Args>(params)...);
            });
        }

        static Factory<Parent, Args...> &instance()
        {
            return Factory<Parent, Args...>::instance();
        }
    };


    template <class Type, class Factory>
    struct InvokeApplyOnConstruction
    {
        InvokeApplyOnConstruction()
        {
            Factory::template include<Type>();
        }
    };

    template <class Type, class Factory>
    struct RegistrationInvoker
    {
        static inline InvokeApplyOnConstruction<Type, Factory> registration_invoker = InvokeApplyOnConstruction<
            Type, Factory>();
    };


    template <class Type, class Factory>
    struct AutomaticTypeRegistration : RegistrationInvoker<Type, Factory>
    {
        InvokeApplyOnConstruction<Type, Factory> &invoker =
            RegistrationInvoker<Type, Factory>::registration_invoker;
    };
}
