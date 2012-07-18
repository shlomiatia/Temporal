#include "Segment.h"
#include <math.h>
#include <assert.h>

namespace Temporal
{
	const Segment Segment::Zero(Point::Zero, Vector(Vector::Zero));

	Segment::Segment(const Point& center, const Vector& radius) 
		: _center(center), _radius(radius) 
	{
		assert(getRadiusVx() >= 0.0f);
	}

	float Segment::getLength() const
	{
		return getRadius().getLength() * 2.0f;
	}

	float Segment::get(Axis::Enum axis, float otherAxisValue) const
	{
		Vector normalizedRadius = getRadius().normalize();
		Axis::Enum otherAxis = Axis::getOpposite(axis);
		float length = (otherAxisValue - getCenter().getAxis(otherAxis)) / normalizedRadius.getAxis(otherAxis);
		float value = getCenter().getAxis(axis) + normalizedRadius.getAxis(axis) * length;
		return value;
	}
}