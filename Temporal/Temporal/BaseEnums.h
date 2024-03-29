#ifndef BASEENUMS_H
#define BASEENUMS_H

namespace Temporal
{
	namespace Side
	{ 
		enum Enum
		{
			LEFT = -1,
			RIGHT = 1 
		};
		inline Side::Enum getOpposite(Side::Enum value) { return value == LEFT ? RIGHT : LEFT; }
		inline Side::Enum get(float value) { return value > 0.0f ? RIGHT : LEFT; }
	}

	namespace Axis
	{
		enum Enum
		{
			X,
			Y
		};

		inline Axis::Enum getOpposite(Axis::Enum axis) { return axis == Axis::X ? Axis::Y : Axis::X; }
	}
	inline void operator++(Axis::Enum& value, int) { value = static_cast<Axis::Enum>(value + 1); }
	inline void operator++(Axis::Enum& value) { value = static_cast<Axis::Enum>(value + 1); }
	inline void operator--(Axis::Enum& value, int) { value = static_cast<Axis::Enum>(value - 1); }
	inline void operator--(Axis::Enum& value) { value = static_cast<Axis::Enum>(value - 1); }
}
#endif