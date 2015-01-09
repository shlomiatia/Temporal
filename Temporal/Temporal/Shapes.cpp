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

	Segment::Segment(const Vector& center, const Vector& radius) : _center(center), _radius(radius) 
	{
		assert(getRadiusX() >= 0.0f);
	}

	Segment::Segment(float x1, float y1, float x2, float y2) : _center((x1+x2)/2.0f, (y1+y2)/2.0f), _radius((x2-x1)/2.0f, (y2-y1)/2.0f) 
	{
		
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

	AABB::AABB(const Vector& center, const Vector& radius)
		: _center(center), _radius(radius)
	{
		validate();
	}

	void AABB::validate() const
	{
		assert(getRadiusX() >= 0);
		assert(getRadiusY() >= 0);
	}

	/**********************************************************************************************
	 * OBB
	 *********************************************************************************************/
	const OBB OBB::Zero(Vector::Zero, 0.0f, Vector::Zero);

	void OBB::setAxis0(const Vector& axis0)
	{
		assert(axis0.getX() > 0.0f);
		assert(axis0.getY() >= 0.0f);
		_axes[0] = axis0;	
		_axes[1] = _axes[0].getLeftNormal();
	}

	OBBAABBWrapper OBB::getAABBWrapper()
	{
		return OBBAABBWrapper(this);
	}

	const OBBAABBWrapper OBB::getAABBWrapper() const 
	{
		return OBBAABBWrapper(const_cast<OBB*>(this));
	}
}