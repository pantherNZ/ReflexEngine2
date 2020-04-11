#include "Precompiled.h"
#include "TileMap.h"
#include "Object.h"
#include "TransformComponent.h"

TODO( "Allow TileMap work when a pos outside the bounds is entered - dynamically resize the array" )

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

	void TileMap::Insert( const Object& obj )
	{
		if( obj && IsValid() )
		{
			const auto position = obj.GetComponent< Reflex::Components::Transform >()->GetWorldPosition();
			auto& chunk = m_spacialChunks[ChunkHash( position )];
			chunk.buckets[GetCellId( position )].push_back( obj );
			chunk.totalObjects++;
		}
	}

	void TileMap::Insert( const Object& obj, const sf::FloatRect& boundary )
	{
		if( obj && IsValid() )
		{
			const auto locTopLeft = CellHash( sf::Vector2f( boundary.left, boundary.top ) );
			const auto locBotRight = CellHash( sf::Vector2f( boundary.left + boundary.width, boundary.top + boundary.height ) );
			std::vector< unsigned > ids;

			for( int x = locTopLeft.x; x <= locBotRight.x; ++x )
			{
				for( int y = locTopLeft.y; y <= locBotRight.y; ++y )
				{
					const auto chunkIdx = sf::Vector2i( x / (int )m_chunkSizeInCells, y / (int )m_chunkSizeInCells );
					auto& chunk = m_spacialChunks[chunkIdx];
					chunk.buckets[y * m_chunkSizeInCells + x].push_back( obj );
					chunk.totalObjects++;
				}
			}
		}
	}

	void TileMap::Remove( const Object& obj )
	{
		if( obj && IsValid() )
		{
			const auto position = obj.GetComponent< Reflex::Components::Transform >()->GetWorldPosition();
			const auto chunkIdx = ChunkHash( position );
			auto& chunk = m_spacialChunks[chunkIdx];
			auto& bucket = chunk.buckets[GetCellId( position )];
			const auto found = std::find( bucket.begin(), bucket.end(), obj );
			assert( found != bucket.end() );
			if( found != bucket.end() )
			{
				bucket.erase( found );
				chunk.totalObjects--;

				if( chunk.totalObjects == 0 )
					m_spacialChunks.erase( chunkIdx );
			}
		}
	}

	void TileMap::Remove( const Object& obj, const sf::FloatRect& boundary )
	{
		if( obj && IsValid() )
		{
			const auto locTopLeft = CellHash( sf::Vector2f( boundary.left, boundary.top ) );
			const auto locBotRight = CellHash( sf::Vector2f( boundary.left + boundary.width, boundary.top + boundary.height ) );
			std::vector< unsigned > ids;

			for( int x = locTopLeft.x; x <= locBotRight.x; ++x )
			{
				for( int y = locTopLeft.y; y <= locBotRight.y; ++y )
				{
					const auto chunkIdx = sf::Vector2i( x / (int )m_chunkSizeInCells, y / (int )m_chunkSizeInCells );
					auto& chunk = m_spacialChunks[chunkIdx];
					auto& container = chunk.buckets[y * m_chunkSizeInCells + x];
					const auto found = std::find( container.begin(), container.end(), obj );
					assert( found != container.end() );
					if( found != container.end() )
					{
						container.erase( found );
						chunk.totalObjects--;

						if( chunk.totalObjects == 0 )
							m_spacialChunks.erase( chunkIdx );
					}
				}
			}
		}
	}

	/*void TileMap::GetNearby( const Object& obj, std::vector< Object >& out ) const
	{
		ForEachNearby( obj, [&out]( const Object& obj )
		{
			out.push_back( obj );
		} );
	}

	void TileMap::GetNearby( const sf::Vector2f& position, std::vector< Object >& out ) const
	{
		ForEachNearby( position, [&out]( const Object& obj )
		{
			out.push_back( obj );
		} );
	}

	void TileMap::GetNearby( const Object& obj, const sf::FloatRect& boundary, std::vector< Object >& out ) const
	{
		ForEachNearby( obj, boundary, [&out]( const Object& obj )
		{
			out.push_back( obj );
		} );
	}*/

	unsigned TileMap::GetCellId( const Object& obj ) const
	{
		assert( obj );
		return GetCellId( obj.GetComponent< Reflex::Components::Transform >()->GetWorldPosition() );
	}

	unsigned TileMap::GetCellId( const sf::Vector2f& position ) const
	{
		const auto chunk = ChunkHash( position );
		const auto startPosition = Reflex::Vector2iToVector2f( chunk ) * (float )m_chunkSize;
		const auto loc = CellHash( position - startPosition );
		return loc.y * m_chunkSizeInCells + loc.x;
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
}