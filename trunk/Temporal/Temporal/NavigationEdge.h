#ifndef NAVIGATIONEDGE_H
#define NAVIGATIONEDGE_H

#include "BaseEnums.h"

namespace Temporal
{
	class NavigationNode;

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
}

#endif