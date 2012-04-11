#include "Segment.h"
#include <math.h>

namespace Temporal
{
	const Segment Segment::Empty(Point::Zero, Point::Zero);

	float Segment::getLength(void) const
	{
		return sqrt(pow(getPoint2().getX() - getPoint1().getX(), 2.0f) + pow(getPoint2().getY() - getPoint1().getY(), 2.0f));
	}

	float DirectedSegment::getAngle(void) const
	{
		Vector vector = getVector();
		return vector.getAngle();
	}

	Vector DirectedSegment::getVector(void) const
	{
		return getTarget() - getOrigin();
	}
}