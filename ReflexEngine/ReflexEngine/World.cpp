#include "Precompiled.h"

#include "World.h"
#include "Object.h"

#include "TransformComponent.h"
#include "CameraComponent.h"
#include "InteractableComponent.h"
#include "SFMLObjectComponent.h"
#include "GridComponent.h"

#include "RenderSystem.h"
#include "InteractableSystem.h"
#include "MovementSystem.h"
#include "CameraSystem.h"
#include "SteeringSystem.h"

namespace Reflex::Core
{
	World::World( Context context, sf::FloatRect worldBounds )
		: m_context( context )
		, m_worldView( context.window.getDefaultView() )
		, m_worldBounds( worldBounds )
		//, m_tileMap( m_worldBounds )
	{
		Setup();
	}

	void World::Setup()
	{
		RegisterComponent< Reflex::Components::Transform >();
		RegisterComponent< Reflex::Components::Interactable >();
		RegisterComponent< Reflex::Components::SFMLObject >();
		RegisterComponent< Reflex::Components::Grid >();
		RegisterComponent< Reflex::Components::Camera >();
		RegisterComponent< Reflex::Components::Steering >();

		AddSystem< Reflex::Systems::RenderSystem >();
		AddSystem< Reflex::Systems::InteractableSystem >();
		AddSystem< Reflex::Systems::MovementSystem >();
		AddSystem< Reflex::Systems::CameraSystem >();
		AddSystem< Reflex::Systems::SteeringSystem >();

		m_sceneGraphRoot = CreateObject( sf::Vector2f( 0.0f, 0.0f ), 0.0f, sf::Vector2f( 1.0f, 1.0f ), false );
	}

	void World::Update( const float deltaTime )
	{
		PROFILE;
		// Update systems
		for( auto& system : m_systems )
			system.second->Update( deltaTime );
	}

	void World::ProcessEvent( const sf::Event& event )
	{
		PROFILE;
		for( auto& system : m_systems )
			system.second->ProcessEvent( event );
	}

	void World::Render()
	{
		PROFILE;
		GetWindow().setView( m_activeCamera ? *m_activeCamera.GetComponent< Reflex::Components::Camera >() : m_worldView );

		for( auto& system : m_systems )
		{
			system.second->RenderUI();
			GetWindow().draw( *system.second );
		}
	}

	Object World::CreateObject( const sf::Vector2f& position, const float rotation, const sf::Vector2f& scale, const bool attachToRoot /*= true*/ )
	{
		unsigned index = 0;

		if( !m_freeList.empty() )
		{
			index = m_freeList.back();
			m_objects.flags[index].reset();
			m_freeList.pop();
		}
		else
		{
			m_objects.components.emplace_back();
			m_objects.flags.emplace_back();
			m_objects.counters.emplace_back();
			index = ( unsigned )m_objects.components.size() - 1;

			for( auto& allocator : m_components )
				allocator->ExpandToFit( index + 1 );
		}

		Object newObject = ObjectFromIndex( index );
		const auto transform = newObject.AddComponent< Reflex::Components::Transform >( position, rotation, scale );
		//GetTileMap().Insert( newObject, sf::FloatRect( transform->GetWorldPosition(), sf::Vector2f( 0.0f, 0.0f ) ) );

		if( attachToRoot )
		{
			assert( m_sceneGraphRoot.IsValid() );
			m_sceneGraphRoot.GetTransform()->AttachChild( newObject );
		}

		SetObjectFlag( newObject, ObjectFlags::ConstructionComplete );

		return newObject;
	}

