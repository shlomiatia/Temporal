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

	OBB::OBB(const Vector& center, float angle, const Vector& radius) : _center(center), _radius(radius) 
	{
		setAngle(angle);
	}

	OBB::OBB(const Vector& center, const Vector& axis0, const Vector& radius) : _center(center), _radius(radius) 
	{
		setAxis0(axis0);
	}

	void OBB::validate() const
	{
		assert(getRadiusX() >= 0.0f);
		assert(getRadiusY() >= 0.0f);
		assert(fabsf(_axes[0].getLength()-1.0f)<EPSILON);
	}

	void OBB::setAxis0(const Vector& axis0)
	{
		_axes[0] = axis0;
		_axes[1] = _axes[0].getLeftNormal();
		validate();
	}

	float OBB::getLeft() const { return getCenterX() - fabsf(getAxisX().getX()) * getRadiusX() - fabsf(getAxisY().getX()) * getRadiusY(); }
	float OBB::getRight() const { return getCenterX() + fabsf(getAxisX().getX()) * getRadiusX() + fabsf(getAxisY().getX()) * getRadiusY(); }
	float OBB::getBottom() const { return getCenterY() - fabsf(getAxisX().getY()) * getRadiusX() - fabsf(getAxisY().getY()) * getRadiusY(); }
	float OBB::getTop() const { return getCenterY() + fabsf(getAxisX().getY()) * getRadiusX() + fabsf(getAxisY().getY()) * getRadiusY(); }

	Segment OBB::getTopSegment(float x) const
	{
		if (getAxisX().getX() == 0.0f || getAxisX().getY() == 0.0f)
		{
			return SegmentPP(getCenter() + Vector(-getRadiusX(), getRadiusY()), getCenter() + getRadius());
		}
		Vector topPoint = getCenter() +
			(getAxisX().getY() > 0.0f ? getAxisX() : -getAxisX()) * getRadiusX() +
			(getAxisY().getY() > 0.0f ? getAxisY() : -getAxisY()) * getRadiusY();
		if (x < topPoint.getX())
		{
			Vector leftPoint = getCenter() -
				(getAxisX().getX() > 0.0f ? getAxisX() : -getAxisX()) * getRadiusX() -
				(getAxisY().getX() > 0.0f ? getAxisY() : -getAxisY()) * getRadiusY();
			return SegmentPP(leftPoint, topPoint);
		}
		else if (x > topPoint.getX())
		{
			Vector rightPoint = getCenter() +
				(getAxisX().getX() > 0.0f ? getAxisX() : -getAxisX()) * getRadiusX() +
				(getAxisY().getX() > 0.0f ? getAxisY() : -getAxisY()) * getRadiusY();
			return SegmentPP(topPoint, rightPoint);
		}
		else
		{
			return SegmentPP(topPoint, topPoint);
		}
	}

	Segment OBB::getBottomSegment(float x) const
	{
		if (getAxisX().getX() == 0.0f || getAxisX().getY() == 0.0f)
		{
			return SegmentPP(getCenter() - getRadius(), getCenter() + Vector(getRadiusX(), -getRadiusY()));
		}
		Vector bottomPoint = getCenter() +
			(getAxisX().getY() < 0.0f ? getAxisX() : -getAxisX()) * getRadiusX() +
			(getAxisY().getY() < 0.0f ? getAxisY() : -getAxisY()) * getRadiusY();
		if (x < bottomPoint.getX())
		{
			Vector leftPoint = getCenter() -
				(getAxisX().getX() < 0.0f ? getAxisX() : -getAxisX()) * getRadiusX() -
				(getAxisY().getX() < 0.0f ? getAxisY() : -getAxisY()) * getRadiusY();
			return SegmentPP(leftPoint, bottomPoint);
		}
		else if (x > bottomPoint.getX())
		{
			Vector rightPoint = getCenter() +
				(getAxisX().getX() < 0.0f ? getAxisX() : -getAxisX()) * getRadiusX() +
				(getAxisY().getX() < 0.0f ? getAxisY() : -getAxisY()) * getRadiusY();
			return SegmentPP(bottomPoint, rightPoint);
		}
		else
		{
			return SegmentPP(bottomPoint, bottomPoint);
		}
	}

	const DirectedSegment DirectedSegment::Zero(Vector::Zero, Vector::Zero);
}