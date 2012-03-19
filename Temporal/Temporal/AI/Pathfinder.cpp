#include "Pathfinder.h"
#include <assert.h>

namespace Temporal
{
	float PathNode::calculateHeuristicCost(const NavigationNode& goal)
	{
		const Rect& sourceArea = getNavigationNode().getArea();
		const Rect& goalArea = goal.getArea();

		float x1 = sourceArea.getCenterX();
		float x2 = goalArea.getCenterX();
		float y1 = sourceArea.getCenterY();
		float y2 = goalArea.getCenterY();

		return sqrt(pow(x2-x1, 2.0f) + pow(y2-y1, 2.0f));
	}

	std::vector<const NavigationEdge* const>* buildPath(const PathNode& pathNode)
	{
		std::vector<const NavigationEdge* const>* result = new std::vector<const NavigationEdge* const>();

		const PathNode* current = &pathNode;
		while(current->getEdge() != NULL)
		{
			result->insert(result->begin(), current->getEdge());
			current = current->getPrevious();
		}
		return result;
	}

	PathNode* find(std::vector<PathNode* const>& where, const NavigationNode* what)
	{
		for(unsigned int i = 0; i < where.size(); ++i)
		{
			PathNode* current = where[i];
			if(&(current->getNavigationNode()) == what)
			{
				return current;
			}
		}
		return NULL;
	}

	void remove(std::vector<PathNode* const>& where, const PathNode* what)
	{
		for(std::vector<PathNode* const>::iterator i = where.begin(); i != where.end(); ++i)
		{
			PathNode* current = *i;
			if(current == what)
			{
				where.erase(i);
				return;
			}
		}
	}

	void add(std::vector<PathNode* const>& where, PathNode* what)
	{
		std::vector<PathNode* const>::iterator smaller;
		for(smaller = where.begin(); smaller != where.end(); ++smaller)
		{
			if((**smaller).getEstaminedPathCost() < what->getEstaminedPathCost())
				break;
		}
		where.insert(smaller, what);
	}

	void deleteCollection(std::vector<PathNode* const>& collection)
	{
		for(std::vector<PathNode* const>::iterator i = collection.begin(); i != collection.end(); ++i)
			delete *i;
	}

	std::vector<const NavigationEdge* const>* Pathfinder::findPath(const NavigationNode* start, const NavigationNode* goal) const
	{
		assert(start);
		assert(goal);

		if (start == goal)
			return NULL;

		std::vector<PathNode* const> open;
		std::vector<PathNode* const> closed;
		open.push_back(new PathNode(*start, *goal));
		// Check all open nodes
		while (!open.empty())
		{
			PathNode& pathNode = *open.back();
			const NavigationNode& navigationNode = pathNode.getNavigationNode();

			// Reached goal
			if (&navigationNode == goal)
			{
				std::vector<const NavigationEdge* const>* result = buildPath(pathNode);
				deleteCollection(open);
				deleteCollection(closed);
				return result;
			}
			
			// Move from open to back
			open.pop_back();
			closed.push_back(&pathNode);

			// Explore all neighbours
			const std::vector<const NavigationEdge* const>& edges = navigationNode.getEdges();
			for(unsigned int i = 0; i < edges.size(); ++i)
			{
				const NavigationEdge& edge = *edges[i];
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