#pragma once

#include <map>
#include <memory>
#include <string>

namespace ioh
{
	namespace common
	{
		/**
		 * \brief A class factory using a map registry
		 * \tparam ChildType Type of the constructed class
		 * \tparam Args Variadic arguments passed a static create method of the ChildType
		 */
		template <class ChildType, typename ...Args>
		class Factory
		{
			/**
			 * \brief The registry containing the references to the constructed classes
			 */
			std::map<std::string, std::shared_ptr<ChildType>(*)(Args&&...)> registry_;
		public:
			Factory() = default;
			~Factory() = default;
			Factory(const Factory&) = delete;
			Factory(const Factory&&) = delete;
			Factory& operator=(const Factory&) = delete;
			Factory& operator=(const Factory&&) = delete;

			/**
			 * \brief Singleton access for factories of a specific template
			 * \return The singleton instance
			 */
			static Factory<ChildType, Args...>& get()
			{
				static Factory instance; // NOLINT
				return instance;
			}

			/**
			 * \brief Helper function to register new entries to the factory
			 * \param id Unique identifier for ChildType
			 * \param func A reference to a static creation method on a ChildType
			 */
			void register_class(std::string id, std::shared_ptr<ChildType> (*func)(Args&&...))
			{
				// if (registry_.find(id) != registry_.end())
					// log::warning(id + " is already registered"); // TODO: this happens very often, find other way to instantiate this
				registry_[id] = func;
			}

			/**
			 * \brief Forwarding function for instantiating new instances of ChildType
			 * \param id The id of the type to be instantiated
			 * \param params The arguments passed to the create method of ChildType
			 * \return An instance of ChildType
			 */
			[[nodiscard]]
			std::shared_ptr<ChildType> create(std::string id, Args ... params) const
			{
				std::shared_ptr<ChildType> ret(nullptr);
				auto reg_entry = registry_.find(id);
				if (reg_entry != registry_.end())
					return (*reg_entry).second(std::forward<Args>(params)...);
				return ret;
			}
		};


		/**
		 * \brief Helper class, used in order to register new entries to \ref Factory<ParentType, ..Args>
		 * \tparam ParentType The base type of the objects in the Factory
		 * \tparam ChildType The derived type of objects in the Factory
		 * \tparam Args Variadic arguments, forwarded to the creation method of ChildType
		 */
		template <class ParentType, class ChildType, typename ...Args>
		class RegisterInFactory
		{
		public:
			/**
			 * \brief Retrieves the singleton instance of \ref Factory<ParentType, Args...>
			 * and registers the create method of ChildType. 
			 * \param id The identifier for ChildType in \ref Factory<ParentType, Args...>
			 */
			explicit RegisterInFactory(const std::string id)
			{
				Factory<ParentType, Args...>::get().register_class(id, create);
			}
			/**
			 * \brief Placeholder function, passed to \ref Factory::register_class by the constructor
			 * \param params Variadic arguments, forwarded to the creation method of ChildType.
			 * \return Instance of ChildType
			 */
			static std::shared_ptr<ParentType> create(Args&&... params)
			{
				return std::shared_ptr<ParentType>(ChildType::create(std::forward<Args>(params)...));
			}
		};		
	}
}
