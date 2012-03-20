#ifndef BASE_ENUMS_H
#define BASE_ENUMS_H

namespace Temporal
{
	namespace Orientation
	{ 
		enum Enum
		{
			NONE = 0,
			LEFT = -1,
			RIGHT = 1 
		};
		inline Orientation::Enum getOpposite(Orientation::Enum value) { return value == LEFT ? RIGHT : LEFT; }
	}

	namespace Direction
	{
		enum Enum
		{
			NONE = 0,
			TOP = 1,
			BOTTOM = 2,
			FRONT = 4,
			BACK = 8,
			ALL = 15,
		};
		inline Direction::Enum operator|(Direction::Enum a, Direction::Enum b) { return (Direction::Enum)((int)a | (int)b); }
		inline Direction::Enum operator&(Direction::Enum a, Direction::Enum b) { return (Direction::Enum)((int)a & (int)b); }
		inline bool match(Direction::Enum direction, Direction::Enum positive) { return positive == Direction::ALL || (direction & positive) == positive; }
		inline bool match(Direction::Enum direction, Direction::Enum positive, Direction::Enum negative) { return match(direction, positive) && !(direction & negative); }
	}	
}
#endif