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

	namespace Axis
	{
		enum Enum
		{
			UNKNOWN,
			X,
			Y
		};
	}
	inline void operator++(Axis::Enum& value, int) { value = (Axis::Enum)(value + 1); }
}
#endif