#include "Segment.h"
#include <math.h>

namespace Temporal
{
	const Segment Segment::Empty(Point::Zero, Point::Zero);

	float Segment::getLength(void) const
	{
		return sqrt(pow(getPoint2().getX() - getPoint1().getX(), 2.0f) + pow(getPoint2().getY() - getPoint1().getY(), 2.0f));
	}

	float DirectedSegment::getAngle(void) const
	{
		Vector vector = getVector();
		return atan2(vector.getVy(), vector.getVx());
	}

	Vector DirectedSegment::getVector(void) const
	{
		return getTarget() - getOrigin();
	}

	// Returns 2 times the signed triangle area. The result is positive if
	// abc is ccw, negative if abc is cw, zero if abc is degenerate.
	float signed2DTriArea(Point a, Point b, Point c)
	{
		return (a.getX() - c.getX()) * (b.getY() - c.getY()) - (a.getY() - c.getY()) * (b.getX() - c.getX());
	}

	bool Segment::intersects(const Segment& other, Point& pointOfIntersection) const
	{
		// Sign of areas correspond to which side of ab points c and d are
		float a1 = signed2DTriArea(getPoint1(), getPoint2(), other.getPoint2()); // Compute winding of abd (+ or -)
		float a2 = signed2DTriArea(getPoint1(), getPoint2(), other.getPoint1()); // To intersect, must have sign opposite of a1
		// If c and d are on different sides of ab, areas have different signs
		if (a1 * a2 < 0.0f) 
		{
			// Compute signs for a and b with respect to segment cd
			float a3 = signed2DTriArea(other.getPoint1(), other.getPoint2(), getPoint1()); // Compute winding of cda (+ or -)
			// Since area is constant a1 - a2 = a3 - a4, or a4 = a3 + a2 - a1
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
				pointOfIntersection = getPoint1() + t * (getPoint2() - getPoint1());
				return true;
			}
		}
		return false;
	}
}