	Object World::CreateObject( const std::string& objectFile, const sf::Vector2f& position, const float rotation, const sf::Vector2f& scale, const bool attachToRoot /*= true*/ )
	{
		auto newObject = CreateObject( position, rotation, scale, attachToRoot );
		
		const auto dot = objectFile.rfind( '.' );

		if( dot == std::string::npos )
			THROW( "Invalid object file name: " << objectFile );

		const auto fileType = objectFile.substr( dot );

		if( fileType.substr( 0, 3 ) != ".ro" )
			THROW( "Invalid object file name (must be .ro): " << objectFile );

		//if( fileType.length() > 3 )
		//	const auto version = std::atoi( &*fileType.rbegin() );

		std::ifstream input( objectFile );

		if( input.fail() )
			THROW( "Invalid object file name: " << objectFile );

		Json::CharReaderBuilder reader;
		Json::Value obj;
		std::string errs;

		if( !Json::parseFromStream( reader, input, &obj, &errs ) )
			THROW( "Invalid object file data: " << objectFile );

		const auto& components = obj["Components"];

		for( unsigned i = 0; i < components.getMemberNames().size(); ++i )
		{
			const auto componentName = components.getMemberNames()[i];
			const auto found = m_componentNameToIndex.find( componentName );

			if( found == m_componentNameToIndex.end() )
				THROW( "Invalid component name in file: " << objectFile << " , component: " << componentName );

			Reflex::Components::BaseComponent* component = nullptr;
				
			if( componentName == Reflex::Components::Transform::GetComponentName() )
				component = ObjectGetComponent( newObject, found->second );
			else
				component = ObjectAddEmptyComponent( newObject, found->second );
				
			const auto& componentData = components[componentName];
			for( unsigned j = 0; j < componentData.getMemberNames().size(); ++j )
			{
				const auto variable = componentData.getMemberNames()[j];
				const auto value = componentData[variable].asString();
				if( !component->SetValue( variable, value ) )
					THROW( "Invalid component variable name in file: " << objectFile << " , component: " << componentName << ", variable: " << variable );
			}
		}

		return newObject;
	}

	void World::CreateROFile( const std::string& name, const Object& object )
	{
		auto path = name;

		const auto dot = path.rfind( '.' );

		if( dot == std::string::npos )
			path = path.append( ".ro" );
		else if( path.substr( 0, 3 ) != ".ro" )
			THROW( "Invalid object file name (must be .ro): " << name );

		Json::Value jsonOut;
		Json::Value components;

		const auto flags = m_objects.components[object.GetIndex()];

		for( unsigned i = 0; i < flags.size(); ++i )
		{
			if( !flags.test( i ) )
				continue;

			Json::Value data;
			std::vector< std::pair< std::string, std::string > > values;
			ObjectGetComponent( object, i )->GetValues( values );

			for( const auto& value : values )
				data[value.first] = value.second;

			const auto componentName = std::find_if( m_componentNameToIndex.begin(), m_componentNameToIndex.end(), [&]( const auto& pair )
			{
				return pair.second == i;
			} );

			assert( componentName != m_componentNameToIndex.end() );
			components[componentName->first] = data;
		}

		jsonOut["Components"] = components;

		Json::StreamWriterBuilder builder;
		builder["commentStyle"] = "None";
		builder["indentation"] = "   ";

		std::ofstream outputFileStream( path );
		builder.newStreamWriter()->write( jsonOut, &outputFileStream );
	}

	Reflex::Components::BaseComponent* World::ObjectAddEmptyComponent( const Object& object, const size_t family )
	{
		assert( IsValidObject( object ) );
		assert( !m_objects.components[object.GetIndex()].test( family ) );
		assert( family < m_components.size() );

		m_components[family]->ExpandToFit( object.GetIndex() + 1 );

		//std::static_pointer_cast< 
		auto* newComponent = static_cast< Reflex::Components::BaseComponent* >( m_components[family].get()->ConstructEmpty( object.GetIndex(), object ) );
		m_objects.components[object.GetIndex()].set( family );
		OnComponentAdded( object );
		newComponent->OnConstructionComplete();
		return newComponent;
	}

	Reflex::Components::BaseComponent* World::ObjectGetComponent( const Object& object, const size_t family ) const
	{
		assert( IsValidObject( object ) );
		if( !ObjectHasComponent( object, family ) )
			return nullptr;
		return static_cast< Reflex::Components::BaseComponent* >( m_components[family]->Get( object.GetIndex() ) );
	}

	bool World::ObjectHasComponent( const Object& object, const size_t family ) const
	{
		return m_objects.components[object.GetIndex()].test( family );
	}

	void World::DestroyObject( const Object& object )
	{
		if( IsObjectFlagSet( object, ObjectFlags::Deleted ) )
			return;

		ObjectRemoveAllComponents( object );
		m_objects.flags[object.GetIndex()] = 0;
		SetObjectFlag( object, ObjectFlags::Deleted );
		m_objects.counters[object.GetIndex()]++;
	}

	void World::DestroyAllObjects()
	{
		for( unsigned i = 0; i < m_objects.counters.size(); ++i )
			if( !IsObjectFlagSet( i, ObjectFlags::Deleted ) )
				DestroyObject( ObjectFromIndex( i ) );
	}

