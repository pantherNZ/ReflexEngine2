#pragma once

#pragma warning( error : 4390 ) // ';': empty controlled statement found; is this the intent?
#pragma warning( error : 4715 ) // not all control paths return a value

#include "Engine.h"
#include "World.h"
#include "Component.h"
#include "Object.h"
#include "System.h"
#include "Utility.h"

#include "TransformComponent.h"
#include "CameraComponent.h"
#include "InteractableComponent.h"
#include "SFMLObjectComponent.h"
#include "GridComponent.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"

#include "RenderSystem.h"
#include "InteractableSystem.h"
#include "MovementSystem.h"
#include "CameraSystem.h"
#include "SteeringSystem.h"
#include "PhysicsSystem.h"

#include "EventManager.h"