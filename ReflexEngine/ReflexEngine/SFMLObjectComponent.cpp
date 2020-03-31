#include "Precompiled.h"
#include "SFMLObjectComponent.h"
#include "Object.h"

namespace Reflex::Components
{
	SFMLObject::SFMLObject( const Reflex::Object& owner )
		: Component< SFMLObject >( owner )
	{

	}

	SFMLObject::SFMLObject( const Reflex::Object& owner, const sf::CircleShape& shape, const sf::Color& colour )
		: Component< SFMLObject >( owner )
		, m_type( SFMLObjectType::Circle )
		, m_objectData( shape )
	{
		Reflex::CenterOrigin( m_objectData.circleShape );
		m_objectData.circleShape.setFillColor( colour );
	}

	SFMLObject::SFMLObject( const Reflex::Object& owner, const sf::ConvexShape& shape, const sf::Color& colour )
		: Component< SFMLObject >( owner )
		, m_type( SFMLObjectType::Convex )
		, m_objectData( shape )
	{
		Reflex::CenterOrigin( m_objectData.convexShape );
		m_objectData.convexShape.setFillColor( colour );
	}

	SFMLObject::SFMLObject( const Reflex::Object& owner, const sf::RectangleShape& shape, const sf::Color& colour )
		: Component< SFMLObject >( owner )
		, m_type( SFMLObjectType::Rectangle )
		, m_objectData( shape )
	{
		Reflex::CenterOrigin( m_objectData.rectShape );
		m_objectData.rectShape.setFillColor( colour );
	}

	SFMLObject::SFMLObject( const Reflex::Object& owner, const sf::Sprite& sprite, const sf::Color& colour )
		: Component< SFMLObject >( owner )
		, m_type( SFMLObjectType::Sprite )
		, m_objectData( sprite )
	{
		Reflex::CenterOrigin( m_objectData.sprite );
		m_objectData.sprite.setColor( colour );
	}

	SFMLObject::SFMLObject( const Reflex::Object& owner, const sf::Text& text, const sf::Color& colour )
		: Component< SFMLObject >( owner )
		, m_type( SFMLObjectType::Text )
		, m_objectData( text )
	{
		Reflex::CenterOrigin( m_objectData.text );
		m_objectData.text.setFillColor( colour );
	}

	SFMLObject::SFMLObject( const SFMLObject& other )
		: Component< SFMLObject >( other )
		, m_type( other.m_type )
	{
		memcpy( &this->m_objectData, &other.m_objectData, sizeof( ObjectType ) );
	}

	sf::CircleShape& SFMLObject::GetCircleShape()
	{
		return m_objectData.circleShape;
	}

	const sf::CircleShape& SFMLObject::GetCircleShape() const
	{
		return m_objectData.circleShape;
	}

	sf::RectangleShape& SFMLObject::GetRectangleShape()
	{
		return m_objectData.rectShape;
	}

	const sf::RectangleShape& SFMLObject::GetRectangleShape() const
	{
		return m_objectData.rectShape;
	}

	sf::ConvexShape& SFMLObject::GetConvexShape()
	{
		return m_objectData.convexShape;
	}

	const sf::ConvexShape& SFMLObject::GetConvexShape() const
	{
		return m_objectData.convexShape;
	}

	sf::Sprite& SFMLObject::GetSprite()
	{
		return m_objectData.sprite;
	}

	const sf::Sprite& SFMLObject::GetSprite() const
	{
		return m_objectData.sprite;
	}

	sf::Text& SFMLObject::GetText()
	{
		return m_objectData.text;
	}

	const sf::Text& SFMLObject::GetText() const
	{
		return m_objectData.text;
	}

	const Reflex::Components::SFMLObjectType SFMLObject::GetType() const
	{
		return m_type;
	}

	namespace
	{
		std::string objectNames[] =
		{
			"Invalid",
			"Circle",
			"Rectangle",
			"Convex",
			"Sprite",
			"Text",
		};

		static_assert( std::size( objectNames ) == ( size_t )SFMLObjectType::NumTypes );
	}

	bool SFMLObject::SetValue( const std::string& variable, const std::string& value )
	{
		return false;
	}

	void SFMLObject::GetValues( std::unordered_map< std::string, std::string >& values ) const
	{
		values["Type"] = objectNames[( size_t )m_type];

		switch( m_type )
		{
		case Reflex::Components::SFMLObjectType::Invalid:
			break;
		case Reflex::Components::SFMLObjectType::Circle:
			if( !Reflex::IsDefault( GetCircleShape().getFillColor() ) )
				values["FillColour"] = Reflex::ToString( GetCircleShape().getFillColor() );
			break;
		case Reflex::Components::SFMLObjectType::Rectangle:
			break;
		case Reflex::Components::SFMLObjectType::Convex:
			break;
		case Reflex::Components::SFMLObjectType::Sprite:
			break;
		case Reflex::Components::SFMLObjectType::Text:
			break;
		case Reflex::Components::SFMLObjectType::NumTypes:
			break;
		}
	}
}