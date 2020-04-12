#include "Precompiled.h"
#include "TileMap.h"
#include "Object.h"
#include "TransformComponent.h"

#define TileMapLogging 0

#if TileMapLogging == 1
	#define TILEMAP_LOG_INFO( x ) LOG_INFO( x )
#else
	#define TILEMAP_LOG_INFO( x ) ((void)0);
#endif

namespace Reflex::Core
{
	TileMap::TileMap( const unsigned cellSize, const unsigned chunkSizeInCells )
		: m_cellSize( cellSize )
		, m_chunkSizeInCells( chunkSizeInCells )
	{
		Reset();
	}

	void TileMap::Reset( const unsigned cellSize, const unsigned chunkSizeInCells )
	{
		m_cellSize = cellSize;
		m_chunkSizeInCells = chunkSizeInCells;
		Reset();
	}

	void TileMap::Reset()
	{
		m_chunkSize = m_cellSize * m_chunkSizeInCells;
		m_spacialChunks.clear();

		Chunk newChunk;
		newChunk.buckets.resize( m_chunkSizeInCells * m_chunkSizeInCells );
		m_spacialChunks[sf::Vector2i( 0, 0 )] = newChunk;
	}

	void TileMap::Repopulate( World& world, const unsigned cellSize, const unsigned chunkSizeInCells )
	{
		m_cellSize = cellSize;
		m_chunkSizeInCells = chunkSizeInCells;
		Repopulate( world );
	}

	void TileMap::Repopulate( World& world )
	{
		Reset();
		TODO( "Repopulate" );
	}

	void TileMap::Insert( const Object& object )
	{
		assert( object && IsValid() );
		if( object && IsValid() )
		{
			const auto position = object.GetComponent< Reflex::Components::Transform >()->GetWorldPosition();
			const auto chunkIdx = ChunkHash( position );
			auto& chunk = m_spacialChunks[chunkIdx];

			if( chunk.buckets.empty() )
				chunk.buckets.resize( m_chunkSizeInCells * m_chunkSizeInCells );

			const auto cellId = GetCellId( position );
			chunk.buckets[cellId].push_back( object );
			chunk.totalObjects++;

			TILEMAP_LOG_INFO( "Insert Position: " << position << ", Chunk: " << chunkIdx << ", cell: " << cellId );
		}
	}

	void TileMap::Insert( const Object& object, const sf::FloatRect& boundary )
	{
		assert( object && IsValid() );
		if( object && IsValid() )
		{
			const auto locTopLeft = CellHash( sf::Vector2f( boundary.left, boundary.top ) );
			const auto locBotRight = CellHash( sf::Vector2f( boundary.left + boundary.width, boundary.top + boundary.height ) );
			std::vector< unsigned > ids;

			for( int x = locTopLeft.x; x <= locBotRight.x; ++x )
			{
				for( int y = locTopLeft.y; y <= locBotRight.y; ++y )
				{
					const auto chunkIdx = sf::Vector2i( x / ( int )m_chunkSizeInCells, y / ( int )m_chunkSizeInCells );
					auto& chunk = m_spacialChunks[chunkIdx];

					if( chunk.buckets.empty() )
						chunk.buckets.resize( m_chunkSizeInCells * m_chunkSizeInCells );

					const auto cellId = y * m_chunkSizeInCells + x;
					chunk.buckets[cellId].push_back( object );
					chunk.totalObjects++;

					TILEMAP_LOG_INFO( "Insert Boundary: Chunk: " << chunkIdx << ", cell: " << cellId );
				}
			}
		}
	}

	void TileMap::Remove( const Object& object )
	{
		assert( object );
		if( object )
			Remove( object, object.GetComponent< Reflex::Components::Transform >()->GetWorldPosition() );
	}

	void TileMap::Remove( const Object& object, const sf::Vector2f& position )
	{
		assert( object );
		if( object )
			Remove( object, ChunkHash( position ), GetCellId( position ) );
	}

