#include "ShapeOperations.h"

#include "Shapes.h"
#include "Math.h"
#include <math.h>
#include <algorithm>

namespace Temporal
{
	bool intersects(const AABB& rect1, const AABB& rect2, Vector* correction)
	{
		bool isFirst = true;
		Vector minCorrection = Vector::Zero;

		// Check x and y axis
		for(Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++) 
		{
			float delta = rect2.getCenter().getAxis(axis) - rect1.getCenter().getAxis(axis);
			float penetration = rect1.getRadius().getAxis(axis) + rect2.getRadius().getAxis(axis) - abs(delta);
			if(penetration < 0.0f) return false;

			// Set min correction if first or smaller
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
		Vector segmentCenter = seg.getCenter();
		const Vector& segmentRadius = seg.getRadius();
		segmentCenter -= rect.getCenter(); // Translate box and segment to origin
		
		bool isFirst = true;
		Vector minCorrection = Vector::Zero;

		// Try world coordinate axes as separating axes
		for(Axis::Enum axis = Axis::X; axis <= Axis::Y; axis++)
		{
			float segmentCenterAxis = segmentCenter.getAxis(axis);
			float penetration = rect.getRadius().getAxis(axis) + abs(segmentRadius.getAxis(axis)) - abs(segmentCenterAxis);
			if (penetration < 0.0f) return false;

			// Set min correction if first or smaller
			if(isFirst || penetration < minCorrection.getLength())
			{
				isFirst = false;
				minCorrection = Vector::Zero;
				minCorrection.setAxis(axis, segmentCenterAxis < 0.0f ? penetration : -penetration);
			}
		}

		// To find max projection of rectangle unto the segment vector, we need to test +/-rx,+/-ry multiply nx*ny. 
		// The max projection is obtained when all elements are positive, so we multiply rx,ry to abs(nx),abs(ny)
		float y = seg.getY(rect.getCenterX());
		Vector vector = seg.getNaturalVector().normalize();
		Vector normal = y > rect.getCenterY() ? vector.getRightNormal() : vector.getLeftNormal();
		float absSegCenterProjection = abs(Vector(segmentCenter) * normal);

		Vector absNormal = normal.absolute();
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

	bool intersects(const DirectedSegment& seg, const AABB& rect, Vector* pointOfIntersection, float* distance)
	{
		float tmin = 0.0f; // set to -FLT_MAX to get first hit on line
		const Vector& origin = seg.getOrigin();
		Vector vector = seg.getVector();
		const Vector direction = vector.normalize();
		float tmax = vector.getLength();

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
		if(distance != NULL)
			*distance = tmin;
		return true;
	}


	bool intersects(const DirectedSegment& dirSeg, const Segment& seg, Vector* pointOfIntersection, float* distance)
	{
		Vector dirSegVec = dirSeg.getVector();
		Vector segVec = seg.getNaturalVector();
		Vector segNormal = segVec.getRightNormal();
		float denominator = segNormal * dirSegVec;
		Vector segOrigin = seg.getNaturalOrigin();
		Vector dirSegOrigin = dirSeg.getOrigin();
		Vector difference = segOrigin - dirSegOrigin;
		Vector dirSegNormal = dirSegVec.getRightNormal();
		float numerator1 = dirSegNormal * difference;

		// Not parallel
		if(denominator != 0)
		{
			float numerator2 = segNormal * difference;
			float length1 = numerator1 / denominator;
			float length2 = numerator2 / denominator;
			if(length1 >= 0.0f && length1 <= 1.0f && length2 >= 0.0f && length2 <= 1.0f)
			{
				Vector ray = length2 * dirSegVec;
				if(pointOfIntersection != NULL)
					*pointOfIntersection = dirSegOrigin + ray;
				if(distance != NULL)
					*distance = ray.getLength();
				return true;
			}
		}
		// Parallel
		else
		{
			// Overlaps
			if(numerator1 == 0)
			{
				Vector dirSegCenter = dirSeg.getOrigin() + dirSeg.getVector() / 2.0f;
				Vector centersVector = Vector(dirSegCenter - seg.getCenterX());
				if(centersVector.getLength() <= dirSeg.getVector().getLength() + seg.getRadius().getLength())
				{
					if(pointOfIntersection != NULL)
					{
						Vector vector1 = segOrigin - dirSegOrigin;
						Vector segTarget = seg.getNaturalTarget();
						Vector vector2 = segTarget - dirSegOrigin;
						float tempDistance;

						// We take 2 vectors that originated in the directed segment origin, and are directed to the segment natural origin and target.
						// If the directions are opposite, it means that the directed segment origin is swallowed by the segment, therefore it's the point of intersection
						if(differentSign(vector1.getX(), vector2.getX()) || differentSign(vector1.getY(), vector2.getY()))
						{
							*pointOfIntersection = dirSeg.getOrigin();
							tempDistance = 0.0f;
						}
						// Otherwise, the shorter vector point to the point of intersection
						else 
						{
							float length1 = vector1.getLength();
							float length2 = vector2.getLength();
							if(length1 < length2)
							{
								*pointOfIntersection = segOrigin;
								tempDistance = length1;
							}
							else
							{
								*pointOfIntersection = segTarget;
								tempDistance = length2;
							}
						}
						
						if(distance != NULL)
							*distance = tempDistance;
					}
					return true;
				}
			}
		}
		return false;
	}

	bool intersects(const YABP& yabp, const Segment& segment)
	{
		// Check x axis (parallel to y)
		Vector segRadius = segment.getRadius();
		float delta = yabp.getCenterX() - segment.getCenterX();
		if(yabp.getSlopedRadiusVx() + segRadius.getX() < abs(delta)) return false;

		// Check sloped axis
		Vector normal = yabp.getSlopedRadius().normalize().getRightNormal();
		Vector yRadius = yabp.getYVector();
		Vector yabpPointMin = yabp.getCenter() + yRadius;
		Vector yabpPointMax = yabp.getCenter() - yRadius;
		float yabpProjectionMin =   normal * yabpPointMin;
		float yabpProjectionMax =   normal * yabpPointMax;
		float segmentProjection1 = normal * segment.getLeftPoint();
		float segmentProjection2 = normal * segment.getRightPoint();
		if((segmentProjection1 < yabpProjectionMin && segmentProjection2 < yabpProjectionMin) ||
		   (segmentProjection1 > yabpProjectionMax && segmentProjection2 > yabpProjectionMax))
		   return false;

		// Check segment axis
		normal = segRadius.normalize().getRightNormal();
		float point = normal * segment.getCenter();

		// We need project c +/- yr +/- sr to n. Instead, we'll project min: c - yr - abs(sr), and max: c + yr +abs(sr), to (abs)n
		Vector yabpRadius = yabp.getSlopedRadius() + yRadius;
		Vector absNormal = normal.absolute();
		Vector absSlopedRadius = Vector(abs(yabp.getSlopedRadiusVx()), abs(yabp.getSlopedRadiusVy()));
		float max = normal * yabp.getCenter() + absSlopedRadius * absNormal + yRadius * absNormal;
		float min = normal * yabp.getCenter() - absSlopedRadius * absNormal - yRadius * absNormal;
		if(point < min || point > max) return false;
		return true;
	}
	
	bool slopedAxisIntersects(const YABP& yabp1, const YABP& yabp2)
	{
		Vector normal = yabp1.getSlopedRadius().normalize().getRightNormal();
		Vector yRadius1 = yabp1.getYVector();
		Vector yabpMinPoint1 = yabp1.getCenter() + yRadius1;
		Vector yabpMaxPoint1 = yabp1.getCenter() - yRadius1;
		float yabpMinProjection1 =   normal * yabpMinPoint1;
		float yabpMaxProjection1 =   normal * yabpMaxPoint1;

		// We need project c +/- yr +/- sr to n. Instead, we'll project min: c - yr - abs(sr), and max: c + yr +abs(sr), to (abs)n
		Vector absNormal = normal.absolute();
		Vector yRadius2 = yabp2.getYVector();
		Vector yabpRadius2 = yabp2.getSlopedRadius() + yRadius2;
		Vector absSlopedRadius2 = Vector(abs(yabp2.getSlopedRadius().getX()), abs(yabp2.getSlopedRadius().getY()));
		float yabpMinProjection2 = normal * yabp2.getCenter() - absSlopedRadius2 * absNormal - yRadius2 * absNormal;
		float yabpMaxProjection2 = normal * yabp2.getCenter()  + absSlopedRadius2 * absNormal + yRadius2 * absNormal;
		
		return !((yabpMinProjection2 < yabpMinProjection1 && yabpMaxProjection2 < yabpMinProjection1) ||
				 (yabpMinProjection2 > yabpMaxProjection1 && yabpMaxProjection2 > yabpMaxProjection1));
	}

	bool intersects(const YABP& yabp1, const YABP& yabp2)
	{
		// Check x axis (parallel to y)
		float delta = yabp1.getCenterX() - yabp2.getCenterX();
		if(yabp1.getSlopedRadiusVx() + yabp2.getSlopedRadiusVx() < abs(delta)) return false;

		// Check first YABP sloped axis
		if(!slopedAxisIntersects(yabp1, yabp2))
			return false;

		// Check second YABP sloped axis
		if(!slopedAxisIntersects(yabp2, yabp1))
			return false;

		return true;
	}

	bool intersects(const YABP& yabp, const AABB& aabb)
	{
		// Check x and y radiuses
		float delta = yabp.getCenterX() - aabb.getCenterX();
		if(yabp.getSlopedRadiusVx() + aabb.getRadiusVx()  < abs(delta)) return false;
		delta = yabp.getCenterY() - aabb.getCenterY();
		if(abs(yabp.getSlopedRadiusVy()) + yabp.getYRadius() + aabb.getRadiusVy()  < abs(delta)) return false;

		// Check sloped axis
		Vector normal = yabp.getSlopedRadius().normalize().getRightNormal();
		Vector yRadius = yabp.getYVector();
		Vector yabpPointMin = yabp.getCenter() + yRadius;
		Vector yabpPointMax = yabp.getCenter() - yRadius;
		float yabpProjectionMin =   normal * yabpPointMin;
		float yabpProjectionMax =   normal * yabpPointMax;

		// We need project c +/- r to n. Instead, we'll project min: c - r, and max: c + r, to (abs)n
		Vector absNormal = normal.absolute();
		float aabbProjectionMin = normal * aabb.getCenter() - absNormal * aabb.getRadius();
		float aabbProjectionMax = normal * aabb.getCenter() + absNormal * aabb.getRadius();
		if((aabbProjectionMin < yabpProjectionMin && aabbProjectionMax < yabpProjectionMin) ||
		   (aabbProjectionMin > yabpProjectionMax && aabbProjectionMax > yabpProjectionMax))
		   return false;
		return true;
	}

	bool intersects(const Shape& shape1, const Shape& shape2, Vector* correction)
	{
		const AABB& rect = static_cast<const AABB&>(shape1);
		if(shape2.getType() == ShapeType::AABB)
		{
			const AABB& rect2 = static_cast<const AABB&>(shape2);
			return intersects(rect, rect2, correction);
		}
		else if(shape2.getType() == ShapeType::SEGMENT)
		{
			const Segment& seg = static_cast<const Segment&>(shape2);
			return intersects(rect, seg, correction);
		}
		else
		{
			// ERROR
			exit(1);
		}
	}

	bool intersects(const DirectedSegment& seg, const Shape& shape, Vector* pointOfIntersection, float* distance)
	{
		if(shape.getType() == ShapeType::AABB)
		{
			const AABB& rect = static_cast<const AABB&>(shape);
			return intersects(seg, rect, pointOfIntersection, distance);
		}
		else if(shape.getType() == ShapeType::SEGMENT)
		{
			const Segment& seg2 = static_cast<const Segment&>(shape);
			return intersects(seg, seg2, pointOfIntersection, distance);
		}
		else
		{
			// ERROR
			exit(1);
		}
	}

	bool intersects(const YABP& yabp, const Shape& shape)
	{
		if(shape.getType() == ShapeType::AABB)
		{
			const AABB& rect = static_cast<const AABB&>(shape);
			return intersects(yabp, rect);
		}
		else if(shape.getType() == ShapeType::SEGMENT)
		{
			const Segment& seg = static_cast<const Segment&>(shape);
			return intersects(yabp, seg);
		}
		else
		{
			// ERROR
			exit(1);
		}
	}
}