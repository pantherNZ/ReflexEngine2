#include "Precompiled.h"
#include "PhysicsSystem.h"
#include "TransformComponent.h"
#include "Box2DShapeComponent.h"

namespace Reflex::Systems
{
	void PhysicsSystem::RegisterComponents()
	{
		RequiresComponent( Reflex::Components::Box2DShapeComponent );
	}

	bool PhysicsSystem::ShouldAddObject( const Object& object ) const
	{
		return ( object.GetComponentFlags() & GetRequiredComponents() ).any();
	}

	void PhysicsSystem::Update( const float deltaTime )
	{
		for( const auto& object : m_releventObjects )
		{
			if( const auto shape = object.GetComponent< Reflex::Components::Box2DShapeComponent >() )
			{
				object.GetTransform()->setPosition( shape->GetPosition() );
				object.GetTransform()->setRotation( shape->GetRotation() );
			}
		}
	}
}