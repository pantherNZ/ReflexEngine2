#include "Precompiled.h"
#include "Object.h"
#include "World.h"
#include "Component.h"
#include "TransformComponent.h"

namespace Reflex
{
	Object::Object( const Object& other )
		: m_world( other.m_world )
		, m_counter( other.m_counter )
		, m_index( other.m_index )
	{ }

	// Constructor can only be called by World
	Object::Object( Reflex::Core::World& world, const uint32_t index, const uint32_t counter )
		: m_index( index ), m_counter( counter ), m_world( &world )
	{ }

	void Object::Destroy()
	{
		GetWorld().DestroyObject( *this );
	}

	void Object::RemoveAllComponents()
	{
		GetWorld().ObjectRemoveAllComponents( *this );
	}

	Reflex::Components::Transform::Handle Object::GetTransform() const
	{
		return GetComponent< Reflex::Components::Transform >();
	}

	Reflex::Core::World& Object::GetWorld() const
	{
		assert( m_world );
		return *m_world;
	}

	bool Object::IsFlagSet( const ObjectFlags flag ) const
	{
		return GetWorld().IsObjectFlagSet( *this, flag );
	}

	bool Object::IsValid() const
	{
		return m_world && GetWorld().IsValidObject( *this );
	}

	Object::operator bool() const
	{
		return IsValid();
	}

	bool Object::operator==( const Object& other ) const
	{
		return GetIndex() == other.GetIndex() && GetCounter() == other.GetCounter();
	}

	bool Object::operator!=( const Object& other ) const
	{
		return !( *this == other );
	}
}