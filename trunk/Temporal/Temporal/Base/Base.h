#pragma once

#include <stdio.h> // NULL

static const float  PI = 3.14159265358979f;

static float toRadians(float angle) { return angle * PI / 180; }
static float toDegrees(float angle) { return angle * 180 / PI; }

namespace Temporal
{
	namespace Orientation
	{ 
		enum Type
		{
			LEFT = -1,
			RIGHT = 1 
		};
	}

	namespace Direction
	{
		enum Type
		{
			TOP = 1,
			BOTTOM = 2,
			FRONT = 4,
			BACK = 8
		};

		static const Direction::Type NONE = (Direction::Type)(0);
	}
	inline Direction::Type operator|(Direction::Type a, Direction::Type b) { return (Direction::Type)((int)a | (int)b); }
	inline Direction::Type operator&(Direction::Type a, Direction::Type b) { return (Direction::Type)((int)a & (int)b); }
	inline bool match(Direction::Type direction, Direction::Type positive) { return (direction & positive) == positive; }
	inline bool match(Direction::Type direction, Direction::Type positive, Direction::Type negative) { return match(direction, positive) && !(direction & negative); }
}

#include "Color.h"
#include "Vector.h"
#include "Line.h"
#include "Rect.h"
#include "Thread.h"