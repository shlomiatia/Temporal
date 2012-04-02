#include "Rect.h"
#include <assert.h>
#include <algorithm>

namespace Temporal
{
	const Rect Rect::Empty(Point::Zero, Size::Zero);

	Rect::Rect(float centerX, float centerY, float getWidth, float getHeight)
		: _center(centerX, centerY), _size(getWidth, getHeight)
	{
		validate();
	}

	Rect::Rect(const Point& center, const Size& size)
		: _center(center), _size(size)
	{
		validate();
	}

	void Rect::validate(void) const
	{
		assert(getSize().getWidth() >= 0);
		assert(getSize().getHeight() >= 0);
	}

	bool Rect::intersects(const DirectedSegment& directedSegment, Point& pointOfIntersection) const
	{
		float tmin = 0.0f; // set to -FLT_MAX to get first hit on line
		const Point& origin = directedSegment.getOrigin();
		const Vector vector = directedSegment.getVector().normalize();
		float tmax = directedSegment.getLength();

		// For all 2 slabs
		for (Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++) {
			float originAxis = origin.getAxis(axis);
			float vectorAxis = vector.getAxis(axis);
			Range rectAxis = getAxis(axis);
			if (vectorAxis == 0) {
				// Ray is parallel to slab. No hit if origin not within slab
				if (originAxis < rectAxis.getMin() || originAxis > rectAxis.getMax()) 
					return false;
			} else {
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
		pointOfIntersection = origin + vector * tmin;
		return true;
	}

	bool Rect::intersects(const Segment& segment) const
	{
		Vector rectRadius = getRadius();
		Point segmentCenter = segment.getCenter();
		Vector segmentRadius = segment.getRadius();
		segmentCenter -= getCenter(); // Translate box and segment to origin
		// Try world coordinate axes as separating axes
		float segmentCenterX = segmentCenter.getX();
		float segmentCenterY = segmentCenter.getY();
		float sgementRadiusX = segmentRadius.getVx();
		float sgementRadiusY = segmentRadius.getVy();
		float rectRadiusX = rectRadius.getVx();
		float rectRadiusY = rectRadius.getVy();

		float absSegmentRadiusX = abs(sgementRadiusX);
		if (abs(segmentCenterX) > rectRadiusX + absSegmentRadiusX) return false;
		float absSegmentRadiusY = abs(sgementRadiusY);
		if (abs(segmentCenterY) > rectRadiusY + absSegmentRadiusY) return false;

		// Try cross products of segment direction vector with coordinate axes
		if (abs(segmentCenterX * sgementRadiusY - segmentCenterY * sgementRadiusX) > rectRadiusX * absSegmentRadiusY + rectRadiusY * absSegmentRadiusX) return false;
		// No separating axis found; segment must be overlapping AABB
		return true;
	}
}