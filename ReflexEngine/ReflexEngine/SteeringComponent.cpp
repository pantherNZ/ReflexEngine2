#include "Precompiled.h"
#include "SteeringComponent.h"
#include "TransformComponent.h"
#include "SFMLObjectComponent.h"

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
			"Alignment",
			"Cohesion",
			"Separation",
		};

		static_assert( std::size( behaviourNames ) == ( size_t )Steering::Behaviours::NumBehaviours );
	}

	bool Steering::SetValue( const std::string& variable, const std::string& value )
	{
		TrySetValue( "MaxForce", m_maxForce );
		TrySetValue( "Mass", m_mass );
		TrySetValue( "SlowingRadius", m_slowingRadius );
		TrySetValue( "WanderCircleRadius", m_wanderCircleRadius );
		TrySetValue( "WanderAngleDelta", m_wanderAngleDelta );
		TrySetValue( "WanderCircleDistance", m_wanderCircleDistance );
		TrySetValue( "NeighbourRange", m_neighbourRange );
		TrySetValue( "AlignmentForce", m_alignmentForce );
		TrySetValue( "CohesionForce", m_cohesionForce );
		TrySetValue( "SeparationForce", m_separationForce );

		for( unsigned i = 0; i < std::size( behaviourNames ); ++i )
		{
			if( variable == behaviourNames[i] )
			{
				m_behaviours.set( i, Reflex::FromString< bool >( value ) );
				return true;
			}
		}

		return false;
	}

	void Steering::GetValues( std::vector< std::pair< std::string, std::string > >& values ) const
	{
		for( unsigned i = 0; i < std::size( behaviourNames ); ++i )
			if( m_behaviours.test( i ) )
				values.emplace_back( behaviourNames[i], "true" );

		GetValue( "MaxForce", m_maxForce );
		GetValue( "Mass", m_maxForce );
		TryGetValue( "SlowingRadius", m_slowingRadius, IsBehaviourSet( Behaviours::Arrival ) );

		// Wander
		if( IsBehaviourSet( Behaviours::Wander ) )
		{
			GetValue( "WanderCircleRadius", m_wanderCircleRadius );
			GetValue( "WanderCircleDistance", m_wanderCircleDistance );
			GetValue( "WanderAngleDelta", m_wanderAngleDelta );
		}

		// Flocking
		if( IsBehaviourSet( Behaviours::Alignment ) ||
			IsBehaviourSet( Behaviours::Cohesion ) ||
			IsBehaviourSet( Behaviours::Separation ) )
		{
			GetValue( "NeighbourRange", m_neighbourRange );
			TryGetValue( "AlignmentForce", m_alignmentForce, IsBehaviourSet( Behaviours::Alignment ) );
			TryGetValue( "CohesionForce", m_cohesionForce, IsBehaviourSet( Behaviours::Cohesion ) );
			TryGetValue( "SeparationForce", m_separationForce, IsBehaviourSet( Behaviours::Separation ) );
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

	void Steering::Evade( const Reflex::Object& target, const float ignoreDistance, const float maxVelocity )
	{
		SetBehaviourInternal( Behaviours::Evade );
		m_targetObject = target;
		m_ignoreDistance = ignoreDistance;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Alignment( const float neighbourRange, const float alignmentForce, const float maxVelocity )
	{
		SetBehaviourInternal( Behaviours::Alignment );
		m_neighbourRange = neighbourRange;
		m_alignmentForce = alignmentForce;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Cohesion( const float neighbourRange, const float cohesionForce, const float maxVelocity )
	{
		SetBehaviourInternal( Behaviours::Cohesion );
		m_neighbourRange = neighbourRange;
		m_cohesionForce = cohesionForce;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Separation( const float neighbourRange, const float separationForce, const float maxVelocity )
	{
		SetBehaviourInternal( Behaviours::Separation );
		m_neighbourRange = neighbourRange;
		m_separationForce = separationForce;
		GetObject().GetTransform()->SetMaxVelocity( maxVelocity );
	}

	void Steering::Flocking( const float neighbourRange, const float alignmentForce, const float cohesionForce, const float separationForce, const float maxVelocity )
	{
		Alignment( neighbourRange, alignmentForce, maxVelocity );
		Cohesion( neighbourRange, cohesionForce, maxVelocity );
		Separation( neighbourRange, separationForce, maxVelocity );
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