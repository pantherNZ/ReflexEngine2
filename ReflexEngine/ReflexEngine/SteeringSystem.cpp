#include "Precompiled.h"
#include "SteeringSystem.h"
#include "TransformComponent.h"

namespace Reflex::Systems
{
	void SteeringSystem::RegisterComponents()
	{
		RequiresComponent( Reflex::Components::Steering );
	}

	void SteeringSystem::Update( const float deltaTime )
	{
		PROFILE;
		ForEachObject< Reflex::Components::Steering >( [&]( const Reflex::Components::Steering::Handle& boid )
			{
				Integrate( boid, deltaTime );
			} );
	}

	void SteeringSystem::Integrate( const Steering::Handle& boid, const float deltaTime ) const
	{
		PROFILE;
		auto transform = boid->GetObject().GetTransform();

		if( boid->m_maxForce <= 0.0f || transform->GetMaxVelocity() <= 0.0f )
			return;

		boid->m_desired = Steering( boid );
		boid->m_steering = boid->m_desired - transform->GetVelocity();

		boid->m_steering = Reflex::Truncate( boid->m_steering, boid->m_maxForce );
		boid->m_steering /= boid->m_mass;

		transform->SetVelocity( transform->GetVelocity() + boid->m_steering );
	}

	sf::Vector2f SteeringSystem::Steering( const Steering::Handle& boid ) const
	{
		sf::Vector2f steering;
		if( boid->IsBehaviourSet( Steering::Behaviours::Seek ) )		steering += Seek( boid, boid->m_targetPosition );
		if( boid->IsBehaviourSet( Steering::Behaviours::Flee ) )		steering += Flee( boid, boid->m_targetPosition );
		if( boid->IsBehaviourSet( Steering::Behaviours::Arrival ) )		steering += Arrival( boid, boid->m_targetPosition );
		if( boid->IsBehaviourSet( Steering::Behaviours::Wander ) )		steering += Wander( boid );
		if( boid->IsBehaviourSet( Steering::Behaviours::Pursue ) )		steering += Pursue( boid, boid->m_targetObject );
		if( boid->IsBehaviourSet( Steering::Behaviours::Evade ) )		steering += Evade( boid, boid->m_targetObject );
		return steering;
	}

	sf::Vector2f SteeringSystem::Seek( const Steering::Handle& boid, const sf::Vector2f& target ) const
	{
		const auto pos = boid->GetTransform()->getPosition();
		if( target == pos )
			return {};
		return Reflex::ScaleTo( target - pos, boid->m_maxForce );
	}

	sf::Vector2f SteeringSystem::Flee( const Steering::Handle& boid, const sf::Vector2f& target ) const
	{
		return -Seek( boid, target );
	}

	sf::Vector2f SteeringSystem::Arrival( const Steering::Handle& boid, const sf::Vector2f& target ) const
	{
		const auto direction = target - boid->GetTransform()->getPosition();
		const auto length = Reflex::GetMagnitude( direction );

		if( length <= 0.00001f )
			return {};

		const auto speedModifier = length < boid->m_slowingRadius ? length / boid->m_slowingRadius : 1.0f;
		return ( direction / length ) * boid->GetTransform()->GetMaxVelocity() * speedModifier;
	}

	sf::Vector2f SteeringSystem::Wander( const Steering::Handle& boid ) const
	{
		PROFILE;
		assert( boid->m_wanderCircleRadius > 0.0f );
		if( boid->m_wanderCircleRadius <= 0.0f )
			return {};

		auto transform = boid->GetObject().GetTransform();

		if( transform->GetVelocity().x == 0.0f && transform->GetVelocity().y == 0.0f )
			transform->SetVelocity( Reflex::RandomUnitVector() );

		boid->m_currentWanderAngle += Reflex::RandomFloat( -boid->m_wanderAngleDelta / 2.0f, boid->m_wanderAngleDelta / 2.0f );
		const auto circleCentre = boid->GetTransform()->getPosition() + Reflex::ScaleTo( transform->GetVelocity(), boid->m_wanderCircleDistance );
		const auto steeringPos = circleCentre + Reflex::VectorFromAngle( boid->m_currentWanderAngle, boid->m_wanderCircleRadius );

		PROFILE_NAME( Seek );
		return Seek( boid, steeringPos );
	}

	sf::Vector2f SteeringSystem::Pursue( const Steering::Handle& boid, const Object& target, const bool useArrival ) const
	{
		const auto speed = Reflex::GetMagnitude( boid->GetObject().GetTransform()->GetVelocity() );
		const auto time = speed <= 0.0001f ? 0.0f : ( Reflex::GetDistance( boid->GetTransform()->getPosition(), target.GetTransform()->getPosition() ) / speed );
		const auto targetPos = target.GetTransform()->getPosition() + target.GetTransform()->GetVelocity() * time;
		return useArrival ? Arrival( boid, targetPos ) : Seek( boid, targetPos );
	}

	sf::Vector2f SteeringSystem::Evade( const Steering::Handle& boid, const Object& target ) const
	{
		return -Pursue( boid, target, false );
	}
}