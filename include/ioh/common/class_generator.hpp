#pragma once

#include <string>
#include <memory>
#include <map>

namespace ioh
{
	namespace common
	{
		typedef std::string defaultIDKeyType;

		template <class manufacturedObj>
		class genericGenerator
		{
			/// typedef std::shared_ptr<manufacturedObj> (*BASE_CREATE_FN)() ;

			/// FN_REGISTRY is the registry of all the BASE_CREATE_FN
			/// pointers registered.  Functions are registered using the
			/// regCreateFn member function (see below).
			typedef std::map<std::string, std::shared_ptr<manufacturedObj>(*)()> FN_registry;
			FN_registry registry;


			genericGenerator()
			{
			}

			genericGenerator(const genericGenerator&) = delete;
			genericGenerator& operator=(const genericGenerator&) = delete;


		public:
			/// Singleton access.
			static genericGenerator<manufacturedObj>& instance()
			{
				/// Note that this is not thread-safe!
				static genericGenerator theInstance;
				return theInstance;
			}

			/// \fn regCreateFn(std::string, std::shared_ptr<manufacturedObj> (*)());
			///
			/// Classes derived from manufacturedObj call this function once
			/// per program to register the class ID key, and a pointer to
			/// the function that creates the class.
			void regCreateFn(std::string clName, std::shared_ptr<manufacturedObj> (*func)())
			{
				registry[clName] = func;
			}

			/// \fn std::shared_ptr<manufacturedObj> create(std::string className) const;
			///
			/// Create a new class of the type specified by className.
			/// The create function simple looks up the class ID, and if it's in the list, the statement "(*i).second();" calls the function.
			///
			std::shared_ptr<manufacturedObj> create(std::string className) const
			{
				std::shared_ptr<manufacturedObj> ret(nullptr);
				typename FN_registry::const_iterator regEntry = registry.find(className);
				if (regEntry != registry.end())
				{
					return (*regEntry).second();
				}
				return ret;
			}
		};

		/// Helper template to make registration simple.
		template <class ancestorType, class manufacturedObj, typename classIDKey = defaultIDKeyType>
		class registerInFactory
		{
		public:
			static std::shared_ptr<ancestorType> createInstance()
			{
				return std::shared_ptr<ancestorType>(manufacturedObj::createInstance());
			}

			/// \fn registerInFactory(const classIDKey id)
			/// \brief Register the creation function.  
			///
			/// This simply associates the classIDKey with the function used to create the class.  
			/// The return value is a dummy value, which is used to allow static initialization of the registry.
			registerInFactory(const classIDKey id)
			{
				genericGenerator<ancestorType>::instance().regCreateFn(id, createInstance);
			}
		};
	}
}

// auto shpre = factory().createInstance("bbob/sphere");
