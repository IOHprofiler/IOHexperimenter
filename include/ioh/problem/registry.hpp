#pragma once

#include <cassert>
#include <functional>

#include "ioh/common.hpp"


namespace ioh::problem
{
    template <class AbstractProblem>
    struct Factory
    {
        using Problem = std::unique_ptr<AbstractProblem>;
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
            Factory<Parent>::instance().include(common::problem_name<T>(), [](const int instance, const int n_variables)
            {
                return std::make_unique<T>(instance, n_variables);
            });
        }
    };

    template <class T, class F>
    struct InvokeApplyOnConstruction
    {
        InvokeApplyOnConstruction()
        {
            F::template include<T>();
        }
    };

    template <class T, class F>
    struct RegistrationInvoker
    {
        static inline const InvokeApplyOnConstruction<T, F> registration_invoker = InvokeApplyOnConstruction<T, F>();
    };

    template <class Derived, class Parent>
    struct AutomaticFactoryRegistration: RegistrationInvoker<Derived, RegisterWithFactory<Parent>>
    {
    };

}
