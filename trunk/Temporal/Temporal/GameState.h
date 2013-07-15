#ifndef GAMESTATEMANAGER_H
#define GAMESTATEMANAGER_H

#include "Hash.h"
#include "ResourceManager.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace Temporal
{
	class LayersManager;
	class Grid;
	class EntitiesManager;
	class NavigationGraph;

	class GameState
	{
	public:
		GameState();
		~GameState();

		Grid& getGrid() const { return *_grid; }
		EntitiesManager& getEntitiesManager() const { return *_entitiesManager; }
		NavigationGraph& getNavigationGraph() const { return *_navigationGraph; }
		LayersManager& getLayersManager() const { return *_layersManager; }

		void init();
		void update(float framePeriod);
		void draw() const;
		
	private:
		Grid* _grid;
		EntitiesManager* _entitiesManager;
		NavigationGraph* _navigationGraph;
		LayersManager* _layersManager;

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

	typedef std::unordered_map<Hash, GameState*> GameStateMap;
	typedef GameStateMap::const_iterator GameStateMapIterator;

	class GameStateListener
	{
	public:
		GameStateListener() {}
		virtual void onUpdate(float framePeriod, GameState& gameState) = 0;
		virtual void onLoaded(Hash id, GameState& gameState) = 0;
	private:
		GameStateListener(const GameStateListener&);
		GameStateListener& operator=(const GameStateListener&);
	};

	class GameStateManager
	{
	public:
		static GameStateManager& get()
		{
			static GameStateManager instance;
			return (instance);
		}

		void setListener(GameStateListener* listener) { _listener = listener; }

		void init(const char* gameStateFile);
		void dispose();

		void update(float framePeriod);
		void draw() const;

		void load(GameStateLoader* loader);
		void unload(StringCollection files);
		void show(const char* gameStateFile);

	private:
		GameStateMap _states;
		
		GameStateListener* _listener;
		GameStateLoader* _loader;
		bool _unload;
		StringCollection _files;
		Hash _currentStateId;
		Hash _nextStateId;

		GameStateManager() : _currentStateId(Hash::INVALID), _nextStateId(Hash::INVALID), _unload(false), _loader(0), _listener(0) {};

		GameState* getCurrentState() const;
		void load();
		void unload();
		Hash add(const char* file, GameState* state);

		GameStateManager(const GameStateManager&);
		GameStateManager& operator=(const GameStateManager&);
	};
}
#endif