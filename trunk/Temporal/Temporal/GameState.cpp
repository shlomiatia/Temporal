#include "GameState.h"
#include "Grid.h"
#include "Layer.h"
#include "NavigationGraph.h"
#include "EntitySystem.h"
#include "ResourceManager.h"
#include "Keyboard.h"
#include "Input.h"
#include "Game.h"

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
		if(Keyboard::get().isPressing(Key::ESC) || Input::get().getGamepad().getButton(GamepadButton::FRONT_RIGHT))
		{
			Game::get().stop();
		}
		
		_entitiesManager->sendMessageToAllEntities(Message(MessageID::UPDATE, &framePeriod));	
	}

	void GameState::draw() const
	{
		_layersManager->draw();
	}

	GameState& GameStateManager::getCurrentState() const
	{
		return *_states.at(_currentStateId);
	}

	void GameStateManager::init(const char* gameStateFile)
	{
		GameStateLoader loader(gameStateFile);
		loader.execute();
		GameStateIterator i = loader.getResult().begin();
		_states[i->first] = i->second;		
		_currentStateId = i->first;
	}

	void GameStateManager::dispose()
	{
		for(GameStateIterator i = _states.begin(); i != _states.end(); ++i)
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
			_listener->onUpdate(framePeriod, getCurrentState());
		getCurrentState().update(framePeriod);
	}

	void GameStateManager::draw() const
	{
		getCurrentState().draw();
	}

	void GameStateManager::unload()
	{
		for(StringIterator i = _files.begin(); i != _files.end(); ++i)
		{
			Hash id = Hash(i->c_str());
			GameStateIterator j = _states.find(id);
			delete j->second;
			_states.erase(j);
		}
		ResourceManager::get().collectGarbage();
		_unload = false;
	}

	void GameStateManager::load()
	{
		for(GameStateIterator i = _loader->getResult().begin(); i != _loader->getResult().end(); ++i)
		{
			_states[i->first] = i->second;
		}
		_loader = 0;
	}
}