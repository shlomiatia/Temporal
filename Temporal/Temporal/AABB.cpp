#include "AABB.h"
#include <assert.h>
#include <math.h>

namespace Temporal
{
	const AABB AABB::Zero(Point::Zero, Vector(Vector::Zero));

	AABB::AABB(float centerX, float centerY, float width, float height)
		: _center(centerX, centerY), _radius(width / 2.0f, height / 2.0f)
	{
		validate();
	}

	AABB::AABB(const Point& center, const Size& size)
		: _center(center), _radius(size / 2.0f)
	{
		validate();
	}

	AABB::AABB(const Point& center, const Vector& radius)
		: _center(center), _radius(radius)
	{
		validate();
	}

	void AABB::validate() const
	{
		assert(getRadiusVx() >= 0);
		assert(getRadiusVy() >= 0);
	}

	bool AABB::contains(const Point& point) const
	{
		for(Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++) 
		{
			if(abs(getCenter().getAxis(axis) - point.getAxis(axis)) > getRadius().getAxis(axis)) return false;
		}
		return true;
	}
}