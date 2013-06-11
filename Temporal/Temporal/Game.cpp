#include "Game.h"
#include "Thread.h"
#include "Timer.h"
#include "Graphics.h"

namespace Temporal
{
	const float Game::FRAME_PERIOD(1.0f / /*FPS*/ 60.0f);

	void Game::run()
	{
		_running = true;
		_level->init();
		_level->update(FRAME_PERIOD);
		_lastFrameTime = Time::now();
		while (isRunning())
		{
			if (!isPaused())
				update();
			draw();
			float currentFrameTime = Time::now();
			float framesDifference = currentFrameTime - _lastFrameTime;
			float sleepTime = FRAME_PERIOD - framesDifference;
			if(sleepTime >= 0)
				Thread::sleep(sleepTime);

			_lastFrameTime += FRAME_PERIOD;
			if(_lastFrameTime < currentFrameTime)
				_lastFrameTime = currentFrameTime;
		}
		_level->dispose();
		delete _level;
	}

	void Game::update()
	{
		_level->update(FRAME_PERIOD);
	}

	void Game::draw() const
	{
		Graphics::get().prepareForDrawing();
		_level->draw();
		Graphics::get().finishDrawing();
	}
}