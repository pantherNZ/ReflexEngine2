#pragma once

#include "Component.h"
#include "TransformComponent.h"

namespace Reflex::Components
{
	class Box2DShapeComponent : public Component< Box2DShapeComponent >
	{
	public:
		Box2DShapeComponent( const Reflex::Object& object, const float radius = 0.0f )
			: Component< Box2DShapeComponent >( object )
		{
			b2BodyDef bodyDef;
			bodyDef.type = b2BodyType::b2_dynamicBody;
			bodyDef.position = Reflex::Vector2fToB2Vec( object.GetTransform()->getPosition(), GetObject().GetWorld().GetBox2DUnitToPixelScale() );

			b2CircleShape shape;
			shape.m_radius = GetObject().GetWorld().ToBox2DUnits( radius );
			//shape.

			//body.
			m_body = GetObject().GetWorld().GetBox2DWorld().CreateBody( &bodyDef );
			m_fixture = m_body->CreateFixture( &shape, 1.0f );
		}

		~Box2DShapeComponent()
		{
			m_body->DestroyFixture( m_fixture );
			GetObject().GetWorld().GetBox2DWorld().DestroyBody( m_body );
		}

		static std::string GetComponentName() { return "Box2DShapeComponent"; }

		const b2Body& GetRigidBody() const { return *m_body; }
		const b2Fixture& GetFixture() const { return *m_fixture; }
		sf::Vector2f GetPosition() const { return Reflex::B2VecToVector2f( m_body->GetPosition(), GetObject().GetWorld().GetBox2DUnitToPixelScale() ); }
		float GetRotation() const { return GetObject().GetWorld().ToWorldUnits( m_body->GetAngle() ); }

	protected:
		b2Body* m_body = nullptr;
		b2Fixture* m_fixture = nullptr;
	};
}