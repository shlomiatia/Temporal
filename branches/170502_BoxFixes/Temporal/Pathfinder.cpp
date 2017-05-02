#include "Pathfinder.h"
#include "NavigationEdge.h"
#include <assert.h>

#include "GameState.h"
#include "EntitySystem.h"
#include "StaticBody.h"
#include "PhysicsEnums.h"
#include "Fixture.h"

namespace Temporal
{
	typedef std::vector<PathNode*> PathNodeList;
	typedef PathNodeList::const_iterator PathNodeIterator;

	float PathNode::calculateHeuristicCost(const NavigationNode& goal)
	{
		const OBB& sourceArea = getNavigationNode().getArea();
		const OBB& goalArea = goal.getArea();

		Segment segment(sourceArea.getCenter(), goalArea.getCenter());
		return segment.getLength();
	}

	NavigationEdgeList* buildPath(const PathNode& pathNode)
	{
		NavigationEdgeList* result = new NavigationEdgeList();

		const PathNode* current = &pathNode;
		while(current->getEdge())
		{
			result->insert(result->begin(), current->getEdge());
			current = current->getPrevious();
		}
		return result;
	}

	PathNode* find(PathNodeList& where, const NavigationNode* what)
	{
		for(PathNodeIterator i = where.begin(); i != where.end(); ++i)
		{
			PathNode* current = *i;
			if(&(current->getNavigationNode()) == what)
			{
				return current;
			}
		}
		return 0;
	}

	void remove(PathNodeList& where, const PathNode* what)
	{
		for(PathNodeIterator i = where.begin(); i != where.end(); ++i)
		{
			PathNode* current = *i;
			if(current == what)
			{
				where.erase(i);
				return;
			}
		}
	}

	void add(PathNodeList& where, PathNode* what)
	{
		PathNodeIterator smaller;
		for(smaller = where.begin(); smaller != where.end(); ++smaller)
		{
			if((**smaller).getEstaminedPathCost() < what->getEstaminedPathCost())
				break;
		}
		where.insert(smaller, what);
	}

	void deleteList(PathNodeList& collection)
	{
		for(PathNodeIterator i = collection.begin(); i != collection.end(); ++i)
			delete *i;
	}

	NavigationEdgeList* Pathfinder::findPath(const NavigationNode* start, const NavigationNode* goal) const
	{
		assert(start);
		assert(goal);
		PathNodeList open;
		PathNodeList closed;
		open.push_back(new PathNode(*start, *goal));
		// Check all open nodes
		while (!open.empty())
		{
			PathNode& pathNode = *open.back();
			const NavigationNode& navigationNode = pathNode.getNavigationNode();

			// Reached goal
			if (&navigationNode == goal)
			{
				NavigationEdgeList* result = buildPath(pathNode);
				deleteList(open);
				deleteList(closed);
				return result;
			}
			
			// Move from open to back
			open.pop_back();
			closed.push_back(&pathNode);

			// Explore all neighbours
			const NavigationEdgeList& edges = navigationNode.getEdges();
			for(NavigationEdgeIterator i = edges.begin(); i != edges.end(); ++i)
			{
				const NavigationEdge& edge = **i;
				const NavigationNode& navigationNeighbour = edge.getTarget();

				if (navigationNeighbour.isDisabled())
					continue;

				// Don't explore closed neighbours
				if(find(closed, &navigationNeighbour))
					continue;

				// Find cost from start through current edge
				float tentativeCostFromStart = pathNode.getCostFromStart() + edge.getCost();

				// If node wasn't explore, we use the current edge
				PathNode* pathNeighbour = find(open, &navigationNeighbour);
				if(!pathNeighbour)
				{
					pathNeighbour = new PathNode(navigationNeighbour, *goal);
				}
				// If current edge is worse then another, don't bother
				else if(tentativeCostFromStart >= pathNeighbour->getCostFromStart())
				{
					continue;
				}
				// If current edge is better, remove the node so we can insert it again with the new cost
				else
				{
					remove(open, pathNeighbour);
				}
				pathNeighbour->setPrevious(&pathNode);
				pathNeighbour->setEdge(&edge);
				pathNeighbour->setCostFromStart(tentativeCostFromStart);
				add(open, pathNeighbour);
			}
		}
		deleteList(open);
		deleteList(closed);
		return 0;
	}
}