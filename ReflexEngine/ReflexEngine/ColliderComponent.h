#pragma once

#include "Component.h"

namespace Reflex::Components
{
	class CircleCollider : public Component< CircleCollider >
	{
	public:
		CircleCollider( const Reflex::Object& object, const float radius = 0.0f )
			: Component< CircleCollider >( object )
			, m_radius( radius )
		{
		}

		float GetRadius() const { return m_radius; }
		void SetRadius( const float radius ) { m_radius = radius; }
		static std::string GetComponentName() { return "CircleCollider"; }

	protected:
		float m_radius = 0.0f;
	};

	class RectangleCollider : public Component< RectangleCollider >
	{
	public:
		RectangleCollider( const Reflex::Object& object, const sf::Vector2f& size = sf::Vector2f() )
			: Component< RectangleCollider >( object )
			, m_size( size )
		{
		}

		sf::Vector2f GetSize() const { return m_size; }
		void SetSize( const sf::Vector2f& size ) { m_size = size; }
		static std::string GetComponentName() { return "RectangleCollider"; }

	protected:
		sf::Vector2f m_size;
	};
}