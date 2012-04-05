#include "Rect.h"
#include <assert.h>
#include <algorithm>

namespace Temporal
{
	const Rect Rect::Empty(Point::Zero, Vector(Vector::Zero));

	Rect::Rect(float centerX, float centerY, float width, float height)
		: _center(centerX, centerY), _radius(width / 2.0f, height / 2.0f)
	{
		validate();
	}

	Rect::Rect(const Point& center, const Size& size)
		: _center(center), _radius(size / 2.0f)
	{
		validate();
	}

	Rect::Rect(const Point& center, const Vector& radius)
		: _center(center), _radius(radius)
	{
		validate();
	}

	void Rect::validate(void) const
	{
		assert(getRadiusVx() >= 0);
		assert(getRadiusVy() >= 0);
	}

	bool Rect::intersects(const Rect& rect) const
	{
		for(Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++) 
		{
			if(abs(getCenter().getAxis(axis) - rect.getCenter().getAxis(axis)) > getRadius().getAxis(axis) + rect.getRadius().getAxis(axis)) return false;
		}
		return true;
	}

	bool Rect::intersects(const Point& point) const
	{
		for(Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++) 
		{
			if(abs(getCenter().getAxis(axis) - point.getAxis(axis)) > getRadius().getAxis(axis)) return false;
		}
		return true;
	}

	bool Rect::intersects(const Segment& segment, Vector* correction) const
	{
		Point segmentCenter = segment.getCenter();
		Vector segmentRadius = segment.getRadius();
		segmentCenter -= getCenter(); // Translate box and segment to origin
		// Try world coordinate axes as separating axes

		Vector minCorrection = Vector(1000.0f, 0.0f);
		for(Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++)
		{
			float segmentCenterAxis = segmentCenter.getAxis(axis);
			float penetration = getRadius().getAxis(axis) + segmentRadius.getAxis(axis) - abs(segmentCenterAxis);
			if (penetration < 0.0f) return false;

			// Hack to prevent plater from falling off edges
			if(penetration - minCorrection.getLength() < (axis == Axis::X ? 0.0f : 20.0f))
			{
				minCorrection = Vector::Zero;
				minCorrection.setAxis(axis, segmentCenterAxis < 0.0f ? penetration : -penetration);
			}
		}
		float sgementRadiusX = segmentRadius.getVx();
		float sgementRadiusY = segmentRadius.getVy();

		// Try cross products of segment direction vector with coordinate axes
		if (abs(segmentCenter.getX() * sgementRadiusY - segmentCenter.getY() * sgementRadiusX) >
			getRadiusVx() * abs(sgementRadiusY) + getRadiusVy() * abs(sgementRadiusX)) return false;
		// No separating axis found; segment must be overlapping AABB
		if(correction != NULL)
			*correction = minCorrection;
		return true;
	}

	bool Rect::intersects(const DirectedSegment& directedSegment, Point* pointOfIntersection) const
	{
		float tmin = 0.0f; // set to -FLT_MAX to get first hit on line
		const Point& origin = directedSegment.getOrigin();
		const Vector vector = directedSegment.getVector().normalize();
		float tmax = directedSegment.getLength();

		// For all 2 slabs
		for(Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++) 
		{
			float originAxis = origin.getAxis(axis);
			float vectorAxis = vector.getAxis(axis);
			Range rectAxis = getAxis(axis);
			if (vectorAxis == 0) 
			{
				// Ray is parallel to slab. No hit if origin not within slab
				if (originAxis < rectAxis.getMin() || originAxis > rectAxis.getMax()) 
					return false;
			} 
			else 
			{
				// Compute intersection t value of ray with near and far plane of slab
				float ood = 1.0f / vectorAxis;
				float t1 = (rectAxis.getMin() - originAxis) * ood;
				float t2 = (rectAxis.getMax() - originAxis) * ood;
				// Make t1 be intersection with near plane, t2 with far plane
				if (t1 > t2) std::swap(t1, t2);
				// Compute the intersection of slab intersection intervals
				if (t1 > tmin) tmin = t1;
				if (t2 < tmax) tmax = t2;
				// Exit with no collision as soon as slab intersection becomes empty
				if (tmin > tmax) return false;
			}
		}
		if(pointOfIntersection != NULL)
			*pointOfIntersection = origin + vector * tmin;
		return true;
	}
}