#pragma once

#include "ioh/common.hpp"

namespace ioh::problem
{
    template <class AbstractProblem>
    struct Factory
    {
        using Problem = std::shared_ptr<AbstractProblem>;
        using Creator = std::function<Problem(int, int)>;

        static Factory& instance()
        {
            static Factory f; // NOLINT 
            return f;
        }

        void include(const std::string id, Creator creator)
        {
            assert(map.find(id) == std::end(map));
            map[id] = std::move(creator);
            const int problem_id = map[id](1, 1)->meta_data().problem_id;
            assert(id_map.find(problem_id) == std::end(id_map));
            id_map[problem_id] = id;
        }

        [[nodiscard]] std::vector<std::string> keys() const
        {
            std::vector<std::string> keys;
            for (const auto& [fst, snd] : map)
                keys.push_back(fst);
            return keys;
        }

        [[nodiscard]] std::unordered_map<int, std::string> problem_id_map() const
        {
            return id_map; 
        }

        [[nodiscard]]
        Problem create(const std::string id, const int instance, const int n_variables) const
        {
            const auto entry = map.find(id);
            assert(entry != std::end(map));
            return entry->second(instance, n_variables);
        }

        Problem create(const int id, const int instance, const int n_variables) const
        {
            const auto entry = id_map.find(id);
            assert(entry != std::end(id_map));
            return create(entry->second, instance, n_variables);
        }

    private:
        Factory() = default;
        Factory(const Factory&) = delete;
        std::unordered_map<std::string, Creator> map;
        std::unordered_map<int, std::string> id_map;
    };

    template <class Parent>
    struct RegisterWithFactory
    {
        template <class T>
        static void include()
        {
            Factory<Parent>::instance().include(common::class_name<T>(), 
                [](const int instance, const int n_variables)
            {
                return std::make_unique<T>(instance, n_variables);
            });
        }

        template <class T>
        static Factory<Parent> &instance()
        {
            return Factory<Parent>::instance();
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
        static inline const InvokeApplyOnConstruction<Type, Factory>
            registration_invoker = InvokeApplyOnConstruction<Type, Factory>();
    };

    template <class Derived, class Parent>
    struct AutomaticFactoryRegistration: RegistrationInvoker<Derived, RegisterWithFactory<Parent>>
    {
    };

}
