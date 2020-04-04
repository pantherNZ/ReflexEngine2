#pragma once

#include "System.h"
#include "TransformComponent.h"

namespace Reflex::Systems
{
	class RenderSystem : public System, public EventReceiver
	{
	public:
		using System::System;

		void RegisterComponents() final;
		void OnComponentAdded( const Reflex::Object& object ) final;
		void Render( sf::RenderTarget& target, sf::RenderStates states ) const final;
		void OnSystemStartup() final {}
		void OnSystemShutdown() final { }
		void OnRenderIndexChanged( const Components::Transform::RenderIndexChangedEvent& e );

		std::vector< Reflex::Object >::const_iterator GetInsertionIndex( const Object& object ) const final;
	};
}