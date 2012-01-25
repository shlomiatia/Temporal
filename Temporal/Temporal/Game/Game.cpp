#include "Game.h"

#include <Temporal\Base\Base.h>
#include <Temporal\Graphics\Graphics.h>

namespace Temporal
{
	void Game::setPanel(Panel* panel)
	{
		_nextPanel = panel;
	}

	void Game::run(void)
	{
		_running = true;

		unsigned int before = Thread::ticks();
		unsigned int after;
		int sleep;

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
					_panel->update();
					Graphics::get().prepareForDrawing();
					_panel->draw();
					Graphics::get().finishDrawing();
				}

				after = Thread::ticks();
				sleep = FRAME_DELAY - (after - before);

				if (sleep > 0)
				{
					Thread::sleep(sleep);
					after += sleep;
				}

				before = after;
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