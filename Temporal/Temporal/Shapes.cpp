#include "Shapes.h"
#include <assert.h>
#include <math.h>

namespace Temporal
{
	/**********************************************************************************************
	 * AABB
	 *********************************************************************************************/
	const AABB AABB::Zero(Point::Zero, Vector(Vector::Zero));

	AABB::AABB(float centerX, float centerY, float width, float height)
		: Shape(centerX, centerY), _radius(width / 2.0f, height / 2.0f)
	{
		validate();
	}

	AABB::AABB(const Point& center, const Size& size)
		: Shape(center), _radius(size / 2.0f)
	{
		validate();
	}

	AABB::AABB(const Point& center, const Vector& radius)
		: Shape(center), _radius(radius)
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

	/**********************************************************************************************
	 * Segment
	 *********************************************************************************************/
	const Segment Segment::Zero(Point::Zero, Vector(Vector::Zero));

	Segment::Segment(const Point& center, const Vector& radius) 
		: Shape(center), _radius(radius) 
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

	/**********************************************************************************************
	 * YABP
	 *********************************************************************************************/
	YABP::YABP(const Point& center, const Vector& slopedRadius, float yRadius)
		: _center(center), _slopedRadius(slopedRadius), _yRadius(yRadius)
	{
		assert(getSlopedRadiusVx() >= 0.0f);
		assert(getYRadius() >= 0.0f);
	}

	float YABP::getTop() const { return getCenterY() + getYRadius() + abs(getSlopedRadiusVy()); }
	float YABP::getBottom() const { return getCenterY() - getYRadius() - abs(getSlopedRadiusVy()); }
}