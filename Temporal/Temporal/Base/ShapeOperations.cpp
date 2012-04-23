#include "ShapeOperations.h"

#include "AABB.h"
#include "Segment.h"
#include "Math.h"
#include <math.h>
#include <algorithm>

namespace Temporal
{
	bool intersects(const AABB& rect1, const AABB& rect2, Vector* correction)
	{
		bool isFirst = true;
		Vector minCorrection = Vector::Zero;
		for(Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++) 
		{
			float delta = rect2.getCenter().getAxis(axis) - rect1.getCenter().getAxis(axis);
			float penetration = rect1.getRadius().getAxis(axis) + rect2.getRadius().getAxis(axis) - abs(delta);
			if(penetration < 0.0f) return false;
			if(isFirst || penetration < minCorrection.getLength())
			{
				isFirst = false;
				minCorrection = Vector::Zero;
				minCorrection.setAxis(axis, delta < 0.0f ? penetration : -penetration);
			}
		}
		if(correction != NULL)
			*correction = minCorrection;
		return true;
	}

	bool intersects(const AABB& rect, const Segment& seg, Vector* correction)
	{
		Point segmentCenter = seg.getCenter();
		Vector segmentRadius = seg.getRadius();
		segmentCenter -= rect.getCenter(); // Translate box and segment to origin
		// Try world coordinate axes as separating axes

		bool isFirst = true;
		Vector minCorrection = Vector::Zero;
		for(Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++)
		{
			float segmentCenterAxis = segmentCenter.getAxis(axis);
			float penetration = rect.getRadius().getAxis(axis) + segmentRadius.getAxis(axis) - abs(segmentCenterAxis);
			if (penetration < 0.0f) return false;

			if(isFirst || penetration < minCorrection.getLength())
			{
				isFirst = false;
				minCorrection = Vector::Zero;
				minCorrection.setAxis(axis, segmentCenterAxis < 0.0f ? penetration : -penetration);
			}
		}

		float y = seg.getY(rect.getCenterX());
		Vector vector = seg.getNaturalVector().normalize();
		Vector normal = y > rect.getCenterY() ? vector.getLeftNormal() : vector.getRightNormal();

		float absSegCenterProjection = abs(Vector(segmentCenter) * normal);

		// To find max projection of rectangle unto the segment vector, we need to test -rx,-ry;-rx,ry;rx,-ry;rx,ry multiply nx*ny. 
		// The max projection is obtained when all elements are positive, so we multiply rx,ry to abs(nx),abs(ny)
		Vector absNormal = Vector(abs(normal.getVx()), abs(normal.getVy()));
		float absRectRadiusProjection = rect.getRadius() * absNormal;
		float penetration = absRectRadiusProjection - absSegCenterProjection;
		if(penetration < -EPSILON) return false;
		if(penetration < minCorrection.getLength())
			minCorrection = penetration * normal;

		// No separating axis found; segment must be overlapping AABB
		if(correction != NULL)
			*correction = minCorrection;
		return true;
	}

	bool intersects(const DirectedSegment& seg, const AABB& rect, Point* pointOfIntersection)
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
			if (abs(vectorAxis) < EPSILON) 
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


	bool intersects(const DirectedSegment& dirSeg, const Segment& seg, Point* pointOfIntersection)
	{
		Vector dirSegVec = dirSeg.getVector();
		Vector segVec = seg.getPoint2() - seg.getPoint1();
		Vector segNormal = segVec.getLeftNormal();
		float denominator = segNormal * dirSegVec;
		Vector difference = seg.getPoint1() - dirSeg.getOrigin();
		Vector dirSegNormal = dirSegVec.getLeftNormal();
		float numerator2 = (dirSegNormal * difference);
		if(denominator == 0)
		{
			if(numerator2 == 0 && abs(dirSeg.getCenterX() - seg.getCenterX()) <= dirSeg.getRadius().getVx() + seg.getRadius().getVx())
			{
				if(pointOfIntersection != NULL)
				{
					Vector vector1 = seg.getPoint1() - dirSeg.getOrigin();
					Vector vector2 = seg.getPoint2() - dirSeg.getOrigin();
					if(differentSign(vector1.getVx(), vector2.getVx()) || differentSign(vector1.getVy(), vector2.getVy()))
					{
						*pointOfIntersection = dirSeg.getOrigin();
					}
					else
					{
						*pointOfIntersection = vector1.getLength() < vector2.getLength() ? seg.getPoint1() : seg.getPoint2();
					}
				}
				return true;
			}
		}
		else
		{
			float length1 = (segNormal * difference) / denominator;
			float length2 = numerator2 / denominator;
			if(length1 >= 0.0f && length1 <= 1.0f && length2 >= 0.0f && length2 <= 1.0f)
			{
				if(pointOfIntersection != NULL)
					*pointOfIntersection = dirSeg.getOrigin() + length1 * dirSegVec;
				return true;
			}
		}
		return false;
	}

	bool intersects(const Shape& shape1, const Shape& shape2, Vector* correction)
	{
		if(shape1.getType() == ShapeType::AABB)
		{
			const AABB& rect1 = (const AABB&)shape1;
			if(shape2.getType() == ShapeType::AABB)
			{
				const AABB& rect2 = (const AABB&)shape2;
				return intersects(rect1, rect2, correction);
			}
			else if(shape2.getType() == ShapeType::SEGMENT)
			{
				const Segment& seg2 = (const Segment&)shape2;
				return intersects(rect1, seg2, correction);
			}
			else
			{
				// ERROR
				exit(1);
			}
		}
		else if(shape2.getType() == ShapeType::SEGMENT)
		{
			const Segment& seg1 = (const Segment&)shape1;
			if(shape2.getType() == ShapeType::AABB)
			{
				const AABB& rect2 = (const AABB&)shape2;
				return intersects(rect2, seg1, correction);
			}
			else if(shape2.getType() == ShapeType::SEGMENT)
			{
				const Segment& seg2 = (const Segment&)shape2;
				return intersects(DirectedSegment(seg1.getPoint1(), seg1.getPoint2()), seg2);
			}
			else
			{
				// ERROR
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
		if(shape.getType() == ShapeType::AABB)
		{
			const AABB& rect = (const AABB&)shape;
			return intersects(seg, rect, pointOfIntersection);
		}
		else if(shape.getType() == ShapeType::SEGMENT)
		{
			const Segment& seg2 = (const Segment&)shape;
			return intersects(seg, seg2, pointOfIntersection);
		}
		else
		{
			// ERROR
			exit(1);
		}
	}
}