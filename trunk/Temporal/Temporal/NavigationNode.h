#ifndef NAVIGATIONNODE_H
#define NAVIGATIONNODE_H

#include "Shapes.h"
#include <vector>

namespace Temporal
{
	class NavigationEdge;

	typedef std::vector<const NavigationEdge*> NavigationEdgeList;
	typedef NavigationEdgeList::const_iterator NavigationEdgeIterator;

	class NavigationNode
	{
	public:
		NavigationNode(const OBB& area) : _area(area) {}
		~NavigationNode();

		const OBB& getArea() const { return _area; }
		void addEdge(const NavigationEdge* edge) { _edges.push_back(edge); }
		const NavigationEdgeList& getEdges() const { return _edges; }

	private:
		const OBB _area;
		NavigationEdgeList _edges;

		NavigationNode(const NavigationNode&);
		NavigationNode& operator=(const NavigationNode&);
	};
}

#endif