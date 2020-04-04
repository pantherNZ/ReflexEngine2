#include "Precompiled.h"
#include "Component.h"
#include "Object.h"

namespace Reflex::Components
{
	ComponentFamily BaseComponent::s_componentFamilyIdx = 0;

	BaseComponent::BaseComponent( const Object& object )
		: m_object( std::make_unique< Reflex::Object >( object ) )
	{

	}

	BaseComponent::BaseComponent( const BaseComponent& other )
		: m_object( std::make_unique< Reflex::Object >( *other.m_object ) )
	{

	}

	Reflex::Object BaseComponent::GetObject() const
	{
		return *m_object;
	}

	Reflex::Handle< Transform > BaseComponent::GetTransform() const
	{ 
		return GetObject().GetTransform(); 
	}

	Reflex::Core::World& BaseComponent::GetWorld() const
	{
		return GetObject().GetWorld();
	}
}