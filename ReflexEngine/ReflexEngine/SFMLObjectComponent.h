#pragma once

#include "Component.h"

namespace Reflex::Components
{
	enum class SFMLObjectType : char
	{
		Invalid,
		Circle,
		Rectangle,
		Convex,
		Sprite,
		Text,
		NumTypes
	};

	// Class definition
	class SFMLObject : public Component< SFMLObject >
	{
	public:
		SFMLObject( const Reflex::Object& owner );
		SFMLObject( const Reflex::Object& owner, const sf::CircleShape& shape, const sf::Color& colour = sf::Color::White );
		SFMLObject( const Reflex::Object& owner, const sf::ConvexShape& shape, const sf::Color& colour = sf::Color::White );
		SFMLObject( const Reflex::Object& owner, const sf::RectangleShape& shape, const sf::Color& colour = sf::Color::White );
		SFMLObject( const Reflex::Object& owner, const sf::Sprite& spriteconst, const sf::Color& colour = sf::Color::White );
		SFMLObject( const Reflex::Object& owner, const sf::Text& text, const sf::Color& colour = sf::Color::White );
		SFMLObject( const SFMLObject& other );
		~SFMLObject() { }

		bool SetValue( const std::string& variable, const std::string& value ) override;
		void GetValues( std::unordered_map< std::string, std::string >& values ) const override;
		static std::string GetComponentName() { return "SFMLObject"; }

		// Get functions
		sf::CircleShape& GetCircleShape();
		const sf::CircleShape& GetCircleShape() const;

		sf::RectangleShape& GetRectangleShape();
		const sf::RectangleShape& GetRectangleShape() const;

		sf::ConvexShape& GetConvexShape();
		const sf::ConvexShape& GetConvexShape() const;

		sf::Sprite& GetSprite();
		const sf::Sprite& GetSprite() const;

		sf::Text& GetText();
		const sf::Text& GetText() const;

		const SFMLObjectType GetType() const;

		template< typename T >
		void GetShapeValues( std::unordered_map< std::string, std::string >& values, const T& shape ) const
		{
			if( !Reflex::IsDefault( shape.getFillColor() ) )
				values["FillColour"] = Reflex::ToString( shape.getFillColor() );
			if( !Reflex::IsDefault( shape.getOutlineColor() ) )
				values["OutlineColour"] = Reflex::ToString( shape.getOutlineColor() );
		};

	private:
		union ObjectType
		{
			sf::CircleShape circleShape;			// 292 bytes
			sf::RectangleShape rectShape;			// 292 bytes
			sf::ConvexShape convexShape;			// 300 bytes
			sf::Sprite sprite;						// 272 bytes
			sf::Text text;							// 280 bytes

			ObjectType() {}
			ObjectType( const sf::CircleShape& shape ) : circleShape( shape ) { }
			ObjectType( const sf::RectangleShape& shape ) : rectShape( shape ) { }
			ObjectType( const sf::ConvexShape& shape ) : convexShape( shape ) { }
			ObjectType( const sf::Sprite& sprite ) : sprite( sprite ) { }
			ObjectType( const sf::Text& text ) : text( text ) { }
			~ObjectType() {}
		};

		ObjectType m_objectData;
		SFMLObjectType m_type = SFMLObjectType::Invalid;
	};
}