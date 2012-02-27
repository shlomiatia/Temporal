#include "Game.h"

#include <Temporal\Base\Base.h>
#include <Temporal\Graphics\Graphics.h>

namespace Temporal
{
	const float Game::FRAME_PERIOD_IN_MILLIS(1000.0f / /*FPS*/ 60.0f);
	const float Game::MAX_FRAMES_SKIP(5.0f);

	void Game::setPanel(Panel* panel)
	{
		_nextPanel = panel;
	}

	void Game::run(void)
	{
		_running = true;

		float lastFrameTick = (float)Thread::ticks();

		while (isRunning())
		{
			if (_nextPanel != NULL)
			{
				if(_panel != NULL)
				{
					_panel->dispose();
					delete _panel;
				}

				_nextPanel->init();
				_panel = _nextPanel;
				_nextPanel = NULL;
			}

			if (!isPaused())
			{
				if (_panel != NULL)
				{
					float currFrameTick = (float)Thread::ticks();
					if((currFrameTick - lastFrameTick) / FRAME_PERIOD_IN_MILLIS > MAX_FRAMES_SKIP)
						lastFrameTick = currFrameTick - MAX_FRAMES_SKIP * FRAME_PERIOD_IN_MILLIS;
					while(lastFrameTick + FRAME_PERIOD_IN_MILLIS <= currFrameTick )
					{
						_panel->update(FRAME_PERIOD_IN_MILLIS);
						lastFrameTick += FRAME_PERIOD_IN_MILLIS;
					}
					
					Graphics::get().prepareForDrawing();
					_panel->draw();
					Graphics::get().finishDrawing();
				}
			}
		}
		_panel->dispose();
	}

	Game::~Game(void)
	{
		stop();
		while (isRunning())
		{
			Thread::sleep(100);
		}
	}
}