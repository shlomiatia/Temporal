#ifndef SHAPE_H
#define SHAPE_H

#include "BaseEnums.h"

namespace Temporal
{
	namespace ShapeType
	{
		enum Enum
		{
			RECTANGLE,
			SEGMENT
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

		virtual float getCenterX(void) const { return (getLeft() + getRight()) / 2.0f; }
		virtual float getCenterY(void) const { return (getBottom() + getTop()) / 2.0f; }

		virtual float getWidth(void) const { return getRight() - getLeft(); }
		virtual float getHeight(void) const { return getTop() - getBottom(); }

		float getSide(Orientation::Enum orientation) const { return orientation == Orientation::LEFT ? getLeft() : getRight(); }
		float getOppositeSide(Orientation::Enum orientation) const { return orientation == Orientation::LEFT ? getRight() : getLeft(); }
	};
}
#endif