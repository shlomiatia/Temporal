#include "Pathfinder.h"
#include <Temporal\Base\Segment.h>
#include <assert.h>

namespace Temporal
{
	typedef std::vector<PathNode*> PathNodeCollection;
	typedef PathNodeCollection::const_iterator PathNodeIterator;

	float PathNode::calculateHeuristicCost(const NavigationNode& goal)
	{
		const YABP& sourceArea = getNavigationNode().getArea();
		const YABP& goalArea = goal.getArea();

		Segment segment(sourceArea.getCenter(), goalArea.getCenter());
		return segment.getLength();
	}

	NavigationEdgeCollection* buildPath(const PathNode& pathNode)
	{
		NavigationEdgeCollection* result = new NavigationEdgeCollection();

		const PathNode* current = &pathNode;
		while(current->getEdge() != NULL)
		{
			result->insert(result->begin(), current->getEdge());
			current = current->getPrevious();
		}
		return result;
	}

	PathNode* find(PathNodeCollection& where, const NavigationNode* what)
	{
		for(PathNodeIterator i = where.begin(); i != where.end(); ++i)
		{
			PathNode* current = *i;
			if(&(current->getNavigationNode()) == what)
			{
				return current;
			}
		}
		return NULL;
	}

	void remove(PathNodeCollection& where, const PathNode* what)
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

	void add(PathNodeCollection& where, PathNode* what)
	{
		PathNodeIterator smaller;
		for(smaller = where.begin(); smaller != where.end(); ++smaller)
		{
			if((**smaller).getEstaminedPathCost() < what->getEstaminedPathCost())
				break;
		}
		where.insert(smaller, what);
	}

	void deleteCollection(PathNodeCollection& collection)
	{
		for(PathNodeIterator i = collection.begin(); i != collection.end(); ++i)
			delete *i;
	}

	NavigationEdgeCollection* Pathfinder::findPath(const NavigationNode* start, const NavigationNode* goal) const
	{
		assert(start);
		assert(goal);

		if (start == goal)
			return NULL;

		PathNodeCollection open;
		PathNodeCollection closed;
		open.push_back(new PathNode(*start, *goal));
		// Check all open nodes
		while (!open.empty())
		{
			PathNode& pathNode = *open.back();
			const NavigationNode& navigationNode = pathNode.getNavigationNode();

			// Reached goal
			if (&navigationNode == goal)
			{
				NavigationEdgeCollection* result = buildPath(pathNode);
				deleteCollection(open);
				deleteCollection(closed);
				return result;
			}
			
			// Move from open to back
			open.pop_back();
			closed.push_back(&pathNode);

			// Explore all neighbours
			const NavigationEdgeCollection& edges = navigationNode.getEdges();
			for(NavigationEdgeIterator i = edges.begin(); i != edges.end(); ++i)
			{
				const NavigationEdge& edge = **i;
				const NavigationNode& navigationNeighbour = edge.getTarget();

				// Don't explore closed neighbours
				if(find(closed, &navigationNeighbour) != NULL)
					continue;

				// Find cost from start through current edge
				float tentativeCostFromStart = pathNode.getCostFromStart() + edge.getCost();

				// If node wasn't explore, we use the current edge
				PathNode* pathNeighbour = find(open, &navigationNeighbour);
				if(pathNeighbour == NULL)
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
		deleteCollection(open);
		deleteCollection(closed);
		return NULL;
	}
}