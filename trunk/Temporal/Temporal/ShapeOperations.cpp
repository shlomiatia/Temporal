#include "ShapeOperations.h"

#include "Shapes.h"
#include "Math.h"
#include <math.h>
#include <algorithm>

namespace Temporal
{
	// Check first YABP sloped axis
	bool slopedAxisIntersects(const YABP& yabp1, const YABP& yabp2, Vector* correction, bool flip)
	{
		Vector normal = yabp1.getSlopedRadius().normalize().getLeftNormal();

		// TODO:
		if(normal == Vector::Zero)
			return true;

		Vector yabpMinPoint1 = yabp1.getCenter() - yabp1.getYVector();
		Vector yabpMaxPoint1 = yabp1.getCenter() + yabp1.getYVector();

		float yabpMinProjection1 = normal * yabpMinPoint1;
		float yabpMaxProjection1 = normal * yabpMaxPoint1;

		float center1 = (yabpMaxProjection1 + yabpMinProjection1) / 2.0f;
		float radius1 = (yabpMaxProjection1 - yabpMinProjection1) / 2.0f;

		// We need project c +/- yr +/- sr to n. Instead, we'll project  c * n +/- (abs(sr * n) + yr * abs(n))
		float centerProjection2 = yabp2.getCenter() * normal;
		float slopedRadiusProjection2 = abs(yabp2.getSlopedRadius() * normal);
		float yRadiusProjection2 = yabp2.getYVector() * normal.absolute();

		float yabpMinProjection2 = centerProjection2 - slopedRadiusProjection2 - yRadiusProjection2;
		float yabpMaxProjection2 = centerProjection2  + slopedRadiusProjection2 + yRadiusProjection2;
		
		float center2 = (yabpMaxProjection2 + yabpMinProjection2) / 2.0f;
		float radius2 = (yabpMaxProjection2 - yabpMinProjection2) / 2.0f;

		float delta = center1 - center2;
		float penetration = radius1 + radius2 - abs(delta);
		if(penetration < 0.0f) return false;

		if(correction && penetration < correction->getLength())
		{
			*correction = normal * ((delta < 0.0f) ^ flip ? -penetration : penetration);
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

		if(!slopedAxisIntersects(yabp1, yabp2, correction, false))
			return false;

		if(!slopedAxisIntersects(yabp2, yabp1, correction, true))
			return false;

		return true;
	}

	bool slopedAxisIntersects(float direction, float origin, float min, float max, float& tmin, float& tmax)
	{
		if (abs(direction) < EPSILON) 
		{
			// Ray is parallel to slab. No hit if origin not within slab
			if (origin < min || origin > max) 
				return false;
		} 
		else 
		{
			// Compute intersection t value of ray with near and far plane of slab
			float ood = 1.0f / direction;
			float t1 = (min - origin) * ood;
			float t2 = (max - origin) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2) std::swap(t1, t2);
			// Compute the intersection of slab intersection intervals
			if (t1 > tmin) tmin = t1;
			if (t2 < tmax) tmax = t2;
			// Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax) 
				return false;
		}

		return true;
	}

	bool intersects(const DirectedSegment& seg, const YABP& yabp, Vector* pointOfIntersection, float* distance)
	{
		float tmin = 0.0f; // set to -FLT_MAX to get first hit on line
		const Vector& origin = seg.getOrigin();
		const Vector& vector = seg.getVector();
		const Vector direction = vector.normalize();
		float tmax = vector.getLength();

		if(!slopedAxisIntersects(direction.getX(), origin.getX(), yabp.getLeft(), yabp.getRight(), tmin, tmax))
			return false;
		
		// Translate yabp to origin with ray, and rotate them so sloped axes is parallel to x
		Vector relativeOrigin = origin - yabp.getCenter();
		float angle = -yabp.getSlopedRadius().getAngle();
		float cosAngle = cos(angle);
		float sinAngle = sin(angle);
		float rotatedOriginY = relativeOrigin.getX() * sinAngle + relativeOrigin.getY() * cosAngle;
		float rotatedDirectionY = direction.getX() * sinAngle + direction.getY() * cosAngle;

		if(!slopedAxisIntersects(rotatedDirectionY, rotatedOriginY, -yabp.getYRadius(), yabp.getYRadius(), tmin, tmax))
			return false;
		
		if(pointOfIntersection)
			*pointOfIntersection = origin + direction * tmin;
		if(distance)
			*distance = tmin;
		return true;
	}

	bool intersects(const YABP& yabp, const Vector& point)
	{
		if(abs(yabp.getCenter().getX() - point.getX()) - yabp.getSlopedRadius().getX() > EPSILON)
			return false;

		Vector normal = yabp.getSlopedRadius().normalize().getLeftNormal();

		Vector yabpMinPoint1 = yabp.getCenter() - yabp.getYVector();
		Vector yabpMaxPoint1 = yabp.getCenter() + yabp.getYVector();

		float yabpMinProjection1 = normal * yabpMinPoint1;
		float yabpMaxProjection1 = normal * yabpMaxPoint1;

		float center1 = (yabpMaxProjection1 + yabpMinProjection1) / 2.0f;
		float radius = (yabpMaxProjection1 - yabpMinProjection1) / 2.0f;

		float center2 = normal * point;

		float delta = center1 - center2;
		float penetration = radius - abs(delta);
		if(penetration < 0.0f) return false;

		return true;
	}
}