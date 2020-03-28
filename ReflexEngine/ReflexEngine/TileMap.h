#pragma once

#include "Precompiled.h"

#include "TransformComponent.h"

namespace Reflex::Core
{
	class TileMap : sf::NonCopyable
	{
		friend class Reflex::Components::Transform;

	public:
		explicit TileMap( const sf::FloatRect& worldBounds );
		explicit TileMap( const sf::FloatRect& worldBounds, const unsigned spacialHashMapSize );

		void Insert( const Object& obj );
		void Insert( const Object& obj,  const sf::FloatRect& boundary );
		void Remove( const Object& obj );

		void GetNearby( const Object& obj, std::vector< Object >& out ) const;
		void GetNearby( const sf::Vector2f& position, std::vector< Object >& out ) const;
		void GetNearby( const Object& obj, const sf::FloatRect& boundary, std::vector< Object >& out ) const;

		template< typename Func >
		void ForEachNearby( const Object& obj, Func f ) const;

		template< typename Func >
		void ForEachNearby( const sf::Vector2f& position, Func f ) const;

		template< typename Func >
		void ForEachNearby( const Object& obj, const sf::FloatRect& boundary, Func f ) const;

		void Reset( const bool shouldRePopulate = false );
		void Reset( const unsigned spacialHashMapSize, const bool shouldRePopulate = false );

	protected:
		void RemoveByID( const Object& obj, const unsigned id );
		unsigned GetID( const Object& obj ) const;
		unsigned GetID( const sf::Vector2f& position ) const;
		std::vector< unsigned > GetID( const sf::FloatRect& boundary ) const;
		sf::Vector2i Hash( const sf::Vector2f& position ) const;

	private:
		const sf::FloatRect m_worldBounds;
		unsigned m_spacialHashMapSize = 0U;

		// Spacial hash map data
		unsigned m_spacialHashMapWidth = 0U;
		unsigned m_spacialHashMapHeight = 0U;
		std::vector< std::vector< Object > > m_spacialHashMap;
	};

	// Template function definitions
	template< typename Func >
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
	}
}