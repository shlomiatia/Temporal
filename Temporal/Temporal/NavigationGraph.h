#ifndef NAVIGATIONGRAPH_H
#define NAVIGATIONGRAPH_H

#include "GameState.h"
#include <vector>

namespace Temporal
{
	class NavigationNode;
	class Vector;

	typedef std::vector<NavigationNode*> NavigationNodeList;
	typedef NavigationNodeList::const_iterator NavigationNodeIterator;

	class NavigationGraph : public GameStateComponent
	{
	public:
		NavigationGraph() {};
		~NavigationGraph();

		void init(GameState* gameState);

		const NavigationNode* getNode(const Vector& position, int period) const;
		void draw() const;

	private:
		NavigationNodeList _pastNodes;
		NavigationNodeList _presentNodes;
		
		NavigationGraph(const NavigationGraph&);
		NavigationGraph& operator=(const NavigationGraph&);
	};
}
#endif