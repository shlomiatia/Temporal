#pragma once

#include <Temporal\Base\Enums.h>
#include <Temporal\Base\Vector.h>
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
		};
	}

	class NavigationNode
	{
	public:
		NavigationNode(const Rect& area) : _area(area) {}

		const Rect& getArea(void) const { return _area; }

	private:
		const Rect _area;

		NavigationNode(const NavigationNode&);
		NavigationNode& operator=(const NavigationNode&);
	};

	class NavigationEdge
	{
	public:
		NavigationEdge(const NavigationNode& node1, const NavigationNode& node2, float x, Orientation::Enum orientation, NavigationEdgeType::Enum type)
			: _node1(node1), _node2(node2), _x(x), _orientation(orientation), _type(type) {}

		const NavigationNode& getNode1(void) const { return _node1; }
		const NavigationNode& getNode2(void) const { return _node2; }
		float getX(void) const { return _x; }
		Orientation::Enum getOrientation(void) const { return _orientation; }
		NavigationEdgeType::Enum getType(void) const { return _type; }

	private:
		const NavigationNode& _node1;
		const NavigationNode& _node2;
		const float _x;
		const Orientation::Enum _orientation;
		const NavigationEdgeType::Enum _type;

		NavigationEdge(const NavigationEdge&);
		NavigationEdge& operator=(const NavigationEdge&);
	};

	class NavigationGraph
	{
	public:
		NavigationGraph(std::vector<const Rect>& platforms);

		void draw(void) const;

	private:
		static const Vector MIN_AREA_SIZE;

		// TODO: Use actual jump values?
		static const float MAX_JUMP_UP_DISTANCE;
		static const float MAX_JUMP_FORWARD_DISTANCE;

		std::vector<const NavigationNode* const> _nodes; 
		std::vector<const NavigationEdge* const> _edges; 

		void checkVerticalEdges(const NavigationNode& node1, const NavigationNode& node2, float x, Orientation::Enum orientation, std::vector<const Rect>& platforms);
		void checkHorizontalEdges(const NavigationNode& node1, const NavigationNode& node2, std::vector<const Rect>& platforms);
		void createNodes(std::vector<const Rect>& platforms);
		void createEdges(std::vector<const Rect>& platforms);

		NavigationGraph(const NavigationGraph&);
		NavigationGraph& operator=(const NavigationGraph&);
	};
}