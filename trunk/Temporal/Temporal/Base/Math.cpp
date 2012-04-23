#include "Math.h"
#include <algorithm>
#include <assert.h>

namespace Temporal
{
	void assertAngle(float angle, float range)
	{
		assert(angle >= -range);
		assert(angle <= range);
	}

	float mirroredAngle(float angle)
	{
		assertAngle(angle, PI);
		return PI - angle;
	}

	float minAnglesDistance(float angle1, float angle2)
	{
		assertAngle(angle1, 2 * PI);
		assertAngle(angle2, 2 * PI);
		float clockwiseDistance = abs(angle1 - angle2);
		float counterClockwiseDistance = abs(2*PI - clockwiseDistance);
		return std::min(clockwiseDistance,  counterClockwiseDistance);
	}

	bool isModerateAngle(float angle)
	{
		float absAngle = abs(angle);
		return absAngle <= ANGLE_45_IN_RADIANS || absAngle >= ANGLE_135_IN_RADIANS; 
	}

	bool isRightAngle(float angle)
	{
		return abs(angle) == ANGLE_90_IN_RADIANS;
	}
}