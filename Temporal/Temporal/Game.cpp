#include "Game.h"
#include "Thread.h"
#include "Timer.h"
#include "Input.h"
#include "Graphics.h"
#include "ResourceManager.h"
#include "GameState.h"
#include "Settings.h"
#include "Font.h"
#include "Log.h"
#include "GameStateEditor.h"

namespace Temporal
{
	const float Game::FRAME_PERIOD(1.0f / /*FPS*/ 60.0f);

	void Game::init(const char* gameState)
	{
		SettingsLoader loader("resources/settings.xml");
		loader.execute();
		Settings* settings = loader.getResult();
		
		Log::init();
		Graphics::get().init(*settings);
		Input::get().init();
		FontManager::get().init();
		IOThread::get().init();
		ResourceManager::get().init();
		GameStateManager::get().init(gameState);
		delete settings;

		if (GameStateManager::get().getCurrentState().getEntitiesManager().getEntity(ComponentsIds::GAME_STATE_EDITOR))
			return;
		GameStateEditor::loadEditor();
	}

	void Game::dispose()
	{
		GameStateManager::get().dispose();
		ResourceManager::get().dispose();
		IOThread::get().dispose();
		FontManager::get().dispose();
		Input::get().dispose();
		Graphics::get().dispose();
		Log::dispose();
	}

	void Game::run(const char* gameState)
	{
		init(gameState);

		_isRunning = true;

		// Wait for initial state to load

		_lastFrameTime = Time::now();
		while (_isRunning)
		{
			float framePeriod = update();
			draw(framePeriod);
		}

		dispose();
	}

	PerformanceTimer& updateTimer = PerformanceTimerManager::get().getTimer(Hash("TMR_UPDATE"));
	float Game::update()
	{
		updateTimer.measure();
		Input::get().update();
		float currentFrameTime = Time::now();
		float framePeriod = currentFrameTime - _lastFrameTime;
		if (framePeriod > 1.0f / 10.0f)
			framePeriod = 1.0f / 60.0f;
		GameStateManager::get().update(framePeriod);
		_lastFrameTime = currentFrameTime;
		updateTimer.print("UPDATE");
		return framePeriod;
	}

	PerformanceTimer& drawTimer = PerformanceTimerManager::get().getTimer(Hash("TMR_DRAW"));
	PerformanceTimer& finishDrawingTimer = PerformanceTimerManager::get().getTimer(Hash("TMR_FINISH_DRAWING"));
	void Game::draw(float framePeriod)
	{
		Graphics::get().prepareForDrawing();
		drawTimer.measure();
		GameStateManager::get().draw(framePeriod);
		drawTimer.print("DRAW");
		finishDrawingTimer.measure();
		Graphics::get().finishDrawing();
		finishDrawingTimer.print("FINISH DRAWING");
	}
}