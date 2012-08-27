#ifndef GAME_H
#define GAME_H

#include <stdio.h>

namespace Temporal
{
	class Level
	{
	public:
		virtual void init() = 0;
		virtual void update(float framePeriodInMillis) = 0;
		virtual void draw() const = 0;
		virtual void dispose() = 0;

		Level() {};
		virtual ~Level() {}
	private:
		Level(const Level&);
		Level& operator=(const Level&);
	};

	class Game
	{
	public:
		static Game& get()
		{
			static Game instance;
			return (instance);
		}

		void setLevel(Level* level) { _level = level; }
		bool isRunning() const { return _running; }
		bool isPaused() const { return _paused; }
		void run();
		void stop() { _running = false; }

	private:
		static const float FRAME_PERIOD_IN_MILLIS;

		Level* _level;
		bool _running;
		bool _paused;
		float _lastFrameMillis;

		void update();
		void draw() const;

		Game() : _level(NULL), _running(false), _paused(false), _lastFrameMillis(0.0f) {}
		Game(const Game&);
		Game& operator=(const Game&);
	};
}
#endif