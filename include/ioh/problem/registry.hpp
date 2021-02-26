#pragma once

#include <cassert>
#include <functional>
#include <any>
#include "ioh/common.hpp"
#include "ioh/problem/problem.hpp"



namespace ioh::problem
{
    template <class AbstractProblem>
    struct Factory
    {
        using Problem = std::shared_ptr<AbstractProblem>;
        using Creator = std::function<Problem(int, int)>; 

        static Factory &instance() 
        {
            static Factory f; // NOLINT 
            return f;
        }

        void include(const std::string id, Creator creator)
        {
            assert(map.find(id) == std::end(map)); 
            map[id] = std::move(creator);
        }

        [[nodiscard]] std::vector<std::string> keys() const 
        {
            std::vector<std::string> keys;
            for (const auto& [fst, snd] : map)
                keys.push_back(fst);
            return keys;
        }

        [[nodiscard]]
        Problem create(const std::string id, const int instance, const int n_variables) const
        {
            const auto entry = map.find(id);
            assert(entry != std::end(map));
            return entry->second(instance, n_variables);
        }

    private:
        Factory() = default;
        Factory(const Factory&) = delete;
        std::unordered_map<std::string, Creator> map;
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

    struct FactoryRegistry
    {
        static FactoryRegistry& instance()
        {
            static FactoryRegistry f; //NOLINT
            return f;
        }
        
        void include(const std::string& id, std::any e)
        {
            map[id] = std::move(e);
        }

        std::vector<std::string> keys()
        {
            std::vector<std::string> keys;
            for (const auto& [fst, snd] : map)
                keys.push_back(fst);
            return keys;
        }

        template<typename T>
        Factory<T> get(const std::string id)
        {
            const auto entry = map.find(id);
            assert(entry != std::end(map));
            return std::any_cast<Factory<T>>(entry->second);
        }

    private:
        std::unordered_map<std::string, std::any> map;
        FactoryRegistry() = default;
        FactoryRegistry(const FactoryRegistry&) = delete;
    };


    template <class Type, class Factory>
    struct InvokeApplyOnConstruction
    {
        InvokeApplyOnConstruction()
        {
            FactoryRegistry::instance().include(
                common::class_name<Factory>(), &Factory::template instance<Type>());
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
