#include "Shapes.h"
#include <assert.h>
#include <math.h>
#include "Math.h"

namespace Temporal
{
	/**********************************************************************************************
	 * Segment
	 *********************************************************************************************/
	const Segment Segment::Zero(Vector::Zero, Vector::Zero);

	Segment::Segment(const Vector& center, const Vector& radius) 
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

	/**********************************************************************************************
	 * AABB
	 *********************************************************************************************/
	const AABB AABB::Zero(Vector::Zero, Vector::Zero);

	AABB::AABB(float centerX, float centerY, float width, float height)
		: _center(centerX, centerY), _radius(width / 2.0f, height / 2.0f)
	{
		validate();
	}

	AABB::AABB(const Vector& center, const Size& size)
		: _center(center), _radius(size.toVector() / 2.0f)
	{
		validate();
	}

	AABB::AABB(const Vector& center, const Vector& radius)
		: _center(center), _radius(radius)
	{
		validate();
	}

	void AABB::validate() const
	{
		assert(getRadiusVx() >= 0);
		assert(getRadiusVy() >= 0);
	}

	/**********************************************************************************************
	 * YABP
	 *********************************************************************************************/
	const YABP YABP::Zero(Vector::Zero, Vector::Zero, 0.0f);

	YABP::YABP(const Vector& center, const Vector& slopedRadius, float yRadius)
		: _center(center), _slopedRadius(slopedRadius), _yRadius(yRadius)
	{
		validate();
	}
	
	float YABP::getTop() const { return getCenterY() + getYRadius() + abs(getSlopedRadiusVy()); }
	float YABP::getBottom() const { return getCenterY() - getYRadius() - abs(getSlopedRadiusVy()); }

	void YABP::validate() const
	{
		assert(getSlopedRadiusVx() >= 0.0f);
		assert(getYRadius() >= 0.0f);
	}
}