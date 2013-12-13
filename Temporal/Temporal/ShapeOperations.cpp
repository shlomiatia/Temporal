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

	bool clip(float denom, float numer, float& t0, float& t1)
	{
		// Return value is 'true' if line rayment intersects the current test
		// plane.  Otherwise 'false' is returned in which case the line rayment
		// is entirely clipped.

		if (denom > 0.0f)
		{
			if (numer > denom*t1)
			{
				return false;
			}
			if (numer > denom*t0)
			{
				t0 = numer/denom;
			}
			return true;
		}
		else if (denom < 0.0f)
		{
			if (numer > denom*t0)
			{
				return false;
			}
			if (numer > denom*t1)
			{
				t1 = numer/denom;
			}
			return true;
		}
		else
		{
			return numer <= 0.0f;
		}
	}
		
	bool intersects(const DirectedSegment& ray, const OBB& obb, Vector* pointOfIntersection, float* distance)
	{
		float t0 = 0.0f;
		float t1 = ray.getVector().getLength();
		// Convert linear component to obb coordinates.
		Vector diff = ray.getOrigin() - obb.getCenter();
		Vector BOrigin(
			diff * (obb.getAxis(Axis::X)),
			diff * (obb.getAxis(Axis::Y))
		);
		Vector BDirection(
			ray.getDirection() * (obb.getAxis(Axis::X)),
			ray.getDirection() * (obb.getAxis(Axis::Y))
		);

		float saveT0 = t0, saveT1 = t1;
		bool notAllClipped =
			clip(+BDirection.getX(), -BOrigin.getX()-obb.getRadius().getX(), t0, t1) &&
			clip(-BDirection.getX(), +BOrigin.getX()-obb.getRadius().getX(), t0, t1) &&
			clip(+BDirection.getY(), -BOrigin.getY()-obb.getRadius().getY(), t0, t1) &&
			clip(-BDirection.getY(), +BOrigin.getY()-obb.getRadius().getY(), t0, t1);

		if (!notAllClipped)
			return false;
		if(pointOfIntersection)
			*pointOfIntersection = ray.getOrigin() + ray.getDirection() * t0;
		if(distance)
			*distance = t0;
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