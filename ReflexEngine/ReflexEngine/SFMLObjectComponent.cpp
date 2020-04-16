#include "Precompiled.h"
#include "SFMLObjectComponent.h"
#include "Object.h"

namespace Reflex::Components
{
	void SFMLObject::Render( sf::RenderTarget& target, sf::RenderStates states ) const
	{
		switch( GetType() )
		{
		case Components::SFMLObjectType::Rectangle:
			target.draw( GetRectangleShape(), states );
			break;
		case Components::SFMLObjectType::Convex:
			target.draw( GetConvexShape(), states );
			break;
		case Components::SFMLObjectType::Circle:
			target.draw( GetCircleShape(), states );
			break;
		case Components::SFMLObjectType::Sprite:
			target.draw( GetSprite(), states );
			break;
		case Components::SFMLObjectType::Text:
			target.draw( GetText(), states );
			break;
		}
	}

	SFMLObject::SFMLObject( const Reflex::Object& owner )
		: Component< SFMLObject >( owner )
	{

	}

	SFMLObject::SFMLObject( const Reflex::Object& owner, const sf::CircleShape& shape, const std::optional< sf::Color > colour )
		: Component< SFMLObject >( owner )
		, m_type( SFMLObjectType::Circle )
		, m_objectData( shape )
	{
		Reflex::CenterOrigin( m_objectData.circleShape );
		if( colour )
			m_objectData.circleShape.setFillColor( *colour );
	}

	SFMLObject::SFMLObject( const Reflex::Object& owner, const sf::ConvexShape& shape, const std::optional< sf::Color > colour )
		: Component< SFMLObject >( owner )
		, m_type( SFMLObjectType::Convex )
		, m_objectData( shape )
	{
		Reflex::CenterOrigin( m_objectData.convexShape );
		
		if( colour )
			m_objectData.convexShape.setFillColor( *colour );
	}

	SFMLObject::SFMLObject( const Reflex::Object& owner, const sf::RectangleShape& shape, const std::optional< sf::Color > colour )
		: Component< SFMLObject >( owner )
		, m_type( SFMLObjectType::Rectangle )
		, m_objectData( shape )
	{
		Reflex::CenterOrigin( m_objectData.rectShape );
		if( colour )
			m_objectData.rectShape.setFillColor( *colour );
	}

	SFMLObject::SFMLObject( const Reflex::Object& owner, const sf::Sprite& sprite, const std::optional< sf::Color > colour )
		: Component< SFMLObject >( owner )
		, m_type( SFMLObjectType::Sprite )
		, m_objectData( sprite )
	{
		Reflex::CenterOrigin( m_objectData.sprite );
		if( colour )
			m_objectData.sprite.setColor( *colour );
	}

	SFMLObject::SFMLObject( const Reflex::Object& owner, const sf::Text& text, const std::optional< sf::Color > colour )
		: Component< SFMLObject >( owner )
		, m_type( SFMLObjectType::Text )
		, m_objectData( text )
	{
		Reflex::CenterOrigin( m_objectData.text );
		if( colour )
			m_objectData.text.setFillColor( *colour );
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
		if( variable == "Type" )
		{
			for( unsigned i = 0; i < std::size( objectNames ); ++i )
			{
				if( value == objectNames[i] )
				{
					m_type = SFMLObjectType( i );
					return true;
				}
			}
		}

		return false;
	}

	void SFMLObject::GetValues( std::vector< std::pair< std::string, std::string > >& values ) const
	{
		values.emplace_back( "Type", objectNames[( size_t )m_type] );

		const auto GetShapeValues = [&values]( const sf::Shape& shape )
		{
			values.emplace_back( "PointCount", Reflex::ToString( shape.getPointCount() ) );
			values.emplace_back( "Points", "" );
			for( size_t i = 0; i < shape.getPointCount(); ++i )
				values.back().second += ( i > 0 ? ", " : "" ) + Reflex::ToString( shape.getPoint( i ) );
		};

		switch( m_type )
		{
		case Reflex::Components::SFMLObjectType::Circle:
		{
			values.emplace_back( "Radius", Reflex::ToString( GetCircleShape().getRadius() ) );
			GetShapeValues( GetCircleShape() );
			GetColourValues( values, GetCircleShape() );
			break;
		}
		case Reflex::Components::SFMLObjectType::Rectangle:
			GetShapeValues( GetRectangleShape() );
			GetColourValues( values, GetCircleShape() );
			break;
		case Reflex::Components::SFMLObjectType::Convex:
			GetShapeValues( GetRectangleShape() );
			GetColourValues( values, GetConvexShape() );
			break;
		case Reflex::Components::SFMLObjectType::Sprite:
		{
			if( !Reflex::IsDefault( GetSprite().getColor() ) )
				values.emplace_back( "Colour", Reflex::ToString( GetSprite().getColor() ) );
			break;
		}
		case Reflex::Components::SFMLObjectType::Text:
		{
			GetColourValues( values, GetText() );
			values.emplace_back( "String", GetText().getString() );
			values.emplace_back( "Style", Reflex::ToString( GetText().getStyle() ) );
			break;
		}
		case Reflex::Components::SFMLObjectType::Invalid:
		case Reflex::Components::SFMLObjectType::NumTypes:
			break;
		}
	}
}