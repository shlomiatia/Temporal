#ifndef MATH_H
#define MATH_H

namespace Temporal
{
	static const float PI = 3.14159265358979f;

	static const float ANGLE_0_IN_RADIANS = 0.0f;
	static const float ANGLE_15_IN_RADIANS = PI / 12.0f; // 15 = 1 * PI / 12 
	static const float ANGLE_30_IN_RADIANS = PI / 6.0f; // 30 = 2 * PI / 12 = PI / 6
	static const float ANGLE_45_IN_RADIANS = PI / 4.0f; // 45 = 3 * PI / 12 = PI / 4
	static const float ANGLE_60_IN_RADIANS = PI / 3.0f; // 60 = 4 * PI / 12 = PI / 3
	static const float ANGLE_75_IN_RADIANS = 5.0f * PI / 12.0f; // 75 = 5 * PI / 12
	static const float ANGLE_90_IN_RADIANS = PI / 2.0f; // 90 = 6 * PI / 12 = PI / 2
	static const float ANGLE_105_IN_RADIANS = 7.0f * PI / 12.0f; // 105 = 7 * PI / 12
	static const float ANGLE_120_IN_RADIANS = 2.0f * PI / 3.0f; // 120 = 8 * PI / 12 = 2 * PI / 3
	static const float ANGLE_135_IN_RADIANS = 3.0f * PI / 4.0f; // 120 = 9 * PI / 12 = 3 * PI / 4
	static const float ANGLE_150_IN_RADIANS = 5.0f * PI / 6.0f; // 120 = 10 * PI / 12 = 5 * PI / 6
	static const float ANGLE_165_IN_RADIANS = 11.0f * PI / 12.0f; // 120 = 11 * PI / 12 = 2 * PI /3
	static const float ANGLE_180_IN_RADIANS = PI; // 120 = 12 * PI / 12 = PI

	inline bool sameSign(float a, float b) { return (a < 0.0f && b < 0.0f) || (a > 0.0f && b > 0.0f); }
	inline bool differentSign(float a, float b) { return (a < 0.0f && b > 0.0f) || (a > 0.0f && b < 0.0f); }
}
#endif