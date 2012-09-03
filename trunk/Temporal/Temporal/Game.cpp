#include "Game.h"
#include "Thread.h"
#include "Graphics.h"

namespace Temporal
{
	const float Game::FRAME_PERIOD_IN_MILLIS(1000.0f / /*FPS*/ 15.0f);

	void Game::run()
	{
		_running = true;
		_level->init();
		_level->update(FRAME_PERIOD_IN_MILLIS);
		_lastFrameMillis = static_cast<float>(Thread::getElapsedTimeInMillis());
		while (isRunning())
		{
			if (!isPaused())
				update();
			draw();
			float currentFrameMillis = static_cast<float>(Thread::getElapsedTimeInMillis());
			float framesDifference = currentFrameMillis - _lastFrameMillis;
			float sleepTime = FRAME_PERIOD_IN_MILLIS - framesDifference;
			if(sleepTime >= 0)
				Thread::sleep(sleepTime);

			_lastFrameMillis += FRAME_PERIOD_IN_MILLIS;
			if(_lastFrameMillis < currentFrameMillis)
				_lastFrameMillis = currentFrameMillis;
		}
		_level->dispose();
		delete _level;
	}

	void Game::update()
	{
		_level->update(FRAME_PERIOD_IN_MILLIS);
		
	}

	void Game::draw() const
	{
		Graphics::get().prepareForDrawing();
		_level->draw();
		Graphics::get().finishDrawing();
	}
}