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
		assert(getRadiusX() >= 0.0f);
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

	Vector OBB::getPoint(Axis::Enum axis, bool positive) const
	{
		Vector axis1 = (getAxisX().getAxis(axis) < 0.0f ^ positive) ? getAxisX() : -getAxisX();

		// Fix for AABB
		if(getAxisX().getAxis(axis) == 0.0f && axis == Axis::Y)
			axis1 = -axis1;
		Vector axis2 = (getAxisY().getAxis(axis) < 0.0f ^ positive) ? getAxisY() : -getAxisY();
		return getCenter() + axis1 * getRadiusX() + axis2 * getRadiusY(); 
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