#pragma once

#include <Temporal\Base\Enums.h>
#include <Temporal\Base\Vector.h>
#include <Temporal\Base\Rect.h>
#include <vector>

namespace Temporal
{
	namespace NavigationEdgeType
	{
		// TODO: Use string for this ENTITIES
		enum Enum
		{
			FALL,
			JUMP,
			DESCEND
		};
	}

	class NavigationEdge;

	class NavigationNode
	{
	public:
		NavigationNode(const Rect& area) : _area(area) {}
		~NavigationNode(void);

		const Rect& getArea(void) const { return _area; }
		void addEdge(const NavigationEdge* edge) { _edges.push_back(edge); }
		const std::vector<const NavigationEdge* const>& getEdges(void) const { return _edges; }

	private:
		const Rect _area;
		// TODO: Fix this freaking const stuff SLOTH
		std::vector<const NavigationEdge* const> _edges;

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

	class NavigationGraph
	{
	public:
		static NavigationGraph& get(void)
		{
			static NavigationGraph instance;
			return instance;
		}

		void init(std::vector<const Rect>& platforms);
		void dispose(void);
		const NavigationNode* getNodeByPosition(const Vector& position) const;
		void draw(void) const;

	private:
		static const Vector MIN_AREA_SIZE;

		// TODO: Use actual jump values? Unify SLOTH
		static const float MAX_JUMP_UP_DISTANCE;
		static const float MAX_JUMP_FORWARD_DISTANCE;
		static const float MIN_FALL_DISTANCE;

		std::vector<NavigationNode* const> _nodes; 

		void createNodes(std::vector<const Rect>& platforms);
		void checkVerticalEdges(NavigationNode& node1, NavigationNode& node2, float x, Orientation::Enum orientation, std::vector<const Rect>& platforms);
		void checkHorizontalEdges(NavigationNode& node1, NavigationNode& node2, std::vector<const Rect>& platforms);
		void createEdges(std::vector<const Rect>& platforms);

		NavigationGraph(void) {}
		NavigationGraph(const NavigationGraph&);
		NavigationGraph& operator=(const NavigationGraph&);
	};
}