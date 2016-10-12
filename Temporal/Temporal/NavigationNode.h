#ifndef NAVIGATIONNODE_H
#define NAVIGATIONNODE_H

#include "Shapes.h"
#include "Hash.h"
#include <vector>

namespace Temporal
{
	class NavigationEdge;

	typedef std::vector<const NavigationEdge*> NavigationEdgeList;
	typedef NavigationEdgeList::const_iterator NavigationEdgeIterator;

	class NavigationNode
	{
	public:
		NavigationNode(const OBB& area, Hash doorId = Hash::INVALID) : _area(area), _doorId(doorId) {}
		~NavigationNode();

		const OBB& getArea() const { return _area; }
		void addEdge(const NavigationEdge* edge) { _edges.push_back(edge); }
		const NavigationEdgeList& getEdges() const { return _edges; }
		Hash getDoorId() const { return _doorId; }

	private:
		const OBB _area;
		Hash _doorId;
		NavigationEdgeList _edges;

		NavigationNode(const NavigationNode&);
		NavigationNode& operator=(const NavigationNode&);
	};
}

#endif