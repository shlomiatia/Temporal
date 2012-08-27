#include "Game.h"
#include "Thread.h"
#include "Graphics.h"

namespace Temporal
{
	const float Game::FRAME_PERIOD_IN_MILLIS(1000.0f / /*FPS*/ 60.0f);

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
		}
		_level->dispose();
		delete _level;
	}

	void Game::update()
	{
		float currFrameMillis = static_cast<float>(Thread::getElapsedTimeInMillis());
		float framesDifference = currFrameMillis - _lastFrameMillis;
		if(framesDifference > FRAME_PERIOD_IN_MILLIS)
			_lastFrameMillis = currFrameMillis - FRAME_PERIOD_IN_MILLIS;
		while(_lastFrameMillis < currFrameMillis )
		{
			_level->update(FRAME_PERIOD_IN_MILLIS);
			_lastFrameMillis += FRAME_PERIOD_IN_MILLIS;
		}
	}

	void Game::draw() const
	{
		Graphics::get().prepareForDrawing();
		_level->draw();
		Graphics::get().finishDrawing();
	}
}