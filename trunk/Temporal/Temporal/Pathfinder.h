#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "NavigationGraph.h"

namespace Temporal
{
	class PathNode
	{
	public:
		PathNode(const NavigationNode& navigationNode, const NavigationNode& goal) 
			: _navigationNode(navigationNode), _costFromStart(0.0f), _previous(NULL), _edge(NULL), _heuristicCostToGoal(calculateHeuristicCost(goal)) {}

		const NavigationNode& getNavigationNode() const { return _navigationNode; }
		void setCostFromStart(float costFromStart) { _costFromStart = costFromStart; }
		float getCostFromStart() const { return _costFromStart; }
		float getHeuristicCostToGoal() const { return _heuristicCostToGoal; }
		float getEstaminedPathCost() const { return getCostFromStart() + getHeuristicCostToGoal(); }
		const PathNode* getPrevious() const { return _previous; }
		void setPrevious(const PathNode* previous) { _previous = previous; }
		const NavigationEdge* getEdge() const { return _edge; }
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
		static Pathfinder& get()
		{
			static Pathfinder instance;
			return instance;
		}

		NavigationEdgeCollection* findPath(const NavigationNode* start, const NavigationNode* goal) const;
	private:
		Pathfinder() {}
		Pathfinder(const Pathfinder&);
		Pathfinder& operator=(const Pathfinder&);
	};
}
#endif