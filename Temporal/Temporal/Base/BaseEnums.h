#ifndef BASEENUMS_H
#define BASEENUMS_H

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

		inline Axis::Enum opposite(Axis::Enum axis) { return axis == Axis::X ? Axis::Y : Axis::X; }
	}
	inline void operator++(Axis::Enum& value, int) { value = (Axis::Enum)(value + 1); }
}
#endif