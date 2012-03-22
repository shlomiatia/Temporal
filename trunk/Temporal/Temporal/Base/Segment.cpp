#include "Segment.h"
#include <math.h>

namespace Temporal
{
	DirectedSegment::DirectedSegment(float originX, float originY, float targetX, float targetY)
		: _origin(originX, originY), _target(targetX, targetY), _length(calculateLength()), _normalizedVector(calculateNormalizedVector()), _angle(calculateAngle()) {}

	DirectedSegment::DirectedSegment(const Vector& origin, const Vector& target)
		: _origin(origin), _target(target), _length(calculateLength()), _normalizedVector(calculateNormalizedVector()), _angle(calculateAngle()) {}

	float DirectedSegment::calculateLength(void) const
	{
		return sqrt(pow(getTarget().getX() - getOrigin().getX(), 2.0f) + pow(getTarget().getY() - getOrigin().getY(), 2.0f));
	}

	Vector DirectedSegment::calculateNormalizedVector(void) const
	{
		Vector vector = getTarget() - getOrigin();
		return vector / getLength();
	}

	float DirectedSegment::calculateAngle(void) const
	{
		float slope = (getTarget().getY() - getOrigin().getY()) / (getTarget().getX() - getOrigin().getX());
		return atan(slope);
	}
}