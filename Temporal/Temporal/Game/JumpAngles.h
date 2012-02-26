#pragma once

namespace Temporal
{
	static const float PI = 3.14159265358979f;

	inline float toRadians(float angle) { return angle * PI / 180; }
	inline float toDegrees(float angle) { return angle * 180 / PI; }

	static const float DEGREES_45 = toRadians(45.0f);
	static const float DEGREES_60 = toRadians(60.0f);
	static const float DEGREES_75 = toRadians(75.0f);
	static const float DEGREES_90 = toRadians(90.0f);
	static const float DEGREES_105 = toRadians(105.0);

	static const float JUMP_ANGLES_SIZE = 4;
	static const float JUMP_ANGLES[] = { DEGREES_45, DEGREES_60, DEGREES_75, DEGREES_105 };
}