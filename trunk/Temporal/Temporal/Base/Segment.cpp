#include "Segment.h"
#include <math.h>

namespace Temporal
{
	float Segment::getLength(void) const
	{
		return sqrt(pow(getPoint2().getX() - getPoint1().getX(), 2.0f) + pow(getPoint2().getY() - getPoint1().getY(), 2.0f));
	}

	float DirectedSegment::getAngle(void) const
	{
		Vector vector = getVector();
		return atan2(vector.getVy(), vector.getVx());
	}

	Vector DirectedSegment::getVector(void) const
	{
		return getTarget() - getOrigin();
	}
}