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

		void Insert( const Object& obj );
		void Insert( const Object& obj, const sf::FloatRect& boundary );
		void Remove( const Object& obj );
		void Remove( const Object& obj, const sf::FloatRect& boundary );

		//void GetNearby( const Object& obj, std::vector< Object >& out ) const;
		//void GetNearby( const sf::Vector2f& position, std::vector< Object >& out ) const;
		//void GetNearby( const Object& obj, const sf::FloatRect& boundary, std::vector< Object >& out ) const;

		//template< typename Func >
		//void ForEachNearby( const BaseObject& obj, const float distance, Func f ) const;
		//
		//template< typename Func >
		//void ForEachNearby( const sf::Vector2f& position, const float distance, Func f ) const;
		//
		//template< typename Func >
		//void ForEachNearby( const BaseObject& obj, const sf::FloatRect& boundary, Func f ) const;

	protected:
		unsigned GetCellId( const Object& obj ) const;
		unsigned GetCellId( const sf::Vector2f& position ) const;

		sf::Vector2i CellHash( const sf::Vector2f& position ) const;
		sf::Vector2i ChunkHash( const sf::Vector2f& position ) const;

		bool IsValid() const;

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
	/*template< typename Func >
	void TileMap::ForEachNearby( const Object& obj, Func f ) const
	{
		if( obj && m_spacialHashMapSize )
		{
			const auto id = GetID( obj );

			if( id == -1 )
				return;

			auto& bucket = m_spacialHashMap[id];

			for( auto& item : bucket )
				if( item != obj )
					f( item );
		}
	}

	template< typename Func >
	void TileMap::ForEachNearby( const sf::Vector2f& position, Func f ) const
	{
		if( m_spacialHashMapSize )
		{
			const auto id = GetID( position );

			if( id == -1 )
				return;

			auto& bucket = m_spacialHashMap[id];

			for( auto& item : bucket )
				f( item );
		}
	}

	template< typename Func >
	void TileMap::ForEachNearby( const Object& obj, const sf::FloatRect& boundary, Func f ) const
	{
		if( obj && m_spacialHashMapSize )
		{
			const auto ids = GetID( boundary );

			for( auto& id : ids )
			{
				if( id == -1 )
					continue;

				auto& bucket = m_spacialHashMap[id];

				for( auto& item : bucket )
					if( item != obj )
						f( item );
			}
		}
	}*/
}