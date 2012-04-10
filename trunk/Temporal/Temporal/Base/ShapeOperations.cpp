#include "ShapeOperations.h"

#include "Rectangle.h"
#include "Segment.h"
#include <math.h>
#include <algorithm>

namespace Temporal
{
	bool intersects(const Rectangle& rect1, const Rectangle& rect2, Vector* correction)
	{
		Vector minCorrection = Vector(1000.0f, 0.0f);
		for(Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++) 
		{
			float delta = rect2.getCenter().getAxis(axis) - rect1.getCenter().getAxis(axis);
			float penetration = rect1.getRadius().getAxis(axis) + rect2.getRadius().getAxis(axis) - abs(delta);
			if(penetration < 0.0f) return false;
			if(penetration < minCorrection.getLength())
			{
				minCorrection = Vector::Zero;
				minCorrection.setAxis(axis, delta < 0.0f ? penetration : -penetration);
			}
		}
		if(correction != NULL)
			*correction = minCorrection;
		return true;
	}

	bool intersects(const Rectangle& rect, const Segment& seg, Vector* correction)
	{
		Point segmentCenter = seg.getCenter();
		Vector segmentRadius = seg.getRadius();
		segmentCenter -= rect.getCenter(); // Translate box and segment to origin
		// Try world coordinate axes as separating axes

		Vector minCorrection = Vector(1000.0f, 0.0f);
		for(Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++)
		{
			float segmentCenterAxis = segmentCenter.getAxis(axis);
			float penetration = rect.getRadius().getAxis(axis) + segmentRadius.getAxis(axis) - abs(segmentCenterAxis);
			if (penetration < 0.0f) return false;

			if(penetration < minCorrection.getLength())
			{
				minCorrection = Vector::Zero;
				minCorrection.setAxis(axis, segmentCenterAxis < 0.0f ? penetration : -penetration);
			}
		}

		Vector vector = seg.getPoint2() - seg.getPoint1();
		Vector normal = Vector(vector.getVy(), -vector.getVx()) / vector.getLength();

		float point = Vector(seg.getCenter()) * normal;
		float val = Vector(rect.getLeft(), rect.getBottom()) * normal;
		float min = val;
		float max = val;
		val = Vector(rect.getLeft(), rect.getTop()) * normal;
		min = std::min(min, val);
		max = std::max(max, val);
		val = Vector(rect.getRight(), rect.getBottom()) * normal;
		min = std::min(min, val);
		max = std::max(max, val);
		val = Vector(rect.getRight(), rect.getTop()) * normal;
		min = std::min(min, val);
		max = std::max(max, val);
		if(min > point || max < point) return false;
		float penetration1 = max - point;
		float penetration2 = point - min;
		float minPenetration = std::min(penetration1, penetration2);
		if(minPenetration < minCorrection.getLength())
			minCorrection = -(minPenetration * normal);
		
		// No separating axis found; segment must be overlapping AABB
		if(correction != NULL)
			*correction = minCorrection;
		return true;
	}

	bool intersects(const DirectedSegment& seg, const Rectangle& rect, Point* pointOfIntersection)
	{
		float tmin = 0.0f; // set to -FLT_MAX to get first hit on line
		const Point& origin = seg.getOrigin();
		const Vector vector = seg.getVector().normalize();
		float tmax = seg.getLength();

		// For all 2 slabs
		for(Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++) 
		{
			float originAxis = origin.getAxis(axis);
			float vectorAxis = vector.getAxis(axis);
			Range rectAxis = rect.getAxis(axis);
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

		// Returns 2 times the signed triangle area. The result is positive if
	// abc is ccw, negative if abc is cw, zero if abc is degenerate.
	float signed2DTriArea(Point a, Point b, Point c)
	{
		return (a.getX() - c.getX()) * (b.getY() - c.getY()) - (a.getY() - c.getY()) * (b.getX() - c.getX());
	}

	bool intersects(const DirectedSegment& dirSeg, const Segment& seg, Point* pointOfIntersection)
	{
		// Sign of areas correspond to which side of ab points c and d are
		float a1 = signed2DTriArea(dirSeg.getPoint1(), dirSeg.getPoint2(), seg.getPoint2()); // Compute winding of abd (+ or -)
		float a2 = signed2DTriArea(dirSeg.getPoint1(), dirSeg.getPoint2(), seg.getPoint1()); // To intersect, must have sign opposite of a1
		// If c and d are on different sides of ab, areas have different signs
		if (a1 * a2 < 0.0f) 
		{
			// Compute signs for a and b with respect to segment cd
			float a3 = signed2DTriArea(seg.getPoint1(), seg.getPoint2(), dirSeg.getPoint1()); // Compute winding of cda (+ or -)
			// Since area isant a1 - a2 = a3 - a4, or a4 = a3 + a2 - a1
			// float a4 = Signed2DTriArea(c, d, b); // Must have opposite sign of a3
			float a4 = a3 + a2 - a1;
			// Points a and b on different sides of cd if areas have different signs
			if (a3 * a4 < 0.0f) 
			{
				// Segments intersect. Find intersection point along L(t) = a + t * (b - a).
				// Given height h1 of an over cd and height h2 of b over cd,
				// t = h1 / (h1 - h2) = (b*h1/2) / (b*h1/2 - b*h2/2) = a3 / (a3 - a4),
				// where b (the base of the triangles cda and cdb, i.e., the length
				// of cd) cancels out.
				float t = a3 / (a3 - a4);
				if(pointOfIntersection != NULL)
					*pointOfIntersection = dirSeg.getPoint1() + t * (dirSeg.getPoint2() - dirSeg.getPoint1());
				return true;
			}
		}
		return false;
	}

	bool intersects(const Shape& shape1, const Shape& shape2, Vector* correction)
	{
		if(shape1.getType() == ShapeType::RECTANGLE)
		{
			const Rectangle& rect1 = (const Rectangle&)shape1;
			if(shape2.getType() == ShapeType::RECTANGLE)
			{
				const Rectangle& rect2 = (const Rectangle&)shape2;
				return intersects(rect1, rect2, correction);
			}
			else if(shape2.getType() == ShapeType::SEGMENT)
			{
				const Segment& seg2 = (const Segment&)shape2;
				return intersects(rect1, seg2, correction);
			}
			else
			{
				exit(1);
			}
		}
		else
		{
			exit(1);
		}
	}

	bool intersects(const DirectedSegment& seg, const Shape& shape, Point* pointOfIntersection)
	{
		if(shape.getType() == ShapeType::RECTANGLE)
		{
			const Rectangle& rect = (const Rectangle&)shape;
			return intersects(seg, rect, pointOfIntersection);
		}
		else if(shape.getType() == ShapeType::SEGMENT)
		{
			const Segment& seg2 = (const Segment&)shape;
			return intersects(seg, seg2, pointOfIntersection);
		}
		else
		{
			exit(1);
		}
	}
}