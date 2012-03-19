#pragma once

#include "NavigationGraph.h"

namespace Temporal
{
	class PathNode
	{
	public:
		PathNode(const NavigationNode& navigationNode, const NavigationNode& goal) 
			: _navigationNode(navigationNode), _costFromStart(0.0f), _previous(NULL), _edge(NULL), _heuristicCostToGoal(calculateHeuristicCost(goal)) {}

		const NavigationNode& getNavigationNode(void) const { return _navigationNode; }
		void setCostFromStart(float costFromStart) { _costFromStart = costFromStart; }
		float getCostFromStart(void) const { return _costFromStart; }
		float getHeuristicCostToGoal(void) const { return _heuristicCostToGoal; }
		float getEstaminedPathCost(void) const { return getCostFromStart() + getHeuristicCostToGoal(); }
		const PathNode* getPrevious(void) const { return _previous; }
		void setPrevious(const PathNode* previous) { _previous = previous; }
		const NavigationEdge* getEdge(void) const { return _edge; }
		void setEdge(const NavigationEdge* edge) { _edge = edge; }

	private:
		const NavigationNode& _navigationNode;
		float _costFromStart;
		float _heuristicCostToGoal;
		const PathNode* _previous;
		const NavigationEdge* _edge;

		float calculateHeuristicCost(const NavigationNode& goal);

		PathNode(const PathNode&);
		PathNode& operator=(const PathNode&);
	};

	class Pathfinder
	{
	public:
		static Pathfinder& get(void)
		{
			static Pathfinder instance;
			return instance;
		}

		std::vector<const NavigationEdge*>* findPath(const NavigationNode* start, const NavigationNode* goal) const;
	private:
		Pathfinder(void) {}
		Pathfinder(const Pathfinder&);
		Pathfinder& operator=(const Pathfinder&);
	};
}