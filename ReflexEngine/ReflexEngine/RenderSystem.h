#pragma once

#include "System.h"

namespace Reflex::Systems
{
	class RenderSystem : public System
	{
	public:
		using System::System;

		void RegisterComponents() final;
		void Update( const float deltaTime ) final;
		void Render( sf::RenderTarget& target, sf::RenderStates states ) const final;
		void OnSystemStartup() final {}
		void OnSystemShutdown() final { }

		std::vector< Reflex::Object >::const_iterator GetInsertionIndex( const Object& object ) const final;
	};
}