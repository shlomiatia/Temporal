#include "Segment.h"
#include <math.h>

namespace Temporal
{
	const Segment Segment::Empty(Point::Zero, Vector(Vector::Zero));
	const DirectedSegment DirectedSegment::Empty(Point::Zero, Point(Vector::Zero));

	float Segment::getLength(void) const
	{
		return getRadius().getLength() * 2.0f;
	}

	float Segment::get(Axis::Enum axis, float otherAxisValue) const
	{
		Vector normalizedRadius = getRadius().normalize();
		Axis::Enum otherAxis = Axis::opposite(axis);
		float length = (otherAxisValue - getCenter().getAxis(otherAxis)) / normalizedRadius.getAxis(otherAxis);
		float value = getCenter().getAxis(axis) + normalizedRadius.getAxis(axis) * length;
		return value;
	}

	float DirectedSegment::getAngle(void) const
	{
		Vector vector = getVector();
		return vector.getAngle();
	}

	Vector DirectedSegment::getVector(void) const
	{
		return getRadius() * 2.0f;
	}
}