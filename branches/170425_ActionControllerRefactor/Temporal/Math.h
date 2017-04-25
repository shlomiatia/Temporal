#ifndef MATH_H
#define MATH_H

namespace Temporal
{
	extern const float EPSILON;
	static const float PI = 3.14159265358979f;

	inline bool sameSign(float a, float b) { return (a < -0.0f && b < -0.0f) || (a > 0.0f && b > 0.0f); }
	inline bool differentSign(float a, float b) { return (a < -0.0f && b > 0.0f) || (a > 0.0f && b < -0.0f); }
	bool equals(float a, float b);

	class BaseAngleUtils
	{
	public:
		BaseAngleUtils(float perigon) : PERIGON(perigon), STRAIGHT(perigon / 2.0f), RIGHT(perigon / 4.0f) {}

		// abs(angle) <= 180,PI
		float mirror(float angle) const;
		float opposite(float angle) const;
		float turn(float angle) const; // Output: abs(angle) <= 360,2PI
		float normalize(float angle) const; // Any input

		bool isAcute(float angle) const;
		bool isRight(float angle) const;
		bool isObtuse(float angle) const;
		bool isStraight(float angle) const;
		bool isReflex(float angle) const;
		bool isPerigon(float angle) const;

		float clockwiseDistance(float start, float end) const;
		float counterclockwiseDistance(float start, float end) const;
		float minDistance(float angle1, float angle2) const;
		
		bool isVertical(float angle) const;
		bool isHorizontal(float angle) const;
		bool isModerate(float angle) const;
		bool isSteep(float angle) const { return !isModerate(angle) && !isVertical(angle); }

	private:
		float RIGHT;
		float STRAIGHT;
		float PERIGON;
	};

	class RadianAngleUtils : public BaseAngleUtils
	{
	public:
		const float ANGLE_0_IN_RADIANS;
		const float ANGLE_15_IN_RADIANS;
		const float ANGLE_30_IN_RADIANS;
		const float ANGLE_45_IN_RADIANS;
		const float ANGLE_60_IN_RADIANS;
		const float ANGLE_75_IN_RADIANS;
		const float ANGLE_90_IN_RADIANS;
		const float ANGLE_105_IN_RADIANS;
		const float ANGLE_120_IN_RADIANS;
		const float ANGLE_135_IN_RADIANS;
		const float ANGLE_150_IN_RADIANS;
		const float ANGLE_165_IN_RADIANS;
		const float ANGLE_180_IN_RADIANS;
		const float ANGLE_360_IN_RADIANS;

		RadianAngleUtils() : 
			ANGLE_0_IN_RADIANS(0.0f),
			ANGLE_15_IN_RADIANS(PI / 12.0f), // 15 = 1 * PI / 12
			ANGLE_30_IN_RADIANS(PI / 6.0f), // 30 = 2 * PI / 12 = PI / 6
			ANGLE_45_IN_RADIANS(PI / 4.0f), // 45 = 3 * PI / 12 = PI / 4
			ANGLE_60_IN_RADIANS(PI / 3.0f), // 60 = 4 * PI / 12 = PI / 3
			ANGLE_75_IN_RADIANS(5.0f * PI / 12.0f), // 75 = 5 * PI / 12
			ANGLE_90_IN_RADIANS(PI / 2.0f), // 90 = 6 * PI / 12 = PI / 2
			ANGLE_105_IN_RADIANS(7.0f * PI / 12.0f), // 105 = 7 * PI / 12
			ANGLE_120_IN_RADIANS(2.0f * PI / 3.0f), // 120 = 8 * PI / 12 = 2 * PI / 3
			ANGLE_135_IN_RADIANS(3.0f * PI / 4.0f), // 135 = 9 * PI / 12 = 3 * PI / 4
			ANGLE_150_IN_RADIANS(5.0f * PI / 6.0f), // 150 = 10 * PI / 12 = 5 * PI / 6
			ANGLE_165_IN_RADIANS(11.0f * PI / 12.0f), // 165 = 11 * PI / 12 = 2 * PI /3
			ANGLE_180_IN_RADIANS(PI), // 180 = 12 * PI / 12 = PI
			ANGLE_360_IN_RADIANS(2 * PI), // 2 * PI
			BaseAngleUtils(2 * PI) {}
	};

	class DegreeAngleUtils : public BaseAngleUtils
	{
	public:
		DegreeAngleUtils() : BaseAngleUtils(360.0f) {};
	};

	class AngleUtils
	{
	public:
		static const RadianAngleUtils& radian() { static RadianAngleUtils radian; return radian; }
		static const DegreeAngleUtils& degree() { static DegreeAngleUtils degree; return degree; }

		static float radiansToDegrees(float angle) { return angle * 180.f / PI; }
		static float degreesToRadians(float angle) { return angle * PI / 180.0f; }
	};
}
#endif