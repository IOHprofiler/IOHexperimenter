#pragma once

#include <string>
#include <memory>
#include <map>

namespace ioh
{
	namespace common
	{
		template <class ChildType, typename ...Args>
		class factory
		{
			std::map<std::string, std::shared_ptr<ChildType>(*)(Args&&...)> registry_;
		public:
			factory() = default;
			factory(const factory&) = delete;
			factory& operator=(const factory&) = delete;

			static factory<ChildType, Args...>& get()
			{
				static factory instance;
				return instance;
			}

			std::vector<std::string> keys()
			{
				std::vector<std::string> keys;
				for (const auto& e : registry_)
					keys.emplace_back(e.fist());
				return keys;
			}

			void register_class(std::string id, std::shared_ptr<ChildType> (*func)(Args&&...))
			{
				registry_[id] = func;
			}

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


		template <class ParentType, class ChildType, typename ...Args>
		class register_in_factory
		{
		public:
			static std::shared_ptr<ParentType> create(Args&&... params)
			{
				return std::shared_ptr<ParentType>(ChildType::create(std::forward<Args>(params)...));
			}

			explicit register_in_factory(const std::string id)
			{
				factory<ParentType, Args...>::get().register_class(id, create);
			}
		};
	}
}
