#include "Precompiled.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "World.h"

namespace Reflex::Components
{
	Camera::Camera( const Reflex::Object& owner )
		: Component< Camera >( owner )
	{

	}

	Camera::Camera( const Reflex::Object& owner, const sf::Vector2f& centre, const sf::Vector2f& size, const bool activate )
		: Component< Camera >( owner )
		, sf::View( centre, size )
	{
		flags.set( StartActivated, activate );
		flags.set( AdditivePanning );
		flags.set( NormaliseDiagonalPanning );
	}

	Camera::Camera( const Reflex::Object& owner, const sf::FloatRect& viewRect )
		: Component< Camera >( owner )
		, sf::View( viewRect )
	{
	}

	Camera::~Camera()
	{
	}

	void Camera::OnConstructionComplete()
	{
		if( flags.test( StartActivated ) || !GetObject().GetWorld().GetActiveCamera().IsValid() )
			SetActiveCamera();

		GetObject().GetTransform()->setPosition( getCenter() );
	}

	void Camera::FollowObject( const Reflex::Object& object, const float interpolationSpeed )
	{
		followTarget = object;
		followInterpSpeed = interpolationSpeed;
	}

	bool Camera::IsActiveCamera() const
	{
		return GetObject().GetWorld().GetActiveCamera().object == GetObject();
	}

	void Camera::SetActiveCamera()
	{
		GetObject().GetWorld().SetActiveCamera( GetHandle() );
	}

	void Camera::EnableMousePanning( const sf::Vector2f& margin, const sf::Vector2f& speed )
	{
		flags.set( MousePanning );
		panMouseMargin = margin;

		if( speed.x != 0.0f && speed.y != 0.0f )
			panSpeed = speed;
	}

	void Camera::EnableMouseZooming( const float scaleFactor, const bool centreOnMouse )
	{
		flags.set( MouseZooming );
		flags.set( ZoomCentreOnMouse, centreOnMouse );
		
		if( scaleFactor != 0.0f )
			zoomScaleFactor = scaleFactor;
	}

	void Camera::EnableWASDPanning( const sf::Vector2f& speed )
	{
		flags.set( WASDPanning );

		if( speed.x != 0.0f && speed.y != 0.0f )
			panSpeed = speed;
	}

	void Camera::EnableArrowKeyPanning( const sf::Vector2f& speed )
	{
		flags.set( ArrowPanning );
		
		if( speed.x != 0.0f && speed.y != 0.0f )
			panSpeed = speed;
	}
}
