#pragma once

#pragma warning( error : 4390 ) // ';': empty controlled statement found; is this the intent?
#pragma warning( error : 4715 ) // not all control paths return a value

#include "World.h"
#include "Component.h"
#include "Utility.h"
#include "Engine.h"

#include "GridComponent.h"
#include "SFMLObjectComponent.h"

#include "RenderSystem.h"
#include "InteractableSystem.h"
#include "MovementSystem.h"
#include "CameraSystem.h"
#include "SteeringSystem.h"

#include "EventManager.h"
