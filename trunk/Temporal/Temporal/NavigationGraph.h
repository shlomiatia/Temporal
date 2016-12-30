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
		NavigationNode* getNode(const Vector& position, int period);
		const NavigationNode* getNode(Hash id, int period) const;
		NavigationNode* getNode(Hash id, int period);
		void draw() const;

	private:
		NavigationNodeList _pastNodes;
		NavigationNodeList _presentNodes;

		const NavigationNodeList& getNodesByPeriod(int period) const;
		
		NavigationGraph(const NavigationGraph&);
		NavigationGraph& operator=(const NavigationGraph&);
	};
}
#endif