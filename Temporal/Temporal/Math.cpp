#include "Math.h"
#include <algorithm>
#include <assert.h>

namespace Temporal
{
	const float EPSILON = std::numeric_limits<float>::epsilon() * 768.0f;

	const float RadianAngleUtils::ANGLE_0_IN_RADIANS = 0.0f;
	const float RadianAngleUtils::ANGLE_15_IN_RADIANS = PI / 12.0f; // 15 = 1 * PI / 12 
	const float RadianAngleUtils::ANGLE_30_IN_RADIANS = PI / 6.0f; // 30 = 2 * PI / 12 = PI / 6
	const float RadianAngleUtils::ANGLE_45_IN_RADIANS = PI / 4.0f; // 45 = 3 * PI / 12 = PI / 4
	const float RadianAngleUtils::ANGLE_60_IN_RADIANS = PI / 3.0f; // 60 = 4 * PI / 12 = PI / 3
	const float RadianAngleUtils::ANGLE_75_IN_RADIANS = 5.0f * PI / 12.0f; // 75 = 5 * PI / 12
	const float RadianAngleUtils::ANGLE_90_IN_RADIANS = PI / 2.0f; // 90 = 6 * PI / 12 = PI / 2
	const float RadianAngleUtils::ANGLE_105_IN_RADIANS = 7.0f * PI / 12.0f; // 105 = 7 * PI / 12
	const float RadianAngleUtils::ANGLE_120_IN_RADIANS = 2.0f * PI / 3.0f; // 120 = 8 * PI / 12 = 2 * PI / 3
	const float RadianAngleUtils::ANGLE_135_IN_RADIANS = 3.0f * PI / 4.0f; // 135 = 9 * PI / 12 = 3 * PI / 4
	const float RadianAngleUtils::ANGLE_150_IN_RADIANS = 5.0f * PI / 6.0f; // 150 = 10 * PI / 12 = 5 * PI / 6
	const float RadianAngleUtils::ANGLE_165_IN_RADIANS = 11.0f * PI / 12.0f; // 165 = 11 * PI / 12 = 2 * PI /3
	const float RadianAngleUtils::ANGLE_180_IN_RADIANS = PI; // 180 = 12 * PI / 12 = PI
	const float RadianAngleUtils::ANGLE_360_IN_RADIANS = 2 * PI; // 2 * PI

	bool equals(float a, float b)
	{
		bool bb = fabsf(a - b) < EPSILON; 
		return bb;
	}

	void assertAngle(float angle, float range)
	{
		assert(angle >= -range);
		assert(angle <= range);
	}

	float BaseAngleUtils::mirror(float angle) const
	{
		assertAngle(angle, STRAIGHT);
		return STRAIGHT - angle;
	}

	float BaseAngleUtils::opposite(float angle) const
	{
		assertAngle(angle, STRAIGHT);
		return -angle; 
	}

	float BaseAngleUtils::turn(float angle) const
	{
		assertAngle(angle, STRAIGHT);
		return angle - (angle > 0.0f ? PERIGON : -PERIGON);
	}

	float BaseAngleUtils::normalize(float angle) const
	{
		angle = fmodf(angle, PERIGON);
		if(fabsf(angle) > STRAIGHT)
			return angle - (angle > 0.0f ? PERIGON : -PERIGON);
		else
			return angle;
	}

	bool BaseAngleUtils::isAcute(float angle) const
	{
		assertAngle(angle, STRAIGHT);
		return angle > 0.0f && angle < RIGHT / 2.0f;
	}

	bool BaseAngleUtils::isRight(float angle) const
	{
		assertAngle(angle, STRAIGHT);
		return angle == RIGHT;
	}

	bool BaseAngleUtils::isObtuse(float angle) const
	{
		assertAngle(angle, STRAIGHT);
		return angle > RIGHT && angle < STRAIGHT;
	}

	bool BaseAngleUtils::isStraight(float angle) const
	{
		assertAngle(angle, STRAIGHT);
		return angle == STRAIGHT;
	}

	bool BaseAngleUtils::isReflex(float angle) const
	{
		assertAngle(angle, STRAIGHT);
		return angle < 0.0f;
	}

	bool BaseAngleUtils::isPerigon(float angle) const
	{
		assertAngle(angle, STRAIGHT);
		return angle == 0.0f;
	}

	float BaseAngleUtils::clockwiseDistance(float start, float end) const
	{
		assertAngle(start, STRAIGHT);
		assertAngle(end, STRAIGHT);
		return start > end ? start - end : PERIGON + start - end;;
	}

	float BaseAngleUtils::counterclockwiseDistance(float start, float end) const
	{
		return PERIGON - clockwiseDistance(start, end);;
	}

	float BaseAngleUtils::minDistance(float angle1, float angle2) const
	{
		float distance1 = clockwiseDistance(angle1, angle2);
		float distance2 = counterclockwiseDistance(angle1, angle2);
		return std::min(distance1,  distance2);
	}
	
	bool BaseAngleUtils::isVertical(float angle) const
	{
		return isRight(fabsf(angle));
	}

	bool BaseAngleUtils::isHorizontal(float angle) const
	{
		return isPerigon(angle) || isStraight(fabsf(angle));
	}

	bool BaseAngleUtils::isModerate(float angle) const
	{
		assertAngle(angle, STRAIGHT);
		float absAngle = fabsf(angle);
		return absAngle < RIGHT / 2.0f || absAngle > STRAIGHT - RIGHT / 2.0f; 
	}
}