#pragma once

#include "Precompiled.h"
#include "Component.h"

namespace Reflex::Core { class World; }

namespace Reflex::Systems
{
	using namespace Reflex::Core;

#define RequiresComponent( T ) \
	GetWorld().RegisterComponent< T >(); \
	m_requiredComponents.set( T::GetFamily() );

	class System : private sf::NonCopyable, public sf::Drawable
	{
	public:
		friend class Reflex::Core::World;

		// Constructors / Destructors
		System( Reflex::Core::World& world ) : m_world( world ) { }
		virtual ~System() { }

		ComponentsMask GetRequiredComponents() const { return m_requiredComponents; }
		Reflex::Core::World& GetWorld() { return m_world; }
		const Reflex::Core::World& GetWorld() const { return m_world; }
		const std::vector< Reflex::Object >& GetObjects() const { return m_releventObjects; }

		template< typename... Args, typename Func >
		void ForEachObject( Func f ) const
		{
			for( auto& object : m_releventObjects )
				f( ( object.template GetComponent< Args >() )... );
		}

	protected:
		virtual void RegisterComponents() = 0;
		virtual void Update( const float deltaTime ) { }
		virtual void ProcessEvent( const sf::Event& event ) { }
		virtual void Render( sf::RenderTarget& target, sf::RenderStates states ) const { }
		virtual void RenderUI() { }

		virtual void OnSystemStartup() { }
		virtual void OnSystemShutdown() { }
		virtual void OnComponentAdded( const Reflex::Object& object ) { }
		virtual void OnComponentRemoved( const Reflex::Object& object ) { }
		virtual std::vector< Reflex::Object >::const_iterator GetInsertionIndex( const Object& object ) const { return m_releventObjects.end(); }

	private:
		void draw( sf::RenderTarget& target, sf::RenderStates states ) const final { Render( target, states ); }

	protected:
		std::vector< Reflex::Object > m_releventObjects;
		ComponentsMask m_requiredComponents;

	private:
		Reflex::Core::World& m_world;
	};
}