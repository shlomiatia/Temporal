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
		// indicate we're now running the Game
		_running = true;

		// define frame-time calculation variables
		unsigned int before = Thread::ticks();
		unsigned int after;
		int sleep;

		while (isRunning())
		{
			// switch panel if nextPanel exists
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
					// update & draw current panel
					_panel->update();
					Graphics::get().prepareForDrawing();
					_panel->draw();
					Graphics::get().finishDrawing();
				}

				// calculate required sleep time according to time passed
				after = Thread::ticks();
				sleep = FRAME_DELAY - (after - before);

				// if has time to sleep - sleep and update calculated "after" time
				if (sleep > 0)
				{
					Thread::sleep(sleep);
					after += sleep;
				}

				// update variables for next frame
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