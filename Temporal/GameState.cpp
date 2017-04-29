#include "GameState.h"
#include "Grid.h"
#include "Layer.h"
#include "NavigationGraph.h"
#include "EntitySystem.h"
#include "ResourceManager.h"
#include "Keyboard.h"
#include "Input.h"
#include "Game.h"
#include "SaverLoader.h"

namespace Temporal
{
	GameState::GameState()
		: _grid(new Grid()), _layersManager(new LayersManager()), _entityTemplatesManager(new EntityTemplatesManager()), _entitiesManager(new EntitiesManager()), _navigationGraph(new NavigationGraph())
	{
	}

	GameState::~GameState()
	{
		delete _navigationGraph;
		delete _entitiesManager;
		delete _entityTemplatesManager;
		delete _layersManager;
		delete _grid;
	}

	void GameState::init()
	{
		_grid->init(this);
		_layersManager->init(this);
		_entityTemplatesManager->init(this);
		_entitiesManager->init(this);
		_navigationGraph->init(this);
	}

	void GameState::update(float framePeriod)
	{
		HashList* updateFilter = 0;
		if (_updateFilter.size() != 0)
			updateFilter = &_updateFilter;
		_entitiesManager->sendMessageToAllEntities(Message(MessageID::UPDATE, &framePeriod), updateFilter);	
	}

	void GameState::draw() const
	{
		_layersManager->draw();
	}

	GameState& GameStateManager::getCurrentState() const
	{
		return *_states.at(_currentStateId);
	}

	void BasicGameStateListener::onLoaded(Hash id, GameState& gameState)
	{
		if(id == Hash("resources/game-states/entities.xml"))
		{
			Entity* entity = new Entity(Hash(GameSaverLoader::TYPE));
			entity->add(new GameSaverLoader());
			gameState.getEntitiesManager().add(entity);
		}
	}

	void GameStateManager::init(const char* gameStateFile)
	{
		_currentStateId = load(gameStateFile);
	}

	void GameStateManager::dispose()
	{
		for(GameStateIterator i = _states.begin(); i != _states.end(); ++i)
			delete (i->second);
		if(_listener)
			delete _listener;
	}

	void GameStateManager::syncLoadAndShow(const char* gameStateFile)
	{
		//IOThread::get().setJob(_loader);
		_nextStateId = load(gameStateFile);
	}

	void GameStateManager::syncUnloadCurrent()
	{
		_unload = true;
	}

	Hash GameStateManager::load(const char* gameStateFile)
	{
		GameStateLoader loader(gameStateFile);
		loader.execute();
		Hash id = loader.getId();
		_states[id] = loader.getResult();
		return id;
	}

	void GameStateManager::update(float framePeriod)
	{
		if (_unload)
		{
			GameStateIterator j = _states.find(_currentStateId);
			delete j->second;
			_states.erase(j);
			ResourceManager::get().collectGarbage();
			_unload = false;
			_nextStateId = _states.begin()->first;
		}
		if(_nextStateId != Hash::INVALID)
		{
			_currentStateId = _nextStateId;
			_nextStateId = Hash::INVALID;
		}
		
		getCurrentState().update(framePeriod);
	}

	void GameStateManager::draw() const
	{
		getCurrentState().draw();
	}
}