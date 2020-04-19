#pragma once

#include "Component.h"
#include "RigidBodyComponent.h"
#include "Events.h"

namespace Reflex::Components
{
	class CircleCollider : public Component< CircleCollider, RigidBody >, protected b2CircleShape, public Core::EventReceiver
	{
	public:
		CircleCollider( const Reflex::Object& object, const float radius = 0.0f, const float density = 1.0f )
			: Component< CircleCollider, RigidBody >( object )
		{
			m_radius = Reflex::ToBox2DUnits( radius );
		}

		static std::string GetComponentName() { return "CircleCollider"; }

		float GetRadius() const { return Reflex::ToWorldUnits( m_radius ); }
		void SetRadius( const float radius, const bool recreate = true ) { m_radius = Reflex::ToBox2DUnits( radius ); if( recreate ) Recreate(); }
		float GetDensity() const { return m_density; }
		void SetDensity( const float density, const bool recreate = true ) { m_density = density; if( recreate ) Recreate(); }
		void OnConstructionComplete() final { Recreate(); }

		void Recreate()
		{
			const auto rigidBody = GetObject().GetComponent< Components::RigidBody >();
			rigidBody->Subscribe< Components::RigidBody::RigidBodyRecreatedEvent >( *this, &CircleCollider::OnRigidBodyRecreated );

			if( m_fixture )
				rigidBody->GetBody().DestroyFixture( m_fixture );

			m_fixture = rigidBody->GetBody().CreateFixture( this, m_density );
		}

	protected:
		void OnRigidBodyRecreated( const Components::RigidBody::RigidBodyRecreatedEvent& e ) 
		{ 
			e.oldBody.DestroyFixture( m_fixture );
			m_fixture = nullptr;
			Recreate(); 
		}

	protected:
		b2Fixture* m_fixture = nullptr;
		float m_density = 1.0f;
	};
} 