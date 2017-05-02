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
		NavigationNode(const OBB& area, Hash id = Hash::INVALID) : _area(area), _id(id), _isDisabled(false) {}
		~NavigationNode();

		const OBB& getArea() const { return _area; }
		void addEdge(const NavigationEdge* edge) { _edges.push_back(edge); }
		const NavigationEdgeList& getEdges() const { return _edges; }
		Hash getId() const { return _id; }
		bool isDisabled() const { return _isDisabled;  }
		void setDisabled(bool isDisabled) { _isDisabled = isDisabled; }

	private:
		const OBB _area;
		Hash _id;
		NavigationEdgeList _edges;
		bool _isDisabled;

		NavigationNode(const NavigationNode&);
		NavigationNode& operator=(const NavigationNode&);
	};
}

#endif