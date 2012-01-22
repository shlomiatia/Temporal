#pragma once
#include "Panel.h"

namespace Temporal
{
	class Game
	{
	public:
		// gets the singleton Game instance
		static Game& get(void)
		{
			static Game instance;
			return (instance);
		}

		// set panel data for the game
		void setPanel(Panel* panel);

		// get running state, get/set paused state
		bool isRunning(void) const { return (_running); }
		bool isPaused(void) const { return (_paused); }

		void run(void);
		void stop(void) { _running = false;	}

	private:
		static const int FRAME_DELAY = 1000 / /*FPS*/ 30;

		bool _running;
		bool _paused;
		Panel* _panel;
		Panel* _nextPanel;

		Game(void) : _running(false), _paused(false) {}
		~Game(void);
		Game(const Game&);
		Game& operator=(const Game&);
	};
}