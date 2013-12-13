#ifndef NAVIGATIONGRAPH_H
#define NAVIGATIONGRAPH_H
/*
#include "GameState.h"
#include "BaseEnums.h"
#include "Vector.h"
#include "Shapes.h"
#include <vector>

namespace Temporal
{
	class NavigationEdge;

	namespace NavigationEdgeType
	{
		enum Enum
		{
			WALK,
			FALL,
			JUMP_UP,
			JUMP_FORWARD,
			DESCEND
		};
	}

	typedef std::vector<const NavigationEdge*> NavigationEdgeCollection;
	typedef NavigationEdgeCollection::const_iterator NavigationEdgeIterator;

	class NavigationNode
	{
	public:
		NavigationNode(const OBB& area) : _area(area) {}
		~NavigationNode();

		const OBB& getArea() const { return _area; }
		void addEdge(const NavigationEdge* edge) { _edges.push_back(edge); }
		const NavigationEdgeCollection& getEdges() const { return _edges; }

	private:
		const OBB _area;
		NavigationEdgeCollection _edges;

		NavigationNode(const NavigationNode&);
		NavigationNode& operator=(const NavigationNode&);
	};

	class NavigationEdge
	{
	public:
		NavigationEdge(const NavigationNode& source, const NavigationNode& target, float x, Side::Enum orientation, NavigationEdgeType::Enum type)
			: _target(target), _x(x), _orientation(orientation), _type(type), _cost(calculateCost(source)) {}

		const NavigationNode& getTarget() const { return _target; }
		float getX() const { return _x; }
		Side::Enum getSide() const { return _orientation; }
		NavigationEdgeType::Enum getType() const { return _type; }
		float getCost() const { return _cost; }
		void draw() const;

	private:
		const NavigationNode& _target;
		const float _x;
		const Side::Enum _orientation;
		const NavigationEdgeType::Enum _type;
		const float _cost;

		float calculateCost(const NavigationNode& source);

		NavigationEdge(const NavigationEdge&);
		NavigationEdge& operator=(const NavigationEdge&);
	};

	typedef std::vector<NavigationNode*> NavigationNodeCollection;
	typedef NavigationNodeCollection::const_iterator NavigationNodeIterator;
	typedef std::vector<const OBB> YABPCollection;
	typedef YABPCollection::const_iterator YABPIterator;
	typedef std::vector<const OBB*> ShapeCollection;
	typedef ShapeCollection::const_iterator ShapeIterator;

	class NavigationGraph : public GameStateComponent
	{
	public:
		NavigationGraph() {};
		~NavigationGraph();

		void init(GameState* gameState);

		const NavigationNode* getNode(const OBB& shape) const;
		void draw() const;

	private:
		static const Vector MIN_AREA_SIZE;

		NavigationNodeCollection _nodes; 

		void createNodes(ShapeCollection& platforms);
		void checkVerticalEdges(NavigationNode& node1, NavigationNode& node2, float x, Side::Enum orientation, ShapeCollection& platforms);
		void checkVerticalEdges(NavigationNode& node1, NavigationNode& node2, ShapeCollection& platforms);
		void checkHorizontalEdges(NavigationNode& node1, NavigationNode& node2, ShapeCollection& platforms);
		void createEdges(ShapeCollection& platforms);
		
		NavigationGraph(const NavigationGraph&);
		NavigationGraph& operator=(const NavigationGraph&);
	};
}*/
#endif