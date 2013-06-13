#ifndef GAMESTATEMANAGER_H
#define GAMESTATEMANAGER_H

#include <vector>

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

	typedef std::vector<GameState*> GameStateCollection;
	typedef GameStateCollection::const_iterator GameStateIterator;

	class GameStateManager
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

		void gameStateReady(GameState* next) { _next = next; }

		void changeState(const char* gameState);
		void pushState(const char* gameState);
		void popState();
	private:
		GameStateCollection _states;
		bool _pop;
		GameState* _next;

		GameStateManager() : _pop(false), _next(0) {};

		GameState* getTopState() const;

		GameStateManager(const GameStateManager&);
		GameStateManager& operator=(const GameStateManager&);
	};
}
#endif