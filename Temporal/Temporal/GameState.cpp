#include "GameState.h"
#include "Grid.h"
#include "Layer.h"
#include "NavigationGraph.h"
#include "EntitySystem.h"
#include "ResourceManager.h"
#include "Keyboard.h"
#include "Input.h"
#include "Game.h"
#include <stdio.h>

namespace Temporal
{
	GameState::GameState()
	{
		_grid = new Grid();
		_entitiesManager = new EntitiesManager();
		_navigationGraph = new NavigationGraph();
		_layersManager = new LayersManager();
	}

	GameState::~GameState()
	{
		delete _grid;
		delete _entitiesManager;
		delete _navigationGraph;
		delete _layersManager;
	}

	void GameState::init()
	{
		_grid->init(this);
		_entitiesManager->init(this);
		_navigationGraph->init(this);
		_layersManager->init(this);
	}

	void GameState::update(float framePeriod)
	{
		if(Keyboard::get().getKey(Key::ESC) || Input::get().getGamepad().getButton(GamepadButton::FRONT_RIGHT))
		{
			Game::get().stop();
		}
		
		_entitiesManager->sendMessageToAllEntities(Message(MessageID::UPDATE, &framePeriod));	
	}

	void GameState::draw() const
	{
		_layersManager->draw();
	}

	GameState* GameStateManager::getCurrentState() const
	{
		return _states.at(_currentStateId);
	}

	void GameStateManager::init(const char* gameStateFile)
	{
		GameStateLoader loader(gameStateFile);
		loader.execute();
		GameState* gameState = *loader.getResult().begin();
		Hash id = add(gameStateFile, gameState);
		_currentStateId = id;
	}

	void GameStateManager::dispose()
	{
		for(GameStateMapIterator i = _states.begin(); i != _states.end(); ++i)
			delete (i->second);
		if(_listener)
			delete _listener;
	}

	void GameStateManager::load(GameStateLoader* loader)
	{
		_loader = loader;
		IOThread::get().setJob(_loader);
	}

	void GameStateManager::unload(StringCollection files)
	{
		_files = files;
		_unload = true;
	}

	void GameStateManager::show(const char* gameStateFile)
	{
		Hash id = Hash(gameStateFile);
		_nextStateId = id;
	}

	void GameStateManager::update(float framePeriod)
	{
		if(_nextStateId != Hash::INVALID)
		{
			_currentStateId = _nextStateId;
			_nextStateId = Hash::INVALID;
		}
		if(_loader && _loader->isFinished())
		{
			load();
		}
		if(_unload)
		{
			unload();
		}
		if(_listener)
			_listener->onUpdate(framePeriod, *getCurrentState());
		getCurrentState()->update(framePeriod);
	}

	void GameStateManager::draw() const
	{
		getCurrentState()->draw();
	}

	void GameStateManager::unload()
	{
		for(StringIterator i = _files.begin(); i != _files.end(); ++i)
		{
			Hash id = Hash(i->c_str());
			GameStateMapIterator j = _states.find(id);
			delete j->second;
			_states.erase(j);
		}
		ResourceManager::get().collectGarbage();
		_unload = false;
	}

	void GameStateManager::load()
	{
		GameStateIterator j = _loader->getResult().begin();
		for(StringIterator i = _loader->getFiles().begin(); i != _loader->getFiles().end(); ++i)
		{
			add(i->c_str(), *j);
			++j;
		}
		_loader = 0;
	}

	Hash GameStateManager::add(const char* file, GameState* state)
	{
		Hash id = Hash(file);
		_states[id] = state;
		if(_listener)
			_listener->onLoaded(id, *state);
		
		return id;
	}
}