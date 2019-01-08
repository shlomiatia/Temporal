#ifndef NAVIGATIONGRAPHGENERATOR_H
#define NAVIGATIONGRAPHGENERATOR_H

#include "Shapes.h"
#include "TemporalPeriod.h"
#include <vector>

namespace Temporal
{
	class GameState;
	class NavigationNode;

	typedef std::vector<NavigationNode*> NavigationNodeList;
	typedef NavigationNodeList::const_iterator NavigationNodeIterator;
	typedef std::vector<OBB> OBBList;
	typedef OBBList::const_iterator OBBIterator;

	class NavigationGraphGenerator
	{
	public:
		NavigationGraphGenerator(const GameState& gameState, Period::Enum period);

		NavigationNodeList get() const { return _nodes; }
	private:
		static const Vector MIN_AREA_SIZE;

		const GameState& _gameState;
		NavigationNodeList _nodes;

		void createNodes(OBBList& platforms);
		void checkFallVerticalEdges(NavigationNode& node1, NavigationNode& node2, float x, Side::Enum orientation, OBBList& platforms);
		void getMinMaxX(NavigationNode& node1, NavigationNode& node2, float& minX, float& maxX);
		void checkClimbDescendVerticalEdges(NavigationNode& node1, NavigationNode& node2, OBBList& platforms);
		void checkHorizontalEdges(NavigationNode& node1, NavigationNode& node2, OBBList& platforms);
		void createEdges(OBBList& platforms);
		void cutAreasByPlatforms(OBBList& areas, OBBList& platforms);
		void createPlatforms(Period::Enum periodType, OBBList& platforms);

		NavigationGraphGenerator(const NavigationGraphGenerator&);
		NavigationGraphGenerator& operator=(const NavigationGraphGenerator&);
	};
}

#endif