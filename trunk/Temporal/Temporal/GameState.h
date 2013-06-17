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

		void loaded();
		
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

	typedef std::unordered_map<Hash, GameState*> GameStateCollection;
	typedef GameStateCollection::const_iterator GameStateIterator;
	typedef std::vector<std::string> StringCollection;
	typedef StringCollection::const_iterator StringIterator;


	class GameStateManager : public IOJob
	{
	public:
		static GameStateManager& get()
		{
			static GameStateManager instance;
			return (instance);
		}

		void init(const char* gameStateFile);
		void dispose();

		void update(float framePeriod);
		void draw() const;

		void load(StringCollection files);
		void unload(StringCollection files);
		void show(const char* gameStateFile);

		void* load();
		void loaded(void* param);
		void unload();
	private:
		GameStateCollection _states;
		
		bool _unload;
		StringCollection _files;
		Hash _currentStateId;
		Hash _nextStateId;

		GameStateManager() : _currentStateId(Hash::INVALID), _nextStateId(Hash::INVALID), _unload(false) {};

		GameState* getCurrentState() const;

		GameStateManager(const GameStateManager&);
		GameStateManager& operator=(const GameStateManager&);
	};
}
#endif