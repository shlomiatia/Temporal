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
		virtual ~Shape(void) {}
		
		virtual ShapeType::Enum getType(void) const = 0;

		virtual float getBottom(void) const = 0;
		virtual float getLeft(void) const = 0;
		virtual float getTop(void) const = 0;
		virtual float getRight(void) const = 0;

		float getSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getLeft() : getRight(); }
		float getOppositeSide(Side::Enum orientation) const { return orientation == Side::LEFT ? getRight() : getLeft(); }
	};
}
#endif