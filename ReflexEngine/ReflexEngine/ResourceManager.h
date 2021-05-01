#pragma once

#include "Utility.h"
#include "Logging.h"

namespace Reflex
{
	typedef unsigned ResourceID;

	namespace Core
	{
		template< typename Resource >
		class ResouceManager;

		typedef ResouceManager< sf::Texture > TextureManager;
		typedef ResouceManager< sf::Font > FontManager;

		template< typename Resource >
		class ResouceManager
		{
		public:
			typedef std::map< ResourceID, std::unique_ptr< Resource > > ResourceMap;

			Resource& LoadResource( const ResourceID id, const std::string& filename );
			void LoadResourceAsync( const ResourceID id, const std::string& filename );

			// Resource loading with a second parameter (for shaders, or textures that specify a bounding rect etc.)
			template< typename Parameter >
			Resource& LoadResource( const ResourceID id, const std::string& filename, const Parameter& secondParam );

			template< typename Parameter >
			void LoadResourceAsync( const ResourceID id, const std::string& filename, const Parameter& secondParam );

			const Resource& GetResource( const ResourceID id ) const;

			template< typename Resource >
			struct ThreadData
			{
				ResouceManager< Resource >* manager;
				ResourceID id;
				std::string filename;
			};

		private:
			friend struct ThreadData< Resource >;

			// Private helper function to insert a new resource into the map and do error checking
			Resource& InsertResource( const ResourceID id, const std::string& filename, std::unique_ptr< Resource > newResource );

			ResourceMap m_resourceMap;
			sf::Mutex m_resourceMutex;
			std::vector< std::pair< sf::Thread, std::future< void > > > m_loadingThreads;
		};

		// Template functions
		template< typename Resource >
		Resource& ResouceManager< Resource >::LoadResource( const ResourceID id, const std::string& filename )
		{
			auto newResource = std::make_unique< Resource >();

			if( !newResource->loadFromFile( filename ) )
				THROW( "Failed to load " << filename );

			return InsertResource( id, filename, std::move( newResource ) );
		}

		template< typename Resource >
		void LoadResourceAsync( void* ptr )
		{
			const auto data = *static_cast< typename ResouceManager< Resource >::ThreadData* >( ptr );

			auto newResource = std::make_unique< Resource >();

			if( !newResource->loadFromFile( data.filename ) )
				THROW( "Failed to load " << data.filename );

			sf::Lock lock( data.manager->m_resourceMutex );
			data.manager->m_resourceMap->insert( std::make_pair( data.id, std::move( newResource ) ) );
		}

		template< typename Resource >
		void ResouceManager< Resource >::LoadResourceAsync( const ResourceID id, const std::string& filename )
		{
			ThreadData data = { this, id, filename };
			std::packaged_task<void()> task( []() { LoadResourceAsync< Resource >( &data ); } );
			sf::Thread thread( &task );

			//m_loadingThreads.emplace_back( &LoadResourceAsync< Resource >, ( void* )&data );
			m_loadingThreads.emplace_back( std::pair( thread, task.get_future() ) );
			m_loadingThreads.back().launch();
		}

		template< typename Resource >
		template< typename Parameter >
		Resource& ResouceManager< Resource >::LoadResource( const ResourceID id, const std::string& filename, const Parameter& secondParam )
		{
			auto newResource = std::make_unique< Resource >();

			if( !newResource->loadFromFile( filename, secondParam ) )
				THROW( "Failed to load " << filename );

			return InsertResource( id, filename, std::move( newResource ) );
		}

		template< typename Resource >
		template< typename Parameter >
		void ResouceManager< Resource >::LoadResourceAsync( const ResourceID id, const std::string& filename, const Parameter& secondParam )
		{

		}

		template< typename Resource >
		const Resource& ResouceManager< Resource >::GetResource( const ResourceID id ) const
		{
			auto found = m_resourceMap.find( id );

			if( found == m_resourceMap.end() )
				LOG_CRIT( "Resource doesn't exist " << ( int )id );

			return *found->second;
		}

		template< typename Resource >
		Resource& ResouceManager< Resource >::InsertResource( const ResourceID id, const std::string& filename, std::unique_ptr< Resource > newResource )
		{
			auto inserted = m_resourceMap.insert( std::make_pair( id, std::move( newResource ) ) );

			//if( !inserted.second )
			//	LOG_CRIT( "Resource already loaded " << filename );

			auto& resource_iter = *inserted.first;
			return *resource_iter.second;
		}
	}
}
