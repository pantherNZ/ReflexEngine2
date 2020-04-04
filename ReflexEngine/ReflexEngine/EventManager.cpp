#include "Precompiled.h"
#include "EventManager.h"
#include "World.h"

namespace Reflex::Core
{
	size_t BaseEvent::typeCounter = 0;

	EventReceiver::~EventReceiver()
	{
		eventManager->Unsubscribe( *this );
	}

	EventTriggerer::~EventTriggerer()
	{
		eventManager->RemoveTriggerer( *this );
	}
}