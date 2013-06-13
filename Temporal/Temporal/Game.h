#ifndef GAME_H
#define GAME_H

namespace Temporal
{
	class GameState;

	class Game
	{
	public:
		static Game& get()
		{
			static Game instance;
			return (instance);
		}

		void run(const char* gameState);
		void stop() { _isRunning = false; }

	private:
		static const float FRAME_PERIOD;

		bool _isRunning;
		float _lastFrameTime;

		void init(const char* gameState);
		void dispose();
		void update();
		void draw() const;

		Game() : _isRunning(false), _lastFrameTime(0.0f) {}
		Game(const Game&);
		Game& operator=(const Game&);
	};
}
#endif