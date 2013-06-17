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

	bool b = false;

	void GameState::update(float framePeriod)
	{
		if(Keyboard::get().getKey(Key::ESC) || Input::get().getGamepad().getButton(GamepadButton::FRONT_RIGHT))
		{
			Game::get().stop();
		}
		if(Keyboard::get().getKey(Key::Q) && !b)
		{
			//const AABB& bounds = *static_cast<AABB*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::GET_SHAPE)));
			//getEntity().getManager().sendMessageToEntity(Hash("ENT_CHASER"), Message(MessageID::SET_NAVIGATION_DESTINATION, const_cast<AABB*>(&bounds)));
			//getEntitiesManager().sendMessageToAllEntities(Message(MessageID::MERGE_TO_TEMPORAL_ECHOES));
			StringCollection files;
			files.push_back(std::string("resources/game-states/entities.xml"));
			GameStateManager::get().load(files);
			b = true;
		}	
		_entitiesManager->sendMessageToAllEntities(Message(MessageID::UPDATE, &framePeriod));	
	}

	void GameState::draw() const
	{
		_layersManager->draw();
	}

	void GameState::loaded()
	{
		StringCollection files;
		files.push_back(std::string("resources/game-states/loading.xml"));
		GameStateManager::get().unload(files);
		GameStateManager::get().show("resources/game-states/entities.xml");
	}

	GameState* GameStateManager::getCurrentState() const
	{
		return _states.at(_currentStateId);
	}

	void GameStateManager::init(const char* gameStateFile)
	{
		GameState* gameState = IOAPI::loadGameState(gameStateFile);
		Hash id = Hash(gameStateFile);
		_states[id] = gameState;
		_currentStateId = id;
	}

	void GameStateManager::dispose()
	{
		for(GameStateIterator i = _states.begin(); i != _states.end(); ++i)
			delete (i->second);
	}

	void GameStateManager::load(StringCollection files)
	{
		_files = files;
		IOThread::get().setJob(this);
	}

	void* GameStateManager::load()
	{
		for(StringIterator i = _files.begin(); i != _files.end(); ++i)
		{
			Hash id = Hash(i->c_str());
			if(_states.find(id) != _states.end())
				continue;
			GameState* gameState = IOAPI::loadGameState(i->c_str());
			_states[id] = gameState;
		}
		return 0;
	}

	void GameStateManager::loaded(void* param)
	{
		getCurrentState()->loaded();
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
		if(_unload)
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
		getCurrentState()->update(framePeriod);
	}

	void GameStateManager::draw() const
	{
		getCurrentState()->draw();
	}
}