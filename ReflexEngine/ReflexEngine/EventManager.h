#pragma once

#include <memory>
#include <vector>
#include <functional>

namespace Reflex::Core
{
	// Event object itself
	struct BaseEvent
	{
		virtual ~BaseEvent() {}
	protected:
		static size_t typeCounter;
	};

	template< typename EventType >
	struct Event : BaseEvent
	{
		static size_t GetType()
		{
			static size_t type = BaseEvent::typeCounter++;
			return type;
		}

		Event( const EventType& event ) : event( event ) {}
		const EventType& event;
	};

	class World;

	// Event receiver
	struct EventReceiver
	{
		friend class EventManager;

		EventReceiver() { }
		virtual ~EventReceiver();

	protected:
		EventReceiver( const EventReceiver& other )
			: receiverIndex( std::nullopt )
			, eventManager( other.eventManager )
		{
		}

	protected:
		std::optional< unsigned > receiverIndex;
		EventManager* eventManager = nullptr;
	};

	// Event triggerer
	struct EventTriggerer
	{
		EventTriggerer( EventManager& eventManager );
		EventTriggerer( World& world );
		virtual ~EventTriggerer();

		template< typename EventType, typename ReceiverType >
		void Subscribe( ReceiverType& receiver, void ( ReceiverType::* func )( const EventType& ) );

		template< typename ReceiverType >
		void Unsubscribe( ReceiverType& receiver );

	protected:
		friend class EventManager;

		EventTriggerer( const EventTriggerer& other )
			: triggererIndex( std::nullopt )
			, eventManager( other.eventManager )
		{
		}

		template< typename EventType >
		void Emit( const EventType& event );

	private:
		std::optional< unsigned > triggererIndex;
		EventManager& eventManager;
	};

	class EventManager
	{
	public:
		template < class EventType >
		using callType = std::function< void( const EventType& ) >;

		template< typename EventType, typename ReceiverType >
		void Subscribe( ReceiverType& receiver, void ( ReceiverType::*func )( const EventType& ) )
		{	
			SubscribeInternal( nullptr, receiver, func );
		}

		template< typename EventType, typename ReceiverType >
		void Subscribe( EventTriggerer& triggerer, ReceiverType& receiver, void ( ReceiverType::* func )( const EventType& ) )
		{
			SubscribeInternal( &triggerer, receiver, func );
		}

		void Unsubscribe( EventReceiver& receiver )
		{
			for( auto& eventType : m_subscribers )
			{
				Reflex::EraseIf( eventType, [&receiver]( const ReceiverInstance& instance )
				{
					return instance.receiverIndex == receiver.receiverIndex;
				} );
			}
		}

		void Unsubscribe( EventTriggerer& triggerer, EventReceiver& receiver )
		{
			for( auto& eventType : m_subscribers )
			{
				Reflex::EraseIf( eventType, [&]( const ReceiverInstance& instance )
				{
					return instance.receiverIndex == receiver.receiverIndex && instance.triggererIndex == triggerer.triggererIndex;
				} );
			}
		}

		void RemoveTriggerer( EventTriggerer& triggerer )
		{
			for( auto& eventType : m_subscribers )
			{
				Reflex::EraseIf( eventType, [&triggerer]( const ReceiverInstance& instance )
				{
					return instance.triggererIndex && instance.triggererIndex == triggerer.triggererIndex;
				} );
			}
		}

		template< typename EventType >
		void Emit( EventTriggerer& triggerer, const EventType& event )
		{
			if( !triggerer.triggererIndex )
				triggerer.triggererIndex = nextTriggererIndex++;

			size_t type = Event< EventType >::GetType();
			if( type >= m_subscribers.size() )
				m_subscribers.resize( type + 1 );

			Event< EventType > eventWrapper( event );
			for( auto& receiver : m_subscribers[type] )
				if( !receiver.triggererIndex || receiver.triggererIndex == triggerer.triggererIndex )
					receiver.functor( eventWrapper );
		}

		template< typename EventType >
		struct CallbackWrapper
		{
			CallbackWrapper( callType< EventType > callable ) : m_callable( callable ) {}

			void operator() ( const BaseEvent& event ) 
			{
				m_callable( static_cast< const Event< EventType >& >( event ).event );
			}

			callType< EventType > m_callable;
		};

	private:
		template< typename EventType, typename ReceiverType >
		void SubscribeInternal( EventTriggerer* triggerer, ReceiverType& receiver, void ( ReceiverType::* func )( const EventType& ) )
		{
			if( triggerer && !triggerer->triggererIndex )
				triggerer->triggererIndex = nextTriggererIndex++;

			const auto callable = callType< EventType >( std::bind( func, &receiver, std::placeholders::_1 ) );
			
			if( !receiver.receiverIndex )
			{
				receiver.receiverIndex = nextReceiverIndex++;
				receiver.eventManager = this;
			}

			size_t type = Event<EventType>::GetType();
			if( type >= m_subscribers.size() )
				m_subscribers.resize( type + 1 );

			m_subscribers[type].push_back( { CallbackWrapper< EventType >( callable ), triggerer ? triggerer->triggererIndex : std::nullopt, *receiver.receiverIndex } );
		}

		struct ReceiverInstance
		{
			callType< BaseEvent > functor;
			std::optional< unsigned > triggererIndex;
			unsigned receiverIndex;
		};

		unsigned nextTriggererIndex = 0;
		unsigned nextReceiverIndex = 0;
		std::vector< std::vector< ReceiverInstance > > m_subscribers;
	};

	template< typename EventType, typename ReceiverType >
	void EventTriggerer::Subscribe( ReceiverType& receiver, void ( ReceiverType::* func )( const EventType& ) )
	{
		eventManager.Subscribe( *this, receiver, func );
	}

	template< typename ReceiverType >
	void EventTriggerer::Unsubscribe( ReceiverType& receiver )
	{
		eventManager.Unsubscribe( *this, receiver );
	}

	template< typename EventType >
	void EventTriggerer::Emit( const EventType& event )
	{
		eventManager.Emit( *this, event );
	}
}