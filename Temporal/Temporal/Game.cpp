#include "Game.h"
#include "Thread.h"
#include "Timer.h"
#include "Input.h"
#include "Graphics.h"
#include "ResourceManager.h"
#include "GameState.h"

namespace Temporal
{
	const float Game::FRAME_PERIOD(1.0f / /*FPS*/ 60.0f);

	void Game::init()
	{
		Graphics::get().init();
		Input::get().init();
		ResourceManager::get().init();
		GameStateManager::get().init();
	}

	void Game::dispose()
	{
		GameStateManager::get().dispose();
		ResourceManager::get().dispose();
		Input::get().dispose();
		Graphics::get().dispose();
	}

	void Game::run(const char* gameState)
	{
		init();

		_isRunning = true;
		GameStateManager::get().pushState(gameState);

		// Wait for initial state to load

		_lastFrameTime = Time::now();
		while (_isRunning)
		{
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

		dispose();
	}

	void Game::update()
	{
		Input::get().update();
		GameStateManager::get().update(FRAME_PERIOD);
	}

	void Game::draw() const
	{
		Graphics::get().prepareForDrawing();
		GameStateManager::get().draw();
		Graphics::get().finishDrawing();
	}
}