#pragma once

#include "Component.h"
#include "TransformComponent.h"
#include "Events.h"

namespace Reflex::Components
{
	class RigidBody : public Component< RigidBody >, public b2BodyDef, public Core::EventTriggerer
	{
	public:
		// Default cosntructor (does not create the physics body)
		RigidBody( const Reflex::Object& object )
			: Component< RigidBody >( object )
			, EventTriggerer( GetWorld() )
		{
		}

		// Creates the physics body immediately
		RigidBody( const Reflex::Object& object, const b2BodyType bodyType )
			: Component< RigidBody >( object )
			, EventTriggerer( GetWorld() )
		{
			type = bodyType;
			Recreate();
		}

		// Creates the physics body immediately
		RigidBody( const Reflex::Object& object, const b2BodyDef& definition )
			: Component< RigidBody >( object )
			, EventTriggerer( GetWorld() )
		{
			userData = definition.userData;
			angle = definition.angle;
			linearVelocity = definition.linearVelocity;
			angularVelocity = definition.angularVelocity;
			linearDamping = definition.linearDamping;
			angularDamping = definition.angularDamping;
			allowSleep = definition.allowSleep;
			awake = definition.awake;
			fixedRotation = definition.fixedRotation;
			bullet = definition.bullet;
			type = definition.type;
			enabled = definition.enabled;
			gravityScale = definition.gravityScale;
			Recreate();
		}

		~RigidBody()
		{
			GetObject().GetWorld().GetBox2DWorld().DestroyBody( m_body );
		}

		static std::string GetComponentName() { return "RigidBody"; }

		sf::Vector2f GetPosition() const { return Reflex::B2VecToVector2f( m_body->GetPosition() ); }
		float GetRotation() const { return Reflex::ToWorldUnits( m_body->GetAngle() ); }
		b2Body& GetBody() { return *m_body; }

		struct RigidBodyRecreatedEvent
		{
			b2Body& oldBody;
		};

		// Must be called after changing properties in order to apply the changes (recreate the body)
		void Recreate()
		{
			auto* oldBody = m_body;
			position = Reflex::Vector2fToB2Vec( GetObject().GetTransform()->getPosition() );
			m_body = GetObject().GetWorld().GetBox2DWorld().CreateBody( this );

			if( oldBody )
			{
				GetWorld().GetEventManager().Emit( *this, RigidBodyRecreatedEvent{ *oldBody } );
				GetObject().GetWorld().GetBox2DWorld().DestroyBody( oldBody );
			}
		}

	protected:
		b2Body* m_body = nullptr;
	};
}