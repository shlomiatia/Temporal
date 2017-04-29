#include "Math.h"
#include <algorithm>
#include <assert.h>

namespace Temporal
{
	const float EPSILON = std::numeric_limits<float>::epsilon() * 10240.0f;

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
		return angle > 0.0f ? STRAIGHT - angle : -STRAIGHT - angle;
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