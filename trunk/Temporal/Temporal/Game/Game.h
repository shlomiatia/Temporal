#pragma once
#include <Temporal\Base\Base.h>
#include "Panel.h"

namespace Temporal
{
	class Game
	{
	public:
		static Game& get(void)
		{
			static Game instance;
			return (instance);
		}

		void setPanel(Panel* panel);

		bool isRunning(void) const { return (_running); }
		bool isPaused(void) const { return (_paused); }

		void run(void);
		void stop(void) { _running = false;	}

	private:
		static const int FRAME_DELAY = 1000 / /*FPS*/ 60;

		bool _running;
		bool _paused;
		Panel* _panel;
		Panel* _nextPanel;

		Game(void) : _running(false), _paused(false), _panel(NULL), _nextPanel(NULL) {}
		~Game(void);
		Game(const Game&);
		Game& operator=(const Game&);
	};
}