#pragma once

#include "Precompiled.h"
#include "BaseObject.h"

#undef GetObject

namespace Reflex 
{ 
	class Object; 

	template< class T >
	class Handle;

	namespace Core { class World; }
}

namespace Reflex::Components
{
	class Transform;

	class BaseComponent
	{
	public:
		friend class Reflex::Core::World;
		Reflex::Object GetObject() const;
		Reflex::Handle< Transform > GetTransform() const;
		Reflex::Core::World& GetWorld() const;

	//protected:
		BaseComponent() {}
		BaseComponent( const Reflex::Object& object );
		BaseComponent( const BaseComponent& other );
		virtual ~BaseComponent() {}
		virtual void OnConstructionComplete() { }
		static std::string GetComponentName() { assert( false ); }
		virtual bool SetValue( const std::string& variable, const std::string& value ) { return false; }
		virtual void GetValues( std::vector< std::pair< std::string, std::string > >& values ) const { }

		BaseObject m_object;
		static ComponentFamily s_componentFamilyIdx;
	};

	template< typename T >
	class Component : public BaseComponent
	{
	public:
		friend class Reflex::Core::World;

		typedef Reflex::Handle< T > Handle;
		Handle GetHandle() const { return Handle( GetObject() ); }

		static ComponentFamily GetFamily()
		{
			static ComponentFamily s_family = s_componentFamilyIdx++;
			assert( s_family < MaxComponents );
			return s_family;
		}

	protected:
		using BaseComponent::BaseComponent;
	};
}

// Helper macros used for serialisation
#define TrySetValue( name, mem ) \
	if( variable == name ) { \
		mem = Reflex::FromString< decltype( mem ) >( value ); \
		return true; } 

#define GetValue( name, mem ) \
	values.emplace_back( name, Reflex::ToString( mem ) );

#define TryGetValue( name, mem, cond ) \
	if( cond ) \
		values.emplace_back( name, Reflex::ToString( mem ) );
