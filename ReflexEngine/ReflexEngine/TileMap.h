#pragma once

#include "Precompiled.h"
#include "BaseObject.h"

namespace Reflex { class Object; }

namespace Reflex::Core
{
	class TileMap : sf::NonCopyable
	{
		friend class Reflex::Components::Transform;

	public:
		explicit TileMap( const unsigned cellSize, const unsigned chunkSizeInCells );

		void Reset( const unsigned cellSize, const unsigned chunkSizeInCells );
		void Reset();

		void Repopulate( World& world, const unsigned cellSize, const unsigned chunkSizeInCells );
		void Repopulate( World& world );

		void Insert( const Object& object );
		void Insert( const Object& object, const sf::FloatRect& boundary );

		void Remove( const Object& object );
		void Remove( const Object& object, const sf::Vector2f& position );
		void Remove( const Object& object, const sf::Vector2i& chunkIdx, const unsigned cellId );
		void Remove( const Object& object, const sf::FloatRect& boundary );

		void GetNearby( const Object& object, const float distance, std::vector< Object >& out ) const;
		void GetNearby( const sf::Vector2f& position, const float distance, std::vector< Object >& out ) const;
		void GetNearby( const sf::FloatRect& boundary, std::vector< Object >& out ) const;

		template< typename Func >
		void ForEachNearby( const BaseObject& object, const float distance, Func f ) const;

		template< typename Func >
		void ForEachNearby( const sf::Vector2f& position, const float distance, Func f ) const;

		template< typename Func >
		void ForEachNearby( const sf::FloatRect& boundary, Func f ) const;

	protected:
		unsigned GetCellId( const Object& obj ) const;
		unsigned GetCellId( const sf::Vector2f& position ) const;

		sf::Vector2i CellHash( const Object& object ) const;
		sf::Vector2i CellHash( const sf::Vector2f& position ) const;

		sf::Vector2i ChunkHash( const Object& object ) const;
		sf::Vector2i ChunkHash( const sf::Vector2f& position ) const;

		sf::Vector2f GetObjectPosition( const BaseObject& object ) const;

		bool IsValid() const;
		bool IsValid( const BaseObject& obj ) const;

	private:
		unsigned m_cellSize = 0U;
		unsigned m_chunkSizeInCells = 0U;
		unsigned m_chunkSize = 0U;

		struct Chunk
		{
			std::vector< std::vector< BaseObject > > buckets;
			unsigned totalObjects = 0;
		};
		std::unordered_map< sf::Vector2i, Chunk > m_spacialChunks;
	};

	// Template function definitions
	template< typename Func >
	void TileMap::ForEachNearby( const BaseObject& object, const float distance, Func f ) const
	{
		ForEachNearby( GetObjectPosition( object ), distance, f );
	}

	template< typename Func >
	void TileMap::ForEachNearby( const sf::Vector2f& position, const float distance, Func f ) const
	{
		if( IsValid() )
		{
			const auto cell = CellHash( position );
			const auto chunkIdx = sf::Vector2i( cell.x / ( int )m_chunkSizeInCells, cell.y / (int )m_chunkSizeInCells );
			const auto& bucket = m_spacialChunks.at( chunkIdx ).buckets[cell.y * m_chunkSizeInCells + cell.x];

			for( const auto& object : bucket )
				if( Reflex::GetDistanceSq( position, Object( object ).GetTransform()->getPosition() ) <= distance * distance )
					f( Object( object ) );
		}
	}

	template< typename Func >
	void TileMap::ForEachNearby( const sf::FloatRect& boundary, Func f ) const
	{
		if( IsValid() )
		{
			const auto locTopLeft = CellHash( sf::Vector2f( boundary.left, boundary.top ) );
			const auto locBotRight = CellHash( sf::Vector2f( boundary.left + boundary.width, boundary.top + boundary.height ) );
			std::vector< unsigned > ids;

			for( int x = locTopLeft.x; x <= locBotRight.x; ++x )
			{
				for( int y = locTopLeft.y; y <= locBotRight.y; ++y )
				{
					const auto chunkIdx = sf::Vector2i( x / ( int )m_chunkSizeInCells, y / ( int )m_chunkSizeInCells );
					const auto& bucket = m_spacialChunks.at( chunkIdx ).buckets[y * m_chunkSizeInCells + x];

					for( const auto& obj : bucket )
						if( boundary.intersects( Object( obj ).GetTransform()->GetGlobalBounds() ) )
							f( Object( obj ) );
				}
			}
		}
	}
}