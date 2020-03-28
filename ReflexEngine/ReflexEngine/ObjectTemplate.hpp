#pragma once

#include "World.h"

namespace Reflex
{
	// Template definitions
	template< class T, typename... Args >
	Handle< T > Object::AddComponent( Args&& ... args )
	{
		return GetWorld().template ObjectAddComponent< T >( *this, std::forward<Args>( args )... );
	}

	template< class T >
	bool Object::RemoveComponent()
	{
		return GetWorld().template ObjectRemoveComponent< T >( *this );
	}

	template< class T >
	bool Object::HasComponent() const
	{
		return GetWorld().template ObjectHasComponent< T >( *this );
	}

	template< class T >
	Handle< T > Object::GetComponent() const
	{
		assert( IsValid() );
		if( !HasComponent< T >() )
			return { };
		return Handle< T >( *this );
	}

	//template< typename... Args >
	//void Object::CopyComponentsFrom( const Object& other )
	//{
	//	CopyComponentsFromInternal< Args... >( other );
	//}
	//
	//template< typename T, typename... Args >
	//void Object::CopyComponentsFromInternal( const Object& other )
	//{
	//	RemoveAllComponents();
	//
	//	auto component = other.GetComponent< T >();
	//
	//	if( component )
	//		AddComponent< T >( *component.Get() );
	//
	//	// Recursively pop arguments off the variadic template args and continue
	//	CopyComponentsFrom< Args... >( other );
	//}

	template< class T >
	T* Handle< T >::Get() const
	{
		assert( object.IsValid() );
		return object.GetWorld().template ObjectGetComponent< T >( object );
	}

	template< class T >
	bool Handle< T >::IsValid() const
	{
		return object.IsValid() && object.HasComponent< T >();
	}

	template< class T >
	bool Handle< T >::operator==( const Handle< T >& other ) const
	{
		return object == other.object;
	}

	template< class T >
	bool Handle< T >::operator!=( const Handle< T >& other ) const
	{
		return !( *this == other );
	}
}

// Hash function for handles allows it to be used with hash maps such as std::unordered_map etc.
namespace std
{
	template<>
	struct hash< Reflex::Object >
	{
		std::size_t operator()( const Reflex::Object& obj ) const
		{
			return hash< uint32_t >()( obj.GetIndex() ) ^ ( hash< uint32_t >()( obj.GetCounter() ) >> 1 );
		}
	};

	template< class T >
	struct hash< Reflex::Handle< T > >
	{
		std::size_t operator()( const Reflex::Handle< T >& handle ) const
		{
			return hash< Reflex::Object >()( handle.object );
		}
	};
}