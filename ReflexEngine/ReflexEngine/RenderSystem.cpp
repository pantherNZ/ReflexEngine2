#include "Precompiled.h"
#include "World.h"
#include "RenderSystem.h"
#include "SFMLObjectComponent.h"
#include "TransformComponent.h"

#include <algorithm>

namespace Reflex::Systems
{
	void RenderSystem::RegisterComponents()
	{
		RequiresComponent( Reflex::Components::SFMLObject );
		RequiresComponent( Reflex::Components::Transform );
	}

	void RenderSystem::Update( const float deltaTime )
	{
		//PROFILE;
		//TODO( "Dirty when a render index changes and only sort when that occurs (or a new component is added)" );
		//std::sort( m_releventObjects.begin(), m_releventObjects.end(), []( const Reflex::Object& left, const Reflex::Object& right )
		//	{
		//		return left.GetTransform()->GetRenderIndex() < right.GetTransform()->GetRenderIndex();
		//	} );
	}

	void RenderSystem::Render( sf::RenderTarget& target, sf::RenderStates states ) const
	{
		PROFILE;
		sf::RenderStates copied_states( states );

		ForEachObject< Reflex::Components::SFMLObject, Reflex::Components::Transform >(
		[&target, &copied_states, &states]( Reflex::Components::SFMLObject::Handle object, Reflex::Components::Transform::Handle transform )
		{
			copied_states.transform = transform->GetWorldTransform();

			switch( object->GetType() )
			{
			case Components::SFMLObjectType::Rectangle:
				target.draw( object->GetRectangleShape(), copied_states );
				break;
			case Components::SFMLObjectType::Convex:
				target.draw( object->GetConvexShape(), copied_states );
				break;
			case Components::SFMLObjectType::Circle:
				target.draw( object->GetCircleShape(), copied_states );
			break;
			case Components::SFMLObjectType::Sprite:
				target.draw( object->GetSprite(), copied_states );
				break;
			case Components::SFMLObjectType::Text:
				target.draw( object->GetText(), copied_states );
				break;
			}
		} );
	}

	std::vector< Reflex::Object >::const_iterator RenderSystem::GetInsertionIndex( const Object& object ) const
	{
		return std::lower_bound( m_releventObjects.begin(), m_releventObjects.end(), object, []( const Reflex::Object& left, const Reflex::Object& right )
			{
				return left.GetTransform()->GetRenderIndex() < right.GetTransform()->GetRenderIndex();
			} );
	}
}