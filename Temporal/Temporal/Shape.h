#ifndef SHAPE_H
#define SHAPE_H

#include "BaseEnums.h"

namespace Temporal
{
	namespace ShapeType
	{
		enum Enum
		{
			AABB,
			SEGMENT,
			YABP
		};
	}

	class Shape
	{
	public:
		virtual ~Shape() {}
		
		virtual ShapeType::Enum getType() const = 0;

		virtual float getBottom() const = 0;
		virtual float getLeft() const = 0;
		virtual float getTop() const = 0;
		virtual float getRight() const = 0;

		float getSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getLeft() : getRight(); }
		float getOppositeSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getRight() : getLeft(); }
	};
}
#endif