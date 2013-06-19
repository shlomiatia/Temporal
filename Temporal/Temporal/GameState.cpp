#include "GameState.h"
#include "Grid.h"
#include "Layer.h"
#include "NavigationGraph.h"
#include "EntitySystem.h"
#include "ResourceManager.h"
#include "Keyboard.h"
#include "Input.h"
#include "Game.h"
#include "Serialization.h"
#include <stdio.h>

namespace Temporal
{
	class TestComponent : public Component
	{
	public:
		ComponentType::Enum getType() const { return ComponentType::OTHER; }
		virtual Component* clone() const { return 0; }
		void handleMessage(Message& message)
		{
			if(message.getID() == MessageID::UPDATE)
			{
				if(Keyboard::get().getKey(Key::Q))
				{
					if(!_loader.isStarted())
					{
						_loader.add("resources/game-states/entities.xml");
						GameStateManager::get().load(&_loader);
					}
				}
				if(_loader.isFinished())
				{
					StringCollection files;
					files.push_back(std::string("resources/game-states/loading.xml"));
					GameStateManager::get().unload(files);
					GameStateManager::get().show("resources/game-states/entities.xml");
				}
			}
		}
	private:
		GameStateLoader _loader;
	};

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
			//const AABB& bounds = *static_cast<AABB*>(getEntity().getManager().sendMessageToEntity(Hash("ENT_PLAYER"), Message(MessageID::GET_SHAPE)));
			//getEntity().getManager().sendMessageToEntity(Hash("ENT_CHASER"), Message(MessageID::SET_NAVIGATION_DESTINATION, const_cast<AABB*>(&bounds)));
			//Stream* stream = new MemoryStream();
			//getEntitiesManager().sendMessageToAllEntities(Message(MessageID::SAVE, stream));
			//getEntitiesManager().sendMessageToAllEntities(Message(MessageID::MERGE_TO_TEMPORAL_ECHOES));
		}	
		if(Keyboard::get().getKey(Key::W))
		{
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
		if(id == Hash("resources/game-states/loading.xml"))
		{
			Entity* entity = new Entity();
			entity->add(new TestComponent());
			_states[id]->getEntitiesManager().add(Hash("ENT_TEST"), entity);
		}
		return id;
	}
}