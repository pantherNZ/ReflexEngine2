#pragma once

#include "Precompiled.h"
#include "ResourceManager.h"
#include "Context.h"
#include "System.h"
#include "ComponentAllocator.h"
#include "Object.h"

// Engine class
namespace Reflex 
{
	class Object;

	namespace Components
	{
		class Transform;
		class Camera;
	}
}

namespace Reflex::Core
{
	enum class ObjectFlags
	{
		Deleted,
		NumFlags,
	};

	// World class
	class World : private sf::NonCopyable
	{
	public:
		explicit World( Context context, sf::FloatRect worldBounds );

		void Update( const float deltaTime );
		void ProcessEvent( const sf::Event& event );
		void Render();

		/* Object functions*/
		void CreateROFile( const std::string& name, const Object& object );
		Object CreateObject( const sf::Vector2f& position = sf::Vector2f(), const float rotation = 0.0f, const sf::Vector2f & scale = sf::Vector2f( 1.0f, 1.0f ), const bool attachToRoot = true );
		Object CreateObject( const std::string& objectFile, const sf::Vector2f& position = sf::Vector2f(), const float rotation = 0.0f, const sf::Vector2f & scale = sf::Vector2f( 1.0f, 1.0f ), const bool attachToRoot = true );

		void DestroyObject( Object object );
		void DestroyAllObjects();

		bool IsValidObject( const Object& object ) const;
		bool IsObjectFlagSet( const Object& object, const ObjectFlags flag ) const;
		void SetObjectFlag( const Object& object, const ObjectFlags flag );
		/*---------------*/

		/* Component functions*/
		template< class T, typename... Args >
		Handle< T > ObjectAddComponent( const Object& object, Args&& ... args );

		// Adds an empty component (no args passed, non templated) - Used to add components from .ro files
		Reflex::Components::BaseComponent* ObjectAddEmptyComponent( const Object& object, const size_t family );

		template< class T >
		T* ObjectGetComponent( const Object& object ) const;

		Reflex::Components::BaseComponent* ObjectGetComponent( const Object& object, const size_t family ) const;

		ComponentsMask ObjectGetComponentFlags( const Object& object ) const;

		// Returns true if a component was removed
		template< class T >
		bool ObjectRemoveComponent( const Object& object );

		void ObjectRemoveAllComponents( const Object& object );

		template< class T >
		bool ObjectHasComponent( const Object& object ) const;

		bool ObjectHasComponent( const Object& object, const size_t family ) const;

		// Returns true if the register resulted in a new component being allocated
		template< class T >
		bool RegisterComponent();
		/*---------------*/

		/* System functions*/
		template< class T, typename... Args >
		T* AddSystem( Args&& ... args );

		template< class T >
		T* GetSystem();

		template< class T >
		void RemoveSystem();
		/*---------------*/

		// Utility and helper functions
		template< typename Func >
		void ForEachObject( Func function );

		sf::RenderWindow& GetWindow() { return m_context.window; }
		const sf::RenderWindow& GetWindow() const { return m_context.window; }
		TextureManager& GetTextureManager() { return m_context.textureManager; }
		FontManager& GetFontManager() { return m_context.fontManager; }

		sf::FloatRect GetBounds() const;
		Reflex::Handle< Reflex::Components::Transform > GetSceneRoot() const;

		void OnComponentAdded( const Object& object );
		void OnComponentRemoved( const Object& object );

		bool IsActiveCamera( const Reflex::Handle< Reflex::Components::Camera >& camera ) const;
		void SetActiveCamera( const Reflex::Handle< Reflex::Components::Camera >& camera );
		Reflex::Handle< Reflex::Components::Camera > GetActiveCamera() const { return m_activeCamera; }

		sf::Vector2f GetMousePosition( const Reflex::Handle< Reflex::Components::Camera >& camera = Reflex::Handle< Reflex::Components::Camera >() ) const;
		sf::Vector2f RandomWindowPosition( const float margin = 0.0f ) const;

	protected:
		void Setup();
		Object ObjectFromIndex( const unsigned index );

		bool IsObjectFlagSet( const std::uint32_t objectIndex, const ObjectFlags flag ) const;
		void SetObjectFlag( const std::uint32_t objectIndex, const ObjectFlags flag );

	private:
		World() = delete;

	protected:
		Context m_context;
		sf::View m_worldView;
		sf::FloatRect m_worldBounds;

		// Object data
		struct ObjectData
		{
			// Separate vectors are more efficient for fast lookup for individual data (less data to pull into cache), we rarely need info from more than 1 at the same time
			std::vector< std::bitset< ( size_t )ObjectFlags::NumFlags > > flags;
			std::vector< ComponentsMask > components;
			std::vector< unsigned > counters;
		};

		ObjectData m_objects;

