#ifndef NAVIGATIONGRAPHGENERATOR_H
#define NAVIGATIONGRAPHGENERATOR_H

#include "Shapes.h"
#include <vector>

namespace Temporal
{
	class Grid;
	class NavigationNode;

	typedef std::vector<NavigationNode*> NavigationNodeList;
	typedef NavigationNodeList::const_iterator NavigationNodeIterator;
	typedef std::vector<const OBB> OBBList;
	typedef OBBList::const_iterator OBBIterator;

	class NavigationGraphGenerator
	{
	public:
		NavigationGraphGenerator(const Grid& grid, OBBList& platforms);

		NavigationNodeList get() const { return _nodes; }
	private:
		static const Vector MIN_AREA_SIZE;

		const Grid& _grid;
		NavigationNodeList _nodes;

		void createNodes(OBBList& platforms);
		void checkFallVerticalEdges(NavigationNode& node1, NavigationNode& node2, float x, Side::Enum orientation, OBBList& platforms);
		void checkClimbDescendVerticalEdges(NavigationNode& node1, NavigationNode& node2, OBBList& platforms);
		void checkHorizontalEdges(NavigationNode& node1, NavigationNode& node2, OBBList& platforms);
		void createEdges(OBBList& platforms);
		void cutAreasByPlatforms(OBBList& areas, OBBList& platforms);

		NavigationGraphGenerator(const NavigationGraphGenerator&);
		NavigationGraphGenerator& operator=(const NavigationGraphGenerator&);
	};
}

#endif