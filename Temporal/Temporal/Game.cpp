#include "Game.h"
#include "Panel.h"
#include "Thread.h"
#include "Graphics.h"

namespace Temporal
{
	const float Game::FRAME_PERIOD_IN_MILLIS(1000.0f / /*FPS*/ 60.0f);
	const float Game::MAX_FRAMES_SKIP(1.0f);

	void Game::setPanel(Panel* panel)
	{
		_nextPanel = panel;
	}

	void Game::run(void)
	{
		_running = true;

		_lastFrameMillis = (float)Thread::getElapsedTimeInMillis();

		while (isRunning())
		{
			handlePanelsSwitch();

			if (!isPaused())
			{
				if (_panel != NULL)
				{
					update();
					draw();

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

	void Game::handlePanelsSwitch(void)
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
	}

	void Game::update(void)
	{
		float currFrameMillis = (float)Thread::getElapsedTimeInMillis();
		float framesToSkip = (currFrameMillis - _lastFrameMillis) / FRAME_PERIOD_IN_MILLIS - 1.0f;
		if(framesToSkip > MAX_FRAMES_SKIP)
			_lastFrameMillis = currFrameMillis - MAX_FRAMES_SKIP * FRAME_PERIOD_IN_MILLIS;
		while(_lastFrameMillis + FRAME_PERIOD_IN_MILLIS <= currFrameMillis )
		{
			_panel->update(FRAME_PERIOD_IN_MILLIS);
			_lastFrameMillis += FRAME_PERIOD_IN_MILLIS;
		}
	}

	void Game::draw(void) const
	{
		Graphics::get().prepareForDrawing();
		_panel->draw();
		Graphics::get().finishDrawing();
	}
}