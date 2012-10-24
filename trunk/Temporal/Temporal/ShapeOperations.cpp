#include "ShapeOperations.h"

#include "Shapes.h"
#include "Math.h"
#include <math.h>
#include <algorithm>

namespace Temporal
{
	bool slopedAxisIntersects(const YABP& yabp1, const YABP& yabp2, Vector* correction, bool flip)
	{
		Vector normal = yabp1.getSlopedRadius().normalize().getLeftNormal();
		Vector yRadius1 = yabp1.getYVector();
		Vector yabpMinPoint1 = yabp1.getCenter() - yRadius1;
		Vector yabpMaxPoint1 = yabp1.getCenter() + yRadius1;
		float yabpMinProjection1 =   normal * yabpMinPoint1;
		float yabpMaxProjection1 =   normal * yabpMaxPoint1;

		float center1 = (yabpMaxProjection1 + yabpMinProjection1) / 2.0f;
		float radius1 = (yabpMaxProjection1 - yabpMinProjection1) / 2.0f;

		// We need project c +/- yr +/- sr to n. Instead, we'll project  c * n +/- (abs(sr * n) + yr * abs(n)
		// TODO:
		Vector absNormal = normal.absolute();
		Vector yRadius2 = yabp2.getYVector();
		Vector yabpRadius2 = yabp2.getSlopedRadius();
		float c = yabp2.getCenter() * normal;
		float s = abs(yabpRadius2 * normal);
		float y = yRadius2 * absNormal;

		float yabpMinProjection2 = c - abs(s) - y;
		float yabpMaxProjection2 = c  + abs(s) + y;
		
		float center2 = (yabpMaxProjection2 + yabpMinProjection2) / 2.0f;
		float radius2 = (yabpMaxProjection2 - yabpMinProjection2) / 2.0f;

		float delta = center1 - center2;
		float penetration = radius1 + radius2 - abs(delta);
		if(penetration < 0.0f) return false;

		if(correction && penetration < correction->getLength())
		{
			*correction = normal * (delta < 0.0f ^ flip ? -penetration : penetration);
		}
		return true;
	}
	
	bool intersects(const YABP& yabp1, const YABP& yabp2, Vector* correction)
	{

		// Check x axis (parallel to y)
		float delta = yabp1.getCenterX() - yabp2.getCenterX();
		float penetration = yabp1.getSlopedRadiusVx() + yabp2.getSlopedRadiusVx() - abs(delta);
		if(penetration < 0.0f) return false;

		if(correction)
		{
			*correction = Vector(delta < 0.0f ? -penetration : penetration, 0.0f);
		}

		// Check first YABP sloped axis
		if(!slopedAxisIntersects(yabp1, yabp2, correction, false))
			return false;

		// Check second YABP sloped axis
		if(!slopedAxisIntersects(yabp2, yabp1, correction, true))
			return false;

		return true;
	}

	bool intersects(const DirectedSegment& seg, const YABP& yabp, Vector* pointOfIntersection, float* distance)
	{
		float tmin = 0.0f; // set to -FLT_MAX to get first hit on line
		const Vector& origin = seg.getOrigin();
		Vector vector = seg.getVector();
		const Vector direction = vector.normalize();
		float tmax = vector.getLength();

		float originX = origin.getX();
		float directionX = direction.getX();
		if (abs(directionX) < EPSILON) 
		{
			// Ray is parallel to slab. No hit if origin not within slab
			if (originX < yabp.getLeft() || originX > yabp.getRight()) 
				return false;
		} 
		else 
		{
			// Compute intersection t value of ray with near and far plane of slab
			float ood = 1.0f / directionX;
			float t1 = (yabp.getLeft() - originX) * ood;
			float t2 = (yabp.getRight() - originX) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2) std::swap(t1, t2);
			// Compute the intersection of slab intersection intervals
			if (t1 > tmin) tmin = t1;
			if (t2 < tmax) tmax = t2;
			// Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax) return false;
		}
		
		// TODO:
		Vector relativeOrigin = origin - yabp.getCenter();
		float angle = yabp.getSlopedRadius().getAngle();
		float cosAngle = cos(-angle);
		float sinAngle = sin(-angle);
		//var nx = c * v.x - s * v.y;
		//var ny = s * v.x + c * v.y;
		float rotatedOriginY = relativeOrigin.getX() * sinAngle + relativeOrigin.getY() * cosAngle;
		float rotatedDirectionY = direction.getX() * sinAngle + direction.getY() * cosAngle;
		if (abs(rotatedDirectionY) < EPSILON) 
		{
			// Ray is parallel to slab. No hit if origin not within slab
			if (rotatedOriginY < -yabp.getYRadius() || rotatedOriginY > yabp.getYRadius()) 
				return false;
		} 
		else 
		{
			// Compute intersection t value of ray with near and far plane of slab
			float ood = 1.0f / rotatedDirectionY;
			float t1 = (-yabp.getYRadius() - rotatedOriginY) * ood;
			float t2 = (yabp.getYRadius() - rotatedOriginY) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2) std::swap(t1, t2);
			// Compute the intersection of slab intersection intervals
			if (t1 > tmin) tmin = t1;
			if (t2 < tmax) tmax = t2;
			// Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax) return false;
		}
		if(pointOfIntersection)
			*pointOfIntersection = origin + direction * tmin;
		if(distance)
			*distance = tmin;
		return true;
	}
}