	void TileMap::Remove( const Object& object, const sf::Vector2i& chunkIdx, const unsigned cellId )
	{
		assert( object && IsValid() );
		if( object && IsValid() )
		{
			auto& chunk = m_spacialChunks[chunkIdx];
			auto& bucket = chunk.buckets[cellId];

			const auto found = std::find( bucket.begin(), bucket.end(), object );
			assert( found != bucket.end() );
			if( found != bucket.end() )
			{
				TILEMAP_LOG_INFO( "Remove Position: " << position << ", Chunk: " << chunkIdx << ", cell: " << cellId );

				bucket.erase( found );
				chunk.totalObjects--;

				if( chunk.totalObjects == 0 )
					m_spacialChunks.erase( chunkIdx );
			}
		}
	}

	void TileMap::Remove( const Object& object, const sf::FloatRect& boundary )
	{
		assert( object && IsValid() );
		if( object && IsValid() )
		{
			const auto locTopLeft = CellHash( sf::Vector2f( boundary.left, boundary.top ) );
			const auto locBotRight = CellHash( sf::Vector2f( boundary.left + boundary.width, boundary.top + boundary.height ) );
			std::vector< unsigned > ids;

			for( int x = locTopLeft.x; x <= locBotRight.x; ++x )
			{
				for( int y = locTopLeft.y; y <= locBotRight.y; ++y )
				{
					const auto chunkIdx = sf::Vector2i( x / ( int )m_chunkSizeInCells, y / ( int )m_chunkSizeInCells );
					auto& chunk = m_spacialChunks[chunkIdx];

					const auto cellId = y * m_chunkSizeInCells + x;
					auto& container = chunk.buckets[cellId];

					const auto found = std::find( container.begin(), container.end(), object );
					assert( found != container.end() );
					if( found != container.end() )
					{
						TILEMAP_LOG_INFO( "Remove Boundary: , Chunk: " << chunkIdx << ", cell: " << cellId );

						container.erase( found );
						chunk.totalObjects--;

						if( chunk.totalObjects == 0 )
							m_spacialChunks.erase( chunkIdx );
					}
				}
			}
		}
	}

	void TileMap::GetNearby( const Object& object, const float distance, std::vector< Object >& out ) const
	{
		ForEachNearby( object, distance, [&out]( const Object& obj )
		{
			out.push_back( obj );
		} );
	}

	void TileMap::GetNearby( const sf::Vector2f& position, const float distance, std::vector< Object >& out ) const
	{
		ForEachNearby( position, distance, [&out]( const Object& obj )
		{
			out.push_back( obj );
		} );
	}

	void TileMap::GetNearby( const sf::FloatRect& boundary, std::vector< Object >& out ) const
	{
		ForEachNearby( boundary, [&out]( const Object& obj )
		{
			out.push_back( obj );
		} );
	}

	unsigned TileMap::GetCellId( const Object& object ) const
	{
		assert( object );
		return GetCellId( object.GetComponent< Reflex::Components::Transform >()->GetWorldPosition() );
	}

	unsigned TileMap::GetCellId( const sf::Vector2f& position ) const
	{
		const auto chunk = ChunkHash( position );
		const auto startPosition = Reflex::Vector2iToVector2f( chunk ) * (float )m_chunkSize;
		const auto loc = CellHash( position - startPosition );
		return loc.y * m_chunkSizeInCells + loc.x;
	}

	sf::Vector2i TileMap::CellHash( const Object& object ) const
	{
		return CellHash( GetObjectPosition( object ) );
	}

	sf::Vector2i TileMap::ChunkHash( const Object& object ) const
	{
		return ChunkHash( GetObjectPosition( object ) );
	}

	sf::Vector2i TileMap::CellHash( const sf::Vector2f& position ) const
	{
		return sf::Vector2i( int( position.x / m_cellSize ), int( position.y / m_cellSize ) );
	}

	sf::Vector2i TileMap::ChunkHash( const sf::Vector2f& position ) const
	{
		return sf::Vector2i( int( position.x / m_chunkSize ), int( position.y / m_chunkSize ) );
	}

	bool TileMap::IsValid() const
	{
		return m_cellSize && m_chunkSize;
	}

	bool TileMap::IsValid( const BaseObject& object ) const
	{
		return Object( object ).IsValid();
	}

	sf::Vector2f TileMap::GetObjectPosition( const BaseObject& object ) const
	{
		assert( IsValid( object ) );
		return Object( object ).GetTransform()->getPosition();
	}

}