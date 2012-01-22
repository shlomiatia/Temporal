#pragma once

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