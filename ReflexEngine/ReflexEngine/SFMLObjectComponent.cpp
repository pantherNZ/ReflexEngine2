#include "Precompiled.h"
#include "SFMLObjectComponent.h"
#include "Object.h"

namespace Reflex::Components
{
	CircleShape::CircleShape( const Reflex::Object& owner, const float radius, const std::size_t pointCount, const std::optional< sf::Color > colour )
		: Component< CircleShape >( owner )
		, sf::CircleShape( radius, pointCount )
	{
		if( colour )
			setFillColor( *colour );
	}

	CircleShape::CircleShape( const Reflex::Object& owner, const std::optional< sf::Color > colour )
		: Component< CircleShape >( owner )
	{
		if( colour )
			setFillColor( *colour );
	}

	RectangleShape::RectangleShape( const Reflex::Object& owner, const sf::Vector2f& size, const std::optional< sf::Color > colour )
		: Component< RectangleShape >( owner )
		, sf::RectangleShape( size )
	{
		if( colour )
			setFillColor( *colour );
	}

	RectangleShape::RectangleShape( const Reflex::Object& owner, const std::optional< sf::Color > colour )
		: Component< RectangleShape >( owner )
	{
		if( colour )
			setFillColor( *colour );
	}

	ConvexShape::ConvexShape( const Reflex::Object& owner, const std::size_t pointCount, const std::optional< sf::Color > colour )
		: Component< ConvexShape >( owner )
		, sf::ConvexShape( pointCount )
	{
		if( colour )
			setFillColor( *colour );
	}

	ConvexShape::ConvexShape( const Reflex::Object& owner, const std::vector< sf::Vector2f >& points, const std::optional< sf::Color > colour )
		: Component< ConvexShape >( owner )
		, sf::ConvexShape( points.size() )
	{
		for( size_t i = 0; i < points.size(); ++i )
			setPoint( i, points[i] );

		if( colour )
			setFillColor( *colour );
	}

	ConvexShape::ConvexShape( const Reflex::Object& owner, const std::optional< sf::Color > colour )
		: Component< ConvexShape >( owner )
	{
		if( colour )
			setFillColor( *colour );
	}

	Sprite::Sprite( const Reflex::Object& owner, const sf::Texture& texture, const std::optional< sf::Color > colour )
		: Component< Sprite >( owner )
		, sf::Sprite( texture )
	{
		if( colour )
			setColor( *colour );
	}

	Sprite::Sprite( const Reflex::Object& owner, const sf::Texture& texture, const sf::IntRect& rectangle, const std::optional< sf::Color > colour )
		: Component< Sprite >( owner )
		, sf::Sprite( texture, rectangle )
	{
		if( colour )
			setColor( *colour );
	}
	
	Sprite::Sprite( const Reflex::Object& owner, const std::optional< sf::Color > colour )
		: Component< Sprite >( owner )
	{
		if( colour )
			setColor( *colour );
	}

	Text::Text( const Reflex::Object& owner, const sf::String& string, const sf::Font& font, const unsigned characterSize, const std::optional< sf::Color > colour )
		: Component< Text >( owner )
		, sf::Text( string, font, characterSize )
	{
		if( colour )
			setFillColor( *colour );
	}

	Text::Text( const Reflex::Object& owner, const std::optional< sf::Color > colour )
		: Component< Text >( owner )
	{
		if( colour )
			setFillColor( *colour );
	}

	TODO( "SFML object set values (deserialisation" );
	bool CircleShape::SetValue( const std::string& variable, const std::string& value )
	{
		return false;
	}

	bool RectangleShape::SetValue( const std::string& variable, const std::string& value )
	{
		return false;
	}

	bool ConvexShape::SetValue( const std::string& variable, const std::string& value )
	{
		return false;
	}

	bool Sprite::SetValue( const std::string& variable, const std::string& value )
	{
		return false;
	}

	bool Text::SetValue( const std::string& variable, const std::string& value )
	{
		return false;
	}

	void CircleShape::GetValues( std::vector< std::pair< std::string, std::string > >& values ) const
	{
		values.emplace_back( "Radius", Reflex::ToString( getRadius() ) );
		GetShapeValues( values, *this);
		GetColourValues( values, *this );
	}

	void RectangleShape::GetValues( std::vector< std::pair< std::string, std::string > >& values ) const
	{
		GetShapeValues( values, *this );
		GetColourValues( values, *this );
	}

	void ConvexShape::GetValues( std::vector< std::pair< std::string, std::string > >& values ) const
	{
		GetShapeValues( values, *this );
		GetColourValues( values, *this );
	}

	void Sprite::GetValues( std::vector< std::pair< std::string, std::string > >& values ) const
	{
		TODO( "Serialise sprite texture somehow" );
		if( !Reflex::IsDefault( getColor() ) )
			values.emplace_back( "Colour", Reflex::ToString( getColor() ) );
	}

	void Text::GetValues( std::vector< std::pair< std::string, std::string > >& values ) const
	{
		GetColourValues( values, *this );
		values.emplace_back( "String", getString() );
		values.emplace_back( "Style", Reflex::ToString( getStyle() ) );
	}
}