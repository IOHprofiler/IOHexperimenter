#pragma once

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "builtins.hpp"
#include "container_utils.hpp"
#include "file.hpp"


namespace ioh::common
{
    //! Function to get the next non zero value in an array of integers
    inline int get_next_id(const std::vector<int> &ids)
    {
        return ids.empty() ? 1 : (*std::max_element(ids.begin(), ids.end())) + 1;
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

        //! Accessor to static instance; the first call adds the built-in entries (see builtins.hpp)
        static Factory &instance()
        {
            static Factory f; // NOLINT
            return f;
        }

        /**
         * @brief Include a new element to the factory
         *
         * A new name gets `id`, or the next free id when `id` is taken; with keep_id, an id that belongs to
         * another name, or a registered name with another id, throws std::invalid_argument instead (checked under
         * the same lock as the insertion). An existing name keeps its id and gets the new creator, except the name
         * of a built-in entry, which throws std::invalid_argument. Built-in entries must be unique: a duplicate name
         * or id among them throws std::logic_error. (register_problem adds the rules for the ids of problems.)
         */
        void include(const std::string &name, const int id, Creator creator, const bool keep_id = false)
        {
            const std::lock_guard<std::recursive_mutex> lock(mutex_);
            const auto already_defined = name_map.find(name) != std::end(name_map);
            if (keep_id && !adding_builtins_)
            {
                const auto holder = id_map.find(id);
                if (holder != std::end(id_map) && holder->second != name)
                    throw std::invalid_argument("Cannot register " + name + " with id " + std::to_string(id) +
                                                ": the id belongs to " + holder->second);
                if (holder == std::end(id_map) && already_defined)
                    throw std::invalid_argument("Cannot register " + name + " with id " + std::to_string(id) +
                                                ": it is registered with another id");
            }
            if (adding_builtins_)
            {
                if (already_defined || id_map.find(id) != std::end(id_map))
                    throw std::logic_error("Duplicate built-in factory entry: " + name + " (id " +
                                           std::to_string(id) + ")");
                builtin_names_.insert(name);
            }
            else if (is_builtin(name))
                throw std::invalid_argument(name + " is a built-in factory entry and cannot be replaced: " +
                                            "register under another name");
            name_map[name] = std::move(creator);
            if (!already_defined)
                id_map[check_or_get_next_available(id)] = name;
        }

        /**
         * @brief Include a new element under the id of its name if the name is registered, otherwise under the first
         * free id from `first`, and return that id
         *
         * Choosing the id and including the element happen under one lock, so that concurrent registrations get
         * different ids. make(id) returns the creator, which may need the id. The rules of include apply.
         */
        template <typename MakeCreator>
        int include_with_id(const std::string &name, const int first, MakeCreator make)
        {
            const std::lock_guard<std::recursive_mutex> lock(mutex_);
            const int id = check_or_get_next_available(first, name);
            include(name, id, make(id));
            return id;
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

        //! Whether name is the name of a built-in entry
        [[nodiscard]] bool is_builtin(const std::string &name) const
        {
            const std::lock_guard<std::recursive_mutex> lock(mutex_);
            return builtin_names_.count(name) != 0;
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
        //! Adds the built-in entries before anything else can register, so their ids are deterministic.
        //! add_builtins must not call instance() of the factory it populates.
        Factory()
        {
            adding_builtins_ = true;
            add_builtins(*this);
            adding_builtins_ = false;
        }
        Factory(const Factory &) = delete;
        Factory &operator=(const Factory &) = delete;

        std::map<std::string, Creator> name_map;
        std::map<int, std::string> id_map;
        std::set<std::string> builtin_names_;
        bool adding_builtins_ = false;
        mutable std::recursive_mutex mutex_;
    };

    /**
     * @brief Access to the factory of Parent (see problem::ProblemRegistry and suite::SuiteRegistry)
     *
     * @tparam Parent Abstract type
     * @tparam Args Parent constructor args
     */
    template <typename Parent, typename... Args>
    struct RegisterWithFactory
    {
        //! Accessor to static instance
        static Factory<Parent, Args...> &instance() { return Factory<Parent, Args...>::instance(); }
    };
} // namespace ioh::common
