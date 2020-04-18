#include "Precompiled.h"
#include "PhysicsSystem.h"
#include "TransformComponent.h"
#include "ColliderComponent.h"

namespace Reflex::Systems
{
	void PhysicsSystem::RegisterComponents()
	{
		RequiresComponent( Reflex::Components::CircleCollider );
		RequiresComponent( Reflex::Components::RectangleCollider );
	}

	bool PhysicsSystem::ShouldAddObject( const Object& object ) const
	{
		return ( object.GetComponentFlags() & GetRequiredComponents() ).any();
	}

	void PhysicsSystem::Update( const float deltaTime )
	{
		for( const auto& object : m_releventObjects )
		{
			if( const auto circle = object.GetComponent< Reflex::Components::CircleCollider >() )
			{

			}
		}
	}
}