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
		if(Keyboard::get().getKey(Key::ESC) || Input::get().getGamepad().getButton(GamepadButton::FRONT_RIGHT))
		{
			Game::get().stop();
		}
		if(Keyboard::get().getKey(Key::Q))
		{
			//GameStateManager::get().changeState("resources/game-states/entities.xml");
			//const AABB& bounds = *static_cast<AABB*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::GET_SHAPE)));
			//getEntity().getManager().sendMessageToEntity(Hash("ENT_CHASER"), Message(MessageID::SET_NAVIGATION_DESTINATION, const_cast<AABB*>(&bounds)));
			getEntitiesManager().sendMessageToAllEntities(Message(MessageID::MERGE_TO_TEMPORAL_ECHOES));
		}	
		_entitiesManager->sendMessageToAllEntities(Message(MessageID::UPDATE, &framePeriod));	
	}

	void GameState::draw() const
	{
		_layersManager->draw();
	}

	GameState* GameStateManager::getTopState() const
	{
		return _states.back();
	}

	void GameStateManager::init(const char* gameStateFile)
	{
		GameState* initial = ResourceManager::get().loadGameState(gameStateFile);
		_states.push_back(initial);
	}

	void GameStateManager::dispose()
	{
		for(GameStateIterator i = _states.begin(); i != _states.end(); ++i)
			delete (*i);
	}

	void GameStateManager::update(float framePeriod)
	{
		if(_next != 0)
		{
			if(_pop)
				popState();
			_states.push_back(_next);
			_next = 0;
		}
		getTopState()->update(framePeriod);
	}

	void GameStateManager::draw() const
	{
		getTopState()->draw();
	}

	void GameStateManager::changeState(const char* state)
	{
		_pop = true;
		ResourceManager::get().queueLoadGameState(state);
	}

	void GameStateManager::pushState(const char* state)
	{
		_pop = false;
		ResourceManager::get().queueLoadGameState(state);
	}

	void GameStateManager::popState()
	{
		delete getTopState();
		_states.pop_back();
	}
}