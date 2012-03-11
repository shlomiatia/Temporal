#pragma once

#include "Panel.h"
#include <stdio.h>

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
		static const float FRAME_PERIOD_IN_MILLIS;
		static const float MAX_FRAMES_SKIP;

		bool _running;
		bool _paused;
		Panel* _panel;
		Panel* _nextPanel;

		void handlePanelsSwitch(void);
		void update(float& lastFrameMillis);
		void draw(void) const;

		Game(void) : _running(false), _paused(false), _panel(NULL), _nextPanel(NULL) {}
		~Game(void);
		Game(const Game&);
		Game& operator=(const Game&);
	};
}