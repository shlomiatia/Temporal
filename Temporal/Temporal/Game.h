#ifndef GAME_H
#define GAME_H

#include <stdio.h>

namespace Temporal
{
	class Panel;

	class Game
	{
	public:
		static Game& get()
		{
			static Game instance;
			return (instance);
		}

		void setPanel(Panel* panel);

		bool isRunning() const { return (_running); }
		bool isPaused() const { return (_paused); }

		void run();

		void stop() { _running = false;	}

	private:
		static const float FRAME_PERIOD_IN_MILLIS;
		static const float MAX_FRAMES_SKIP;

		bool _running;
		bool _paused;
		Panel* _panel;
		Panel* _nextPanel;
		float _lastFrameMillis;

		void handlePanelsSwitch();
		void update();
		void draw() const;

		Game() : _running(false), _paused(false), _panel(NULL), _nextPanel(NULL), _lastFrameMillis(0.0f) {}
		~Game();
		Game(const Game&);
		Game& operator=(const Game&);
	};
}
#endif