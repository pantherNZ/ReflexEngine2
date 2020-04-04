#pragma once

#include "Precompiled.h"
#include "Component.h"

namespace Reflex::Core
{
	class World;
}

namespace Reflex
{
	template< class T >
	class Handle;

	namespace Components { class Transform; }

	enum class ObjectFlags
	{
		Deleted,
		ConstructionComplete,
		NumFlags,
	};

	class Object
	{
	public:
		friend class Reflex::Core::World;

		// This constructor is only used to create null / invalid objects
		Object() { }
		Object( const Object& other );
		virtual ~Object() { }

		void Destroy();

		// Creates and adds a new component of the template type and returns a handle to it
		template< class T, typename... Args >
		Handle< T > AddComponent( Args&& ... args );

		// Removes all components
		void RemoveAllComponents();

		// Removes component matching the template type
		template< class T >
		bool RemoveComponent();

		// Checks if this object has a component of template type
		template< class T >
		bool HasComponent() const;

		// Returns a component handle of template type if this object has one
		template< class T >
		Handle< T > GetComponent() const;

		Handle< Reflex::Components::Transform > GetTransform() const;

		//template< class T >
		//std::vector< Handle< T > > GetComponents() const;

		// Copy components from another object to this object
		//template< typename... Args >
		//void CopyComponentsFrom( const Object& other );

		Reflex::Core::World& GetWorld() const;
		//const Reflex::Core::World& GetWorld() const;
		std::uint32_t GetIndex() const { return m_index; }
		std::uint32_t GetCounter() const { return m_counter; }
		bool IsFlagSet( const ObjectFlags flag ) const;

		bool IsValid() const;
		explicit operator bool() const;
		bool operator==( const Object& other ) const;
		bool operator!=( const Object& other ) const;

	protected:
		//template< typename T, typename... Args >
		//void CopyComponentsFromInternal( const Object& other );
		//
		//// Remove the base case where there is 0 argument types (above function calls itself recursively until we reach 0 template arguments)
		//template< typename... Args >
		//typename std::enable_if< sizeof...( Args ) == 0 >::type CopyComponentsFromInternal( const Object& other ) { LOG_INFO( "Copy components complete" ); }

		// Constructor can only be called by World
		Object( Reflex::Core::World& world, const uint32_t index, const uint32_t counter );

	protected:
		Reflex::Core::World* m_world = nullptr;
		std::uint32_t m_index = -1;
		std::uint32_t m_counter = -1;
	};

	template< class T >
	class Handle
	{
	public:
		Handle() {}
		Handle( const Object& object ) : object( object ) { }
		T* Get() const;
		T* operator->() const { return Get(); }
		T& operator*() const { return *Get(); }
		bool IsValid() const;

		explicit operator bool() const { return IsValid(); }
		bool operator==( const Handle< T >& other ) const;
		bool operator!=( const Handle< T >& other ) const;

		template< class V >
		Handle( const Handle< V >& handle ) = delete;

		Object object;
	};
}

#include "ObjectTemplate.hpp"