	bool World::IsValidObject( const Object& object ) const
	{
		return object.GetIndex() < m_objects.counters.size() && object.GetCounter() == m_objects.counters[object.GetIndex()];
	}

	bool World::IsObjectFlagSet( const Object& object, const ObjectFlags flag ) const
	{
		assert( IsValidObject( object ) );
		return IsObjectFlagSet( object.GetIndex(), flag );
	}

	bool World::IsObjectFlagSet( const std::uint32_t objectIndex, const ObjectFlags flag ) const
	{
		return m_objects.flags[objectIndex].test( (size_t )flag );
	}

	void World::SetObjectFlag( const Object& object, const ObjectFlags flag )
	{
		assert( IsValidObject( object ) );
		SetObjectFlag( object.GetIndex(), flag );
	}

	void World::SetObjectFlag( const std::uint32_t objectIndex, const ObjectFlags flag )
	{
		m_objects.flags[objectIndex].set( (size_t )flag );
	}

	Reflex::ComponentsMask World::ObjectGetComponentFlags( const Object& object ) const
	{
		assert( IsValidObject( object ) );
		return m_objects.components[object.GetIndex()];
	}

	void World::ObjectRemoveAllComponents( const Object& object )
	{
		assert( IsValidObject( object ) );
		m_objects.components[object.GetIndex()].reset();
		OnComponentRemoved( object );
	}

	sf::FloatRect World::GetBounds() const
	{
		return m_worldBounds;
	}

	Reflex::Components::Transform::Handle World::GetSceneRoot() const
	{
		return m_sceneGraphRoot;
	}

	void World::OnComponentAdded( const Object& object )
	{
		assert( IsValidObject( object ) );

		// Here we want to check if we should add this component to any systems
		for( const auto& system : m_systems )
		{
			if( ( ObjectGetComponentFlags( object ) & system.second->GetRequiredComponents() ) != system.second->GetRequiredComponents() )
				continue;

			if( Reflex::Contains( system.second->m_releventObjects, object ) )
				continue;

			const auto insertionIter = system.second->GetInsertionIndex( object );
			system.second->m_releventObjects.insert( insertionIter, object );
			system.second->OnComponentAdded( object );
		}
	}

	void World::OnComponentRemoved( const Object& object )
	{
		for( const auto& system : m_systems )
		{
			if( ( ObjectGetComponentFlags( object ) & system.second->GetRequiredComponents() ) == system.second->GetRequiredComponents() )
				continue;

			const auto found = Reflex::Find( system.second->m_releventObjects, object );
			if( found == system.second->m_releventObjects.end() )
				continue;

			system.second->OnComponentRemoved( *found );
			system.second->m_releventObjects.erase( found );
		}
	}

	bool World::IsActiveCamera( const Reflex::Components::Camera::Handle& camera ) const
	{
		return camera && m_activeCamera == camera->GetObject();
	}

	void World::SetActiveCamera( const Reflex::Components::Camera::Handle& camera )
	{
		if( !camera )
		{
			LOG_CRIT( "World::SetActiveCamera called with an invalid camera component handle" );
			return;
		}

		m_activeCamera = camera->GetObject();
		GetWindow().setView( *m_activeCamera.GetComponent< Reflex::Components::Camera >() );
	}

	sf::Vector2f World::GetMousePosition( const Reflex::Components::Camera::Handle& camera ) const
	{
		if( camera )
			return GetWindow().mapPixelToCoords( sf::Mouse::getPosition( GetWindow() ), *camera );
		return GetWindow().mapPixelToCoords( sf::Mouse::getPosition( GetWindow() ) );
	}

	Reflex::Object World::ObjectFromIndex( const unsigned index )
	{
		return Reflex::Object( *this, uint32_t( index ), uint32_t( m_objects.counters[index] ) );
	}

	sf::Vector2f World::RandomWindowPosition( const float margin /*= 0.0f */ ) const
	{
		return sf::Vector2f(
			Reflex::RandomFloat( margin, GetWindow().getSize().x - margin * 2.0f ),
			Reflex::RandomFloat( margin, GetWindow().getSize().y - margin * 2.0f ) );
	}

	std::vector< Object > World::GetObjects()
	{
		std::vector< Object > output;
		output.reserve( m_objects.flags.size() );

		for( unsigned i = 1; i < m_objects.flags.size(); ++i )
			if( !IsObjectFlagSet( i, ObjectFlags::Deleted ) )
				output.push_back( ObjectFromIndex( i ) );

		return output;
	}
}