		// Storage for all components
		std::vector< std::unique_ptr< ComponentAllocatorBase > > m_components;
		//std::vector<  > m_componentEvents;
		std::unordered_map< std::string, unsigned > m_componentNameToIndex;
		std::queue< unsigned > m_freeList;

		// List of systems, indexed by their type, storage for all systems
		std::unordered_map< Type, std::unique_ptr< Reflex::Systems::System > > m_systems;

		// Tilemap which stores object handles in the world in an efficient spacial hash map
		//TileMap m_tileMap;
		Object m_sceneGraphRoot;
		Object m_activeCamera;
	};

	// Template functions
	template< class T, typename... Args >
	Handle< T >  World::ObjectAddComponent( const Object& object, Args&& ... args )
	{
		const auto family = T::GetFamily();

		if( family >= m_components.size() )
		{
			assert( family == m_components.size() );
			m_components.push_back( std::unique_ptr< ComponentAllocatorBase >( new ComponentAllocator< T >( std::max( 128U, object.GetIndex() ) ) ) );
		}
		else
			m_components[family]->ExpandToFit( object.GetIndex() + 1 );

		auto* allocator = static_cast< ComponentAllocator< T >* >( m_components[family].get() );
		auto newComponent = static_cast< T* >( allocator->Construct( object.GetIndex(), object, std::forward<Args>( args )... ) );
		m_objects.components[object.GetIndex()].set( family );
		OnComponentAdded( object );
		newComponent->OnConstructionComplete();
		return Handle< T >( object );
	}

	template< class T >
	T* World::ObjectGetComponent( const Object& object ) const
	{
		return static_cast< T* >( ObjectGetComponent( object, T::GetFamily() ) );
	}

	template< class T >
	bool World::ObjectRemoveComponent( const Object& object )
	{
		assert( IsValidObject( object ) );
		const auto family = T::GetFamily();

		if( !ObjectHasComponent< T >( object ) )
			return false;

		m_objects.components[object.GetIndex()].reset( family );
		static_cast< ComponentAllocator< T >* >( m_components[family].get() )->Destroy( object.GetIndex() );
		OnComponentRemoved( object );
		return true;
	}

	template< class T >
	bool World::ObjectHasComponent( const Object& object ) const
	{
		return ObjectHasComponent( object, ( size_t )T::GetFamily() );
	}

	template< class T >
	bool World::RegisterComponent()
	{
		const auto family = T::GetFamily();

		if( family >= m_components.size() )
		{
			assert( family == m_components.size() );
			m_componentNameToIndex[T::GetComponentName()] = m_components.size();
			m_components.push_back( std::unique_ptr< ComponentAllocatorBase >( new ComponentAllocator< T >( 128 ) ) );
			return true;
		}
		return false;
	}

	template< class T, typename... Args >
	T* World::AddSystem( Args&& ... args )
	{
		const auto type = Type( typeid( T ) );

		if( m_systems.find( type ) != m_systems.end() )
		{
			LOG_CRIT( "Trying to add a system that has already been added!" );
			return nullptr;
		}

		auto system = std::make_unique< T >( *this, std::forward< Args >( args )... );

		std::vector< Type > requiredComponentTypes;

		// Register components
		system->RegisterComponents();

		// Look for any existing objects that match what this system requires and add them to the system's list
		for( unsigned i = 0; i < m_objects.components.size(); ++i )
		{		
			if( IsObjectFlagSet( i, ObjectFlags::Deleted ) )
				continue;

			auto mask = m_objects.components[i];

			if( ( mask & system->GetRequiredComponents() ) == system->GetRequiredComponents() )
			{
				const auto insertionIter = system->GetInsertionIndex( ObjectFromIndex( i ) );
				system->m_releventObjects.insert( insertionIter, ObjectFromIndex( i ) );
			}
		}

		auto result = m_systems.insert( std::make_pair( type, std::move( system ) ) );
		assert( result.second );

		result.first->second->OnSystemStartup();

		return ( T* )result.first->second.get();
	}

	template< class T >
	void World::RemoveSystem()
	{
		const auto systemType = Type( typeid( T ) );

		for( auto iter = m_systems.begin(); iter != m_systems.end(); ++iter )
		{
			if( systemType == Type( typeid( iter->first.get() ) ) )
			{
				iter->first->OnSystemShutdown();
				iter->first.release();
				m_systems.erase( iter );
				break;
			}
		}
	}

	template< class T >
	T* World::GetSystem()
	{
		const auto systemType = Type( typeid( T ) );

		for( auto iter = m_systems.begin(); iter != m_systems.end(); ++iter )
			if( systemType == iter->first )
				return (T* )iter->second.get();

		return nullptr;
	}


	template< typename Func >
	void World::ForEachObject( Func function )
	{
		for( unsigned i = 1; i < m_objects.counters.size(); ++i )
			function( Reflex::Object( *this, i, m_objects.counters[i] ) );
	}
}