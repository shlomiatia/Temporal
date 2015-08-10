#include "ShapeOperations.h"
#include "Shapes.h"
#include "Math.h"
#include <limits>

namespace Temporal
{
	bool intersects(const OBB& obb, const Vector& point)
	{
		Vector diff = point - obb.getCenter();

		for (Axis::Enum i = Axis::X; i <= Axis::Y; ++i)
		{
			float dot = fabsf(diff * obb.getAxis(i));
			if (dot > obb.getRadius().getAxis(i))
				return false;
		}

		return true;
	}

	bool intersects(const DirectedSegment& ray, const OBB& obb, Vector* pointOfIntersection, float* distance)
	{
		float maxS = -FLT_MAX;
		float minT = FLT_MAX;

		// compute difference vector
		Vector diff = obb.getCenter() - ray.getOrigin();

		// for each axis do
		for(Axis::Enum i = Axis::X; i <= Axis::Y; i++) 
		{
			// get axis i
			Vector axis = obb.getAxis(i);

			// project relative vector onto axis
			float e = axis * diff;
			float f = ray.getDirection().normalize() * axis;

			float axisRadius = obb.getRadius().getAxis(i);

			// ray is parallel to plane
			if (equals(f, 0.0f))
			{
				// ray passes by box
				if (-e - axisRadius > 0.0f || -e + axisRadius < 0.0f )
					return false;
				continue;
			}

			float s = (e - axisRadius)/f;
			float t = (e + axisRadius)/f;

			// fix order
			if ( s > t )
			{
				float temp = s;
				s = t;
				t = temp;
			}

			// adjust min and max values
			if ( s > maxS )
				maxS = s;
			if ( t < minT )
				minT = t;

			// check for intersection failure
			if ( minT < 0.0f || maxS > minT || maxS > ray.getVector().getLength())
				return false;
		}
		if (maxS < 0.0f)
			maxS = 0.0f;

		if(pointOfIntersection)
			*pointOfIntersection = ray.getOrigin() + ray.getDirection() * maxS;
		if(distance)
			*distance = maxS;

		// done, have intersection
		return true;
	}

	bool intersects(const OBB& obb1, const OBB& obb2, Vector* correction)
	{
		// Convenience variables.
		const Vector* A = obb1.getAxes();
		const Vector* B = obb2.getAxes();
		const Vector* C[2];
		C[0] = A;
		C[1] = B;
		const Vector& EA = obb1.getRadius();
		const Vector& EB = obb2.getRadius();

		// Compute difference of obb centers, D = C1-C0.
		Vector D = obb2.getCenter() - obb1.getCenter();

		float AbsAdB[2][2], AbsAdD, RSum;
	
		// axis C0+t*A0
		AbsAdB[0][0] = fabsf(A[0] * B[0]);
		AbsAdB[0][1] = fabsf(A[0] * B[1]);

		// axis C0+t*A1
		AbsAdB[1][0] = fabsf(A[1] * B[0]);
		AbsAdB[1][1] = fabsf(A[1] * B[1]);

		float minPenetration = std::numeric_limits<float>::max();
		float penetration;

		AbsAdD = fabsf(A[0] * (D));
		RSum = EA.getX() + EB.getX()*AbsAdB[0][0] + EB.getY()*AbsAdB[0][1];
		penetration = RSum - AbsAdD;
		if (penetration < 0.0f)
			return false;
		if(correction && penetration < minPenetration)
		{
			minPenetration = penetration;
			*correction = A[0] * penetration;
		}
			
		AbsAdD = fabsf(A[1] * (D));
		RSum = EA.getY() + EB.getX()*AbsAdB[1][0] + EB.getY()*AbsAdB[1][1];
		penetration = RSum - AbsAdD;
		if (penetration < 0.0f)
			return false;
		if(correction && penetration < minPenetration)
		{
			minPenetration = penetration;
			*correction = A[1] * penetration;
		}

		// axis C0+t*B0
		AbsAdD = fabsf(B[0] * (D));
		RSum = EB.getX() + EA.getX()*AbsAdB[0][0] + EA.getY()*AbsAdB[1][0];
		penetration = RSum - AbsAdD;
		if (penetration < 0.0f)
			return false;
		if(correction && penetration < minPenetration)
		{
			minPenetration = penetration;
			*correction = B[0] * penetration;
		}

		// axis C0+t*B1
		AbsAdD = fabsf(B[1] * (D));
		RSum = EB.getY() + EA.getX()*AbsAdB[0][1] + EA.getY()*AbsAdB[1][1];
		penetration = RSum - AbsAdD;
		if (penetration < 0.0f)
			return false;
		if(correction && penetration < minPenetration)
		{
			minPenetration = penetration;
			*correction = B[1] * penetration;
		}
		if(correction)
		{
			if(correction->getX() > correction->getY() ? sameSign(D.getX(), correction->getX()) : sameSign(D.getY(), correction->getY()))
			{
				*correction = -*correction;
			}
		}

		return true;
	}
}