#ifndef NAVIGATIONGRAPH_H
#define NAVIGATIONGRAPH_H

#include <Temporal\Base\BaseEnums.h>
#include <Temporal\Base\NumericPair.h>
#include <Temporal\Base\Rect.h>
#include <vector>

namespace Temporal
{
	namespace NavigationEdgeType
	{
		enum Enum
		{
			FALL,
			JUMP,
			DESCEND
		};
	}

	class NavigationEdge;

	typedef std::vector<const NavigationEdge*> NavigationEdgeCollection;
	typedef NavigationEdgeCollection::const_iterator NavigationEdgeIterator;

	class NavigationNode
	{
	public:
		NavigationNode(const Rect& area) : _area(area) {}
		~NavigationNode(void);

		const Rect& getArea(void) const { return _area; }
		void addEdge(const NavigationEdge* edge) { _edges.push_back(edge); }
		const NavigationEdgeCollection& getEdges(void) const { return _edges; }

	private:
		const Rect _area;
		NavigationEdgeCollection _edges;

		NavigationNode(const NavigationNode&);
		NavigationNode& operator=(const NavigationNode&);
	};

	class NavigationEdge
	{
	public:
		NavigationEdge(const NavigationNode& source, const NavigationNode& target, float x, Orientation::Enum orientation, NavigationEdgeType::Enum type)
			: _target(target), _x(x), _orientation(orientation), _type(type), _cost(calculateCost(source)) {}

		const NavigationNode& getTarget(void) const { return _target; }
		float getX(void) const { return _x; }
		Orientation::Enum getOrientation(void) const { return _orientation; }
		NavigationEdgeType::Enum getType(void) const { return _type; }
		float getCost(void) const { return _cost; }
		void draw(void) const;

	private:
		const NavigationNode& _target;
		const float _x;
		const Orientation::Enum _orientation;
		const NavigationEdgeType::Enum _type;
		const float _cost;

		float calculateCost(const NavigationNode& source);

		NavigationEdge(const NavigationEdge&);
		NavigationEdge& operator=(const NavigationEdge&);
	};

	typedef std::vector<NavigationNode*> NavigationNodeCollection;
	typedef NavigationNodeCollection::const_iterator NavigationNodeIterator;
	typedef std::vector<const Rect> RectCollection;
	typedef RectCollection::const_iterator RectIterator;

	class NavigationGraph
	{
	public:
		static NavigationGraph& get(void)
		{
			static NavigationGraph instance;
			return instance;
		}

		void init(void);
		void dispose(void);
		const NavigationNode* getNodeByPosition(const Point& position) const;
		void draw(void) const;

	private:
		static const Size MIN_AREA_SIZE;

		NavigationNodeCollection _nodes; 

		void createNodes(RectCollection& platforms);
		void checkVerticalEdges(NavigationNode& node1, NavigationNode& node2, float x, Orientation::Enum orientation, RectCollection& platforms);
		void checkHorizontalEdges(NavigationNode& node1, NavigationNode& node2, RectCollection& platforms);
		void createEdges(RectCollection& platforms);

		NavigationGraph(void) {}
		NavigationGraph(const NavigationGraph&);
		NavigationGraph& operator=(const NavigationGraph&);
	};
}
#endif