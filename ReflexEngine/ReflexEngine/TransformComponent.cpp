#include "Precompiled.h"
#include "TransformComponent.h"
#include "Object.h"
#include "World.h"

namespace Reflex::Components
{
	unsigned Transform::s_nextRenderIndex = 1U;

	Transform::Transform( const Reflex::Object& owner, const sf::Vector2f& position /*= sf::Vector2f()*/, const float rotation /*= 0.0f*/, const sf::Vector2f& scale /*= sf::Vector2f( 1.0f, 1.0f )*/ )
		: Component< Transform >( owner )
		, SceneNode( owner )
	{
		assert( scale.x != 0.0f || scale.y != 0.0f );
		sf::Transformable::setPosition( position );
		setRotation( rotation );
		setScale( scale );
	}

	Transform::Transform( const Transform& other )
		: SceneNode( other )
		, Component( other )
		, m_rotateDegreesPerSec( other.m_rotateDegreesPerSec )
		, m_rotateDurationSec( other.m_rotateDurationSec )
		, m_finishedRotationCallback( other.m_finishedRotationCallback )
	{

	}

	bool Transform::SetValue( const std::string& variable, const std::string& value )
	{
		if( variable == "Position" )
		{
			setPosition( Reflex::FromString< sf::Vector2f >( value ) );
			return true;
		}
		else if( variable == "Rotation" )
		{
			setRotation( Reflex::FromString< float >( value ) );
			return true;
		}
		else if( variable == "Scale" )
		{
			setScale( Reflex::FromString< sf::Vector2f >( value ) );
			return true;
		}
		else if( variable == "RenderIndex" )
		{
			m_renderIndex = Reflex::FromString< unsigned >( value );
			return true;
		}

		return false;
	}

	void Transform::GetValues( std::unordered_map< std::string, std::string >& values ) const
	{
		values["Position"] = Reflex::ToString( getPosition() );
		values["Rotation"] = Reflex::ToString( getRotation() );
		values["Scale"] = Reflex::ToString( getScale() );
		//values["RenderIndex"] = Reflex::ToString( GetRenderIndex() );
	}

	void Transform::setPosition( float x, float y )
	{
		Transform::setPosition( sf::Vector2f( x, y ) );
	}

	void Transform::setPosition( const sf::Vector2f& position )
	{
		//auto& tileMap = m_object->GetWorld().GetTileMap();
		//const auto previousID = tileMap.GetID( m_object );

		sf::Transformable::setPosition( position );

		//const auto newID = tileMap.GetID( m_object );
		//
		//if( previousID != newID )
		//{
		//	tileMap.RemoveByID( m_object, previousID );
		//	tileMap.Insert( m_object );
		//}
	}

	void Transform::move( float offsetX, float offsetY )
	{
		Transform::move( sf::Vector2f( offsetX, offsetY ) );
	}

	void Transform::move( const sf::Vector2f& offset )
	{
		setPosition( getPosition() + offset );
	}

	void Transform::setScale( const sf::Vector2f scale )
	{
		Transformable::setScale( scale );
		assert( scale.x != 0.0f || scale.y != 0.0f );
	}

	void Transform::setScale( const float scaleX, const float scaleY )
	{
		Transformable::setScale( scaleX, scaleY );
		assert( scaleX != 0.0f || scaleY != 0.0f );
	}

	void Transform::RotateForDuration( const float degrees, const float durationSec )
	{
		m_rotateDegreesPerSec = degrees / durationSec;
		m_rotateDurationSec = durationSec;
		m_finishedRotationCallback = nullptr;
	}

	void Transform::RotateForDuration( const float degrees, const float durationSec, std::function< void( const Transform::Handle& ) > finishedRotationCallback )
	{
		m_rotateDegreesPerSec = degrees / durationSec;
		m_rotateDurationSec = durationSec;
		m_finishedRotationCallback = finishedRotationCallback;
	}

	void Transform::StopRotation()
	{
		m_rotateDegreesPerSec = 0.0f;
		m_rotateDurationSec = 0.0f;
		m_finishedRotationCallback = nullptr;
	}

	void Transform::SetVelocity( const sf::Vector2f velocity )
	{
		m_velocity = Reflex::Truncate( velocity, GetMaxVelocity() );
	}

	void Transform::SetZOrder( const unsigned renderIndex )
	{
		m_renderIndex = GetLayer() * 10000 + renderIndex;
	}

	void Transform::IncrementZOrder()
	{
		SetZOrder( s_nextRenderIndex++ );
	}

	void Transform::SetLayer( const unsigned layerIndex )
	{
		const auto idx = m_renderIndex % 10000;
		m_renderIndex = layerIndex * 10000 + idx;
	}

	unsigned Transform::GetLayer() const
	{
		return m_renderIndex / 10000;
	}

	unsigned Transform::GetRenderIndex() const
	{
		return m_renderIndex;
	}
}