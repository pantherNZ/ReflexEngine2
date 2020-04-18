#pragma once

// Includes
#include "Precompiled.h"
#include "ResourceManager.h"
#include "StateManager.h"
#include "Object.h"

#define DISABLE_TILEMAP

namespace Reflex::Core
{
	// Engine class
	class Engine : private sf::NonCopyable
	{
	public:
		// Construct an engine instance
		explicit Engine( const std::string& windowName = "Reflex Engine", const bool fullscreen = false, const int fixedUpdatesPerSecond = 30, const bool enableProfiling = false );
		explicit Engine( const int screenWidth, const int screenHeight, const std::string& windowName = "Reflex Engine", const int fixedUpdatesPerSecond = 30, const bool enableProfiling = false );

		// This constructor is intended for creating an engine without a window / UI (by passing false)
		explicit Engine( const bool createWindow, const int fixedUpdatesPerSecond = 30, const bool enableProfiling = false );

		~Engine();

		void Run();
		void Exit();

		template< typename T >
		void RegisterState( const bool isStartingState = false );

		sf::RenderWindow& GetWindow() { return m_window; }

	protected:
		void Setup();
		void KeyboardInput( const sf::Keyboard::Key key, const bool isPressed );
		void ProcessEvents();
		void Update( const float deltaTime );
		void Render();

		void UpdateStatistics( const int deltaTimeUS, const int frameTimeUS );

	protected:
		// Core window
		sf::RenderWindow m_window;

		// Resource managers
		TextureManager m_textureManager;
		FontManager m_fontManager;

		// State manager (handles different scenes & transitions, contains worlds which hold objects etc.)
		StateManager m_stateManager;

		// Stats
		int m_fpsLimit = 60;
		int m_fixedUpdatesPerSecond;
		sf::String m_statisticsText;
		sf::Time m_statisticsUpdateTime;
		sf::Clock m_totalTime;

		struct Frame
		{
			int deltaTimeUS = 0;
			int frameTimeUS = 0;
		};

		int totalDeltaUS = 0;
		int totalTimeUS = 0;
		enum { NumSamples = 20 };
		Frame frames[NumSamples];
		int idx = -1;

		// ImGui
		bool m_profilingEnabled = false;
		bool m_cmdMode = false;
		bool m_showMetrics = false;
		bool m_showStyleEditor = false;
	};

	template< typename T >
	void Engine::RegisterState( const bool isStartingState )
	{
		m_stateManager.RegisterState< T >();

		if( isStartingState )
			m_stateManager.PushState< T >();
	}
}