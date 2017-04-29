#ifndef GAMESTATEMANAGER_H
#define GAMESTATEMANAGER_H

#include "Hash.h"
#include <vector>
#include <unordered_map>

namespace Temporal
{
	class LayersManager;
	class Grid;
	class EntitiesManager;
	class NavigationGraph;
	class EntityTemplatesManager;

	class GameState
	{
	public:
		GameState();
		~GameState();

		Grid& getGrid() const { return *_grid; }
		EntitiesManager& getEntitiesManager() const { return *_entitiesManager; }
		NavigationGraph& getNavigationGraph() const { return *_navigationGraph; }
		LayersManager& getLayersManager() const { return *_layersManager; }
		EntityTemplatesManager& getEntityTemplatesManager() const { return *_entityTemplatesManager; }

		void init();
		void update(float framePeriod);
		void draw() const;

		void setUpdateFilter(HashList updateFilter) { _updateFilter = updateFilter; }
		
	private:
		HashList _updateFilter;
		Grid* _grid;
		EntitiesManager* _entitiesManager;
		NavigationGraph* _navigationGraph;
		LayersManager* _layersManager;
		EntityTemplatesManager* _entityTemplatesManager;

		GameState(const GameState&);
		GameState& operator=(const GameState&);

		friend class SerializationAccess;
	};

	class GameStateComponent
	{
	public:
		GameStateComponent() {}
		virtual ~GameStateComponent() {}
		virtual void init(GameState* gameState) { _gameState = gameState; }
		GameState& getGameState() { return *_gameState; }
	private:
		GameState* _gameState;

		GameStateComponent(const GameStateComponent&);
		GameStateComponent& operator=(const GameStateComponent&);
	};

	class GameStateListener
	{
	public:
		GameStateListener() {}
		virtual void onLoaded(Hash id, GameState& gameState) {};
	private:
		GameStateListener(const GameStateListener&);
		GameStateListener& operator=(const GameStateListener&);
	};

	class BasicGameStateListener : public GameStateListener
	{
	public:
		void onLoaded(Hash id, GameState& gameState);
	private:
	};

	typedef std::unordered_map<Hash, GameState*> HashGameStateMap;
	typedef HashGameStateMap::const_iterator GameStateIterator;

	class GameStateManager
	{
	public:
		static GameStateManager& get()
		{
			static GameStateManager instance;
			return (instance);
		}

		void setListener(GameStateListener* listener) { _listener = listener; }
		GameStateListener* getListener() const { return _listener; }

		void init(const char* gameStateFile);
		void dispose();

		void update(float framePeriod);
		void draw() const;

		void syncLoadAndShow(const char* gameStateFile);
		void syncUnloadCurrent();

		GameState& getCurrentState() const; 

	private:
		HashGameStateMap _states;
		
		GameStateListener* _listener;
		Hash _currentStateId;
		Hash _nextStateId;
		bool _unload;

		Hash load(const char* gameStateFile);

		GameStateManager() : _currentStateId(Hash::INVALID), _nextStateId(Hash::INVALID), _listener(0), _unload(false){};

		GameStateManager(const GameStateManager&);
		GameStateManager& operator=(const GameStateManager&);
	};
}
#endif