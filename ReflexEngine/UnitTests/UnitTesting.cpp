#pragma once

#include "../ReflexEngine/Include.h"
#include "UnitTesting.h"

using namespace Reflex;

class TestState;

int main()
{
	Core::Engine engine( false );
	engine.RegisterState< TestState >( true );
	engine.Run();

	//const auto test = CreateObject( "Data/Objects/Test.ro" );
	//auto test = CreateObject( sf::Vector2f( 0.0f, 0.0 ), 1.0f );
	//test.AddComponent< Reflex::Components::Camera >();
	//CreateROFile( "test", test );
	return 0;
}

class TestState : public Core::State, public UnitTesting
{
public:
	TestState( Core::StateManager& stateManager, Core::Context context )
		: State( stateManager, context )
	{
		RegisterSection( "Reflex Utility functions" );
		RegisterTest( std::bind( Reflex::IsDefault< sf::Vector2f >, sf::Vector2f() ) );
		//RegisterTest( std::bind( Reflex::IsDefault, sf::Color() ) );
		//RegisterTest( std::bind( Reflex::IsDefault, sf::Vector2f( 1.0f, 0.0f ) ), false );
		//RegisterTest( std::bind( Reflex::IsDefault, sf::Color( 1, 1, 1, 255 ) ), false );

		Run();
	}
};
