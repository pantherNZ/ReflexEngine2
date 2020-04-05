#include "Precompiled.h"
#include "SteeringComponent.h"
#include "TransformComponent.h"

namespace Reflex::Components
{
	namespace
	{
		std::string behaviourNames[] =
		{
			"Seek",
			"Flee",
			"Arrival",
			"Wander",
			"Pursue",
			"Evade",
		};

		static_assert( std::size( behaviourNames ) == ( size_t )Steering::Behaviours::NumBehaviours );
	}

	bool Steering::SetValue( const std::string& variable, const std::string& value )
	{
		for( unsigned i = 0; i < std::size( behaviourNames ); ++i )
		{
			if( variable == behaviourNames[i] )
			{
				m_behaviours.set( i, Reflex::FromString< bool >( value ) );
				return true;
			}
		}

		if( variable == "MaxForce" )
		{
			m_maxForce = Reflex::FromString< float >( value );
			return true;
		}
		else if( variable == "Mass" )
		{
			m_mass = Reflex::FromString< float >( value );
			return true;
		}
		else if( variable == "SlowingRadius" )
		{
			m_slowingRadius = Reflex::FromString< float >( value );
			return true;
		}
		else if( variable == "WanderCircleRadius" )
		{
			m_wanderCircleRadius = Reflex::FromString< float >( value );
			return true;
		}
		else if( variable == "WanderAngleDelta" )
		{
			m_wanderAngleDelta = Reflex::FromString< float >( value );
			return true;
		}
		else if( variable == "WanderCircleDistance" )
		{
			m_wanderCircleDistance = Reflex::FromString< float >( value );
			return true;
		}

		return false;
	}

	void Steering::GetValues( std::vector< std::pair< std::string, std::string > >& values ) const
	{
		for( unsigned i = 0; i < std::size( behaviourNames ); ++i )
			if( m_behaviours.test( i ) )
				values.emplace_back( behaviourNames[i], "true" );

		values.emplace_back( "MaxForce", Reflex::ToString( m_maxForce ) );
		values.emplace_back( "Mass", Reflex::ToString( m_mass ) );

		// Arrival
		if( IsBehaviourSet( Behaviours::Arrival ) )
			values.emplace_back( "SlowingRadius", Reflex::ToString( m_slowingRadius ) );

		float m_slowingRadius = 100.0f;

		// Wander
		if( IsBehaviourSet( Behaviours::Wander ) )
		{
			values.emplace_back( "WanderCircleRadius", Reflex::ToString( m_wanderCircleRadius ) );
			values.emplace_back( "WanderCircleDistance", Reflex::ToString( m_wanderCircleDistance ) );
			values.emplace_back( "WanderAngleDelta", Reflex::ToString( m_wanderAngleDelta ) );
		}
	}

	void Steering::Seek( const sf::Vector2f& target, const float maxVelocity )
	{
		SetBehaviourInternal( Behaviours::Seek );
		m_targetPosition = target;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Flee( const sf::Vector2f& target, const float maxVelocity )
	{
		SetBehaviourInternal( Behaviours::Flee );
		m_targetPosition = target;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Arrival( const sf::Vector2f& target, const float slowingRadius, const float maxVelocity )
	{
		SetBehaviourInternal( Behaviours::Arrival );
		m_targetPosition = target;
		m_slowingRadius = slowingRadius;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Wander( const float circleRadius, const float crcleDistance, const float angleDelta, const float maxVelocity )
	{
		SetBehaviourInternal( Behaviours::Wander );
		m_wanderCircleRadius = circleRadius;
		m_wanderCircleDistance = crcleDistance;
		m_wanderAngleDelta = angleDelta;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Pursue( const Reflex::Object& target, const float slowingRadius, const float maxVelocity )
	{
		SetBehaviourInternal( Behaviours::Pursue );
		m_targetObject = target;
		m_slowingRadius = slowingRadius;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Evade( const Reflex::Object& target, const float maxVelocity )
	{
		SetBehaviourInternal( Behaviours::Evade );
		m_targetObject = target;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::DisableBehaviour( const Behaviours behaviour )
	{ 
		if( ( size_t )behaviour >= 0 && ( size_t )behaviour < ( size_t )Behaviours::NumBehaviours )
			m_behaviours.reset( ( size_t )behaviour );
	}

	void Steering::ClearBehaviours()
	{
		m_behaviours.reset();
	}

	void Steering::SetMaxForce( const float force ) 
	{ 
		m_maxForce = force; 
	}

	void Steering::SetMass( const float newMass )
	{
		m_mass = newMass;
	}

	void Steering::SetBehaviourInternal( const Behaviours behaviour )
	{
		m_behaviours.set( ( size_t )behaviour );
	}

	bool Steering::IsBehaviourSet( const Behaviours behaviour ) const
	{
		return m_behaviours.test( (size_t )behaviour );
	}

}