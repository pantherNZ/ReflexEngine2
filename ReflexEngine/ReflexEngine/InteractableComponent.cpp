#include "Precompiled.h"
#include "InteractableComponent.h"
#include "SFMLObjectComponent.h"
#include "Object.h"

namespace Reflex::Components
{
	Interactable::Interactable( const Reflex::Object& owner, const Reflex::Object& collisionObjectOverride )
		: Component< Interactable >( owner )
		, m_replaceCollisionObject( collisionObjectOverride )
	{

	}

	bool Interactable::IsFocussed() const
	{
		return isFocussed;
	}

	bool Interactable::IsSelected() const
	{
		return isSelected;
	}

	void Interactable::Select()
	{
		if( !isSelected && isEnabled )
		{
			isSelected = true;

			if( selectionChangedCallback )
				selectionChangedCallback( *this, true );
		}
	}

	void Interactable::Deselect()
	{
		if( isSelected )
		{
			isSelected = false;

			if( selectionChangedCallback )
				selectionChangedCallback( *this, false );
		}
